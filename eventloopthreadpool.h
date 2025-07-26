#pragma once
#include "nocopyable.h"
#include<functional>
#include<string>
#include <vector>
#include <memory>



class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : nocopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){ numThreads_ = numThreads; }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    // 如果工作在多线程中，baseloop默认以轮询的方式分配channel
    EventLoop* getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return started_; }
    const std::string name() { return name_; }

private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};