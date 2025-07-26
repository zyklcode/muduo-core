#include "eventloopthread.h"
#include "eventloop.h"


EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc, this), name)
    , callback_(cb)
    , mutex_()
    , cond_()
{
    // 启动线程时，线程会执行threadFunc函数
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}


EventLoop* EventLoopThread::startLoop()
{
    thread_.start();
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop = loop_;
        cond_.wait(lock, [this]() { return loop_ != nullptr; });
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop; // 创建EventLoop对象
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop; // 设置loop_为当前线程的EventLoop对象
        cond_.notify_one(); // 通知主线程EventLoop已经创建完成
    }

    loop.loop(); // 开始事件循环
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr; // 事件循环结束后，清除loop_指针
    }
    exiting_ = true; // 标记线程正在退出
}