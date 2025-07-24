#include "epollpoller.h"
#include "logger.h"
#include "channel.h"
#include <errno.h>
#include <unistd.h>
#include <cstring>

// channel未添加到Poller中
const int kNew = -1;
// channel已添加到Poller中
const int kAdded = 1;
// channel已删除到Poller中
const int kDeleted = 2;
EpollPoller::EpollPoller(EventLoop* loop)
: Poller(loop)
, epollfd_(epoll_create1(EPOLL_CLOEXEC))
, events_(kInitEventListSize)
{
    if(epollfd_ < 0){
        LOG_FATAL("epoll_create1 error: %d", errno);
    }
}

EpollPoller::~EpollPoller(){
    close(epollfd_);
}


Timestamp EpollPoller::poll(int timeout, ChannelList* activeChannels){
    // 实际上应该用LOG_DEBUG输出日志更为合理
    LOG_INFO("func=%s => fd total count:%d", __FUNCTION__, channels_.size());
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeout);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents > 0){
        LOG_INFO("%d events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size()){
            events_.resize(events_.size() * 2);
        }
    }else if(numEvents == 0){
        LOG_DEBUG("%s timeout!", __FUNCTION__);
    }else{
        if(saveErrno != EINTR){
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!");
        }
    }
    return now
}


/*
*           EventLoop
    ChannelList             Poller
                    ChannelMap<fd, channel*>
*/
void EpollPoller::updateChannel(Channel* channel){
    int status = channel->index();
    LOG_INFO("func=%s => fd=%d", __FUNCTION__, channel->fd());
    if(status == kNew || status == kDeleted){
        if(status == kNew){
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }else{  // channel已经在poller上注册过了
        int fd = channel->fd();
        if(channel->isNoneEvent()){
            channel->set_index(kDeleted);
            update(EPOLL_CTL_DEL, channel);
        }else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

// 从Poller中删除channel
void EpollPoller::removeChannel(Channel* channel){
    LOG_INFO("func=%s => fd=%d", __FUNCTION__, channel->fd());
    int fd = channel->fd();
    int status = channel->index();
    channels_.erase(fd);
    if(status == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);

}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const{
    for(int i=0;i<numEvents;i++){
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);         // EventLoop就拿到了他的poller给他返回的所有发生事件的channel列表了
    }
}
void EpollPoller::update(int operation, Channel* channel){
    struct epoll_event event;
    memset(&event, sizeof(event), 0);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if(epoll_ctl(epollfd_, operation, fd, &event) < 0){
        if(operation == EPOLL_CTL_DEL){
            LOG_ERROR("epoll_ctl del error: %d", errno);
        }else{
            LOG_FATAL("epoll_ctl add/mod error: %d", errno);
        }
    }
}