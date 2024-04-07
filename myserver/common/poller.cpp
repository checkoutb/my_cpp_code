
#include "../include/poller.h"



Poller::Poller(EventLoop* loop) : m_ownerLoop {loop}
{}


bool Poller::hasChannel(Channel* channel) const
{
    auto it = m_channels.find(channel->getfd());
    return it != m_channels.end() && it->second == channel;
}
