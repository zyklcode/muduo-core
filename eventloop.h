#pragma once
#include "nocopyable.h"
#include "timestamp.h"
#include "currentthread.h"
#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>


class Channel; // 前向声明
class Poller;


// 事件循环类，主要包含了两个大模块Channel（fd和fd关注的事件）和Poller(epoll的抽象)
// Channel负责处理IO事件，Poller负责多路复用
class EventLoop: nocopyable {
public:
    using Functor = std::function<void()>;
    EventLoop();
    ~EventLoop();

    void loop(); // 开始事件循环
    void quit(); // 退出事件循环
    Timestamp pollReturnTime() const { return pollReturnTime_; } // 获取上次poll的返回时间

    void runInLoop(Functor cb); // 在当前loop中执行函数
    void queueInLoop(Functor cb); // 把cb放入队列中，唤醒loop线程执行

    void wakeup(); // 唤醒事件循环
    // EventLoop的方法 =》 Poller的方法
    void updateChannel(Channel* channel); // 更新Channel
    void removeChannel(Channel* channel); // 移除Channel
    bool hasChannel(Channel* channel) const; // 检查Channel是否存在

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); } // 检查是否在当前线程


private:
    void handleRead(); // 处理读事件
    void doPendingFunctors(); // 执行回调

    using ChannelList = std::vector<Channel*>;

    std::atomic<bool> looping_;     // 是否正在循环
    std::atomic<bool> quit_;        // 是否退出循环
    

    const pid_t threadId_;         // 线程ID，确保EventLoop只能在创建它的线程中使用
    std::unique_ptr<Poller> poller_;               // Poller对象，负责多路复用
    Timestamp pollReturnTime_; // 上次poll的返回时间

    int wakeupFd_; // 主要作用，当mainLoop获取一个新用户的channel时，通过轮询的方式，通知subLoop处理新用户的channel
    std::unique_ptr<Channel> wakeupChannel_; // 唤醒通道，用于唤醒EventLoop

    ChannelList activeChannels_;    // 保存所有活跃的Channel
    Channel* currentActiveChannel_; // 当前活跃的Channel

    std::atomic<bool> eventHandling_; // 是否正在处理事件
    std::atomic<bool> callingPendingFunctors_; // 表示当前的loop是否正在调用PendingFunctors
    std::vector<Functor> pendingFunctors_; // 存储待执行的函数
    std::mutex mutex_; // 保护pendingFunctors_的互斥锁
};