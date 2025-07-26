#include "eventloop.h"
#include "logger.h"
#include "poller.h"
#include "channel.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// 防止一个线程创建多个EventLoop
__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000; // 定义默认的poll超时时间


// 创建wakeupfd, 用来notify subLoop
// 主要作用，当mainLoop获取一个新用户的channel时，通过轮询的方式
int createEventfd() {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG_FATAL("eventfd error: %d", errno);
    }
    return fd;
}


EventLoop::EventLoop()
: looping_(false)
, quit_(false)
, callingPendingFunctors_(false)
, threadId_(CurrentThread::tid())
, poller_(Poller::newDefaultPoller(this))
, wakeupFd_(createEventfd())
, wakeupChannel_(new Channel(this, wakeupFd_))
, currentActiveChannel_(nullptr)
{
    LOG_DEBUG("EventLoop created in thread %d", threadId_);
    if (t_loopInThisThread) {
        LOG_FATAL("Another EventLoop exists in this thread %d", threadId_);
    } else {
        t_loopInThisThread = this;
    }

    // 设置wakeup的事件类型以及发生时间后的回调操作
    wakeupChannel_->setReadCallback(
        std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading(); // 启用读事件
}

EventLoop::~EventLoop() {
    LOG_DEBUG("EventLoop destruct in thread %d", threadId_);
    wakeupChannel_->disableAll();
    wakeupChannel_->remove(); // 从Poller中移除
    close(wakeupFd_);
    t_loopInThisThread = nullptr; // 清除当前线程的EventLoop指针
}


void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::handleRead() reads %zd bytes instead of 8", n);
    }
}

// 开启事件循环
void EventLoop::loop() {
    looping_ = true;
    quit_ = false;
    LOG_INFO("EventLoop %p start looping", this);

    while (!quit_) {
       activeChannels_.clear();
         // poller_的poll方法会阻塞，直到有事件发生或者超时
       pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

       for(Channel* channel: activeChannels_){
            channel->handleEvent(pollReturnTime_);
       }
       // 执行当前EventLoop事件循环需要处理的回调操作
       doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping", this);
    looping_ = false;
}


void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if (!isInLoopThread() || callingPendingFunctors_ ) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor& functor : functors) {
        functor();
    }

    callingPendingFunctors_ = false;
}

void EventLoop::updateChannel(Channel* channel) {
    poller_->updateChannel(channel);
}


void EventLoop::removeChannel(Channel* channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) const {
    return poller_->hasChannel(channel);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::wakeup() writes %zd bytes instead of 8", n);
    }
}


void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup(); // 唤醒EventLoop线程
    }
}
