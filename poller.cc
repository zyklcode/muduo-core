#include "poller.h"
#include "channel.h"

Poller::Poller(EventLoop *loop)
    : loop_(loop)
{
}

bool Poller::hasChannel(Channel *channel) const
{
    if (auto it = channels_.find(channel->fd()); it != channels_.end())
    {
        return true;
    }
    return false;
}