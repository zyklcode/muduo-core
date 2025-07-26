#pragma once

// #include "nocopyable.h"
#include "thread.h"
#include <functional>
#include <mutex>
#include <condition_variable>
#include <string>


class EventLoop;

class EventLoopThread: nocopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThread(const ThreadInitCallback& cb, const std::string& name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop(); // 启动事件循环线程并返回EventLoop指针

private:
    void threadFunc(); // 线程函数，执行事件循环

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};