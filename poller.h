#pragma once
#include "nocopyable.h"
#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;
class Timestamp;

// muduo库中多路时间分发器的核心：IO复用模块的抽象
class Poller: nocopyable{
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    // 给所有的IO复用保留通义的接口
    virtual Timestamp poll(int timeout, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    // 判断参数channel是否在当前poller当中
    bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);


protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop* loop_;
};