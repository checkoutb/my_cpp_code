#pragma once

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>

#include "../include/logging.h"
#include "../include/poller.h"
#include "../include/timestamp.h"


class EpollPoller : public Poller
{
    using EventList = std::vector<epoll_event>;     //zzz

public:
    EpollPoller(EventLoop*);
    ~EpollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList * activeChannels) override;
    void updateChannel(Channel * channel) override;
    void removeChannel(Channel * channel) override;

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    int m_epollfd;
    EventList m_events;
};





