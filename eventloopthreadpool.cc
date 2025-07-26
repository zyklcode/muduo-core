#include "eventloopthreadpool.h"
#include "eventloopthread.h"
#include "eventloop.h"
#include "logger.h"



EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
    : baseLoop_(baseLoop)
    , name_(name)
    , started_(false)
    , numThreads_(0)
    , next_(0)
    , loops_(0)
    , threads_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
    started_ = true;
    for(int i=0;i<numThreads_;++i){
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%s%d", name_.c_str(), i);
        std::unique_ptr<EventLoopThread> thread(new EventLoopThread(cb, buffer));
        threads_.push_back(std::move(thread));
        loops_.push_back(thread->startLoop());
    }

    // 服务端只有一个线程，运行着baseLoop_
    if(numThreads_ == 0 && cb){
        cb(baseLoop_);
    }
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops(){
    if(loops_.empty() ){
        return std::vector<EventLoop*>(1, baseLoop_);
    }else{
        return loops_;
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    EventLoop* loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        ++next_;
        if(next_ >= loops_.size()){
            next_ = 0;
        }
    }
    return loop;
}