#pragma once
#include "nocopyable.h"
#include "timestamp.h"
#include <functional>
#include <memory>

class EventLoop;
/*
* Channel理解为通道，封装了sockfd和其感兴趣的event,如EPOLLIN, EPOLLOUT等。
* 还绑定了poller返回的事件。
*/
class Channel: nocopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallBack = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // fd_得到poller通知后，处理事件
    void handleEvent(Timestamp receiveTime);

    // 设置读回调
    void setReadCallback(ReadEventCallBack cb){readCallback_ = std::move(cb);}
    // 设置写回调
    void setWriteCallback(EventCallback cb){writeCallback_ = std::move(cb);}
    // 设置关闭回调
    void setCloseCallback(EventCallback cb){closeCallback_ = std::move(cb);}
    // 设置错误回调
    void setErrorCallback(EventCallback cb){errorCallback_ = std::move(cb);}

    // 防止当channel被手动remove时，channel还在执行回调
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; } // 获取fd
    int events() const { return events_; } // 获取感兴趣的事件

    void set_revents(int revt) { revents_ = revt; } // 设置实际发生的事件

    void enableReading() { events_ |= kReadEvent; update();} // 启用读事件
    void disableReading() { events_ &= ~kReadEvent; update();} // 禁用读事件
    void enableWriting() { events_ |= kWriteEvent; update();} // 启用写事件
    void disableWriting() { events_ &= ~kWriteEvent; update();} // 禁用写事件
    void disableAll() { events_ = kNoneEvent; update();} // 禁用所有 

    // 返回fd_当前的事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; } // 是否没有事件
    bool isWriting() const { return events_ & kWriteEvent; } // 是否有写事件
    bool isReading() const { return events_ & kReadEvent; } // 是否有

    int index() const { return index_; } // 获取索引
    void set_index(int idx) { index_ = idx; } // 设置索引


    // one loop per thread
    EventLoop* ownerLoop() { return loop_; } // 获取所属的EventLoop
    void remove(); // 从EventLoop中移除


private:
    void update(); // 更新Channel到EventLoop
    void handleEventWithGuard(Timestamp receiveTime); // 处理事件，防止Channel被提前销毁
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    int fd_;            // Poller监听的对象
    int events_;         // 注册fd_感兴趣的事件
    int revents_;       // 实际fd_发生的事件

    std::weak_ptr<void> tie_; // 用于防止Channel被提前销毁
    bool tied_;        // 是否绑定了tie_

    int index_;        // 用于标识Channel的状态

    // 由于Channel对象知道当前fd_发生的事件，所以可以执行相应的回调
    ReadEventCallBack readCallback_; // 读事件回调
    EventCallback writeCallback_;    // 写事件回调
    EventCallback closeCallback_;    // 关闭事件回调
    EventCallback errorCallback_;    // 错误事件回调
};