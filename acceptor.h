#pragma once

#include "nocopyable.h"
#include "socket.h"
#include "channel.h"
#include <functional>

class EventLoop;
class InetAddr;

class Acceptor: nocopyable{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddr&)>;
    Acceptor(EventLoop* loop, const InetAddr& addr, bool reuseport = false);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) {
        newConnectionCallback_ = cb;
    }

    bool listening() const { return listening_; } // 是否在监听

    void listen(); // 开始监听
    

private:
    void handleRead();
    EventLoop* loop_;       // 用户定义的mainloop, 也就是用户定义的baseloop
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;

};