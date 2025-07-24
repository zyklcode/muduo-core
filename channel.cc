#include "channel.h"
#include "eventloop.h"
#include "logger.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;                  // 没有事件
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI; // 读事件
const int Channel::kWriteEvent = EPOLLOUT;          // 写事件

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(kNoneEvent), revents_(kNoneEvent), tied_(false), index_(-1)
{
}

Channel::~Channel()
{
}

// Channel的tie方法什么时候调用过
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::update()
{
    // 更新Channel到EventLoop
    loop_->updateChannel(this);
}

void Channel::remove()
{
    // 从EventLoop中移除Channel
    loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    LOG_INFO("Channel::handleEvent() fd = %d, events = %d, revents = %d", fd_, events_, revents_);
    if (tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

// 根据实际发生的事件，执行相应的回调
// 这里的receiveTime是Poller通知Channel时的时间戳
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    // 先处理错误事件
    if(revents_ & EPOLLERR){
        if(errorCallback_)
        {
            errorCallback_();  
        }
        return;
    }
    // 再处理关闭事件
    if(revents_ & (EPOLLHUP | EPOLLRDHUP))
    {
        // 先判断是否可读
        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDNORM))
        {
            if (readCallback_)
            {
                readCallback_(receiveTime);
            }
        }

        if(closeCallback_)
        {
            closeCallback_();
        }
        return;
    }

    // 写事件
    if(revents_ & EPOLLOUT)
    {
        if(writeCallback_)
        {
            writeCallback_();
        }
    }

    // 读事件
    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDNORM))
    {
        if(readCallback_)
        {
            readCallback_(receiveTime);
        }
    }
}