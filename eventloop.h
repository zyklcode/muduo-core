#pragma once

class Channel; // 前向声明
// 事件循环类，主要包含了两个大模块Channel（fd和fd关注的事件）和Poller(epoll的抽象)
// Channel负责处理IO事件，Poller负责多路复用
class EventLoop{
public:
    // 更新Channel到Poller
    void updateChannel(Channel* channel);  
private:
};