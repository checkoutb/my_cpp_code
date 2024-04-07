
#include <string>

#include "../include/epoll_poller.h"

const int kNew = -1;    // 某个channel还没添加至Poller
const int kAdded = 1;   // 某个channel已经添加至Poller
const int kDeleted = 2; // 某个channel已经从Poller删除

EpollPoller::EpollPoller(EventLoop* loop) : Poller(loop), m_epollfd(::epoll_create(EPOLL_CLOEXEC)), m_events(kInitEventListSize)
{
    if (m_epollfd < 0)
        LOG_FATAL << "epoll_create fail, errno is " << errno;
}

EpollPoller::~EpollPoller()
{
    ::close(m_epollfd);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    size_t numEvents = ::epoll_wait(m_epollfd, &(*m_events.begin()), static_cast<int>(m_events.size()), timeoutMs);             // ....zzz....
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)  // 有event
    {
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == m_events.size())
        {
            m_events.resize(m_events.size() * 2);       // zzz vector自动扩容，这里有必要提前扩容吗？ 还有，这里是单线程，所以 不会有并发问题.. 是单线程吧？应该是
        }
    }
    else if (numEvents == 0)    // timeout
    {
        LOG_DEBUG << "epoll wait timeout";
    }
    else
    {   // error
        if (saveErrno != EINTR)
        {
            errno = saveErrno;
            LOG_ERROR << "epollpoller::poll failed";
        }
    }
    return now;
}

 // * Channel::update => EventLoop::updateChannel => Poller::updateChannel
 // * Channel::remove => EventLoop::removeChannel => Poller::removeChannel
void EpollPoller::updateChannel(Channel* channel)
{
    const int index = channel->getIndex();

    if (index == kNew || index == kDeleted)
    {
        if (index == kNew)
        {
            int fd = channel->getfd();
            m_channels[fd] = channel;
        }
        else
        {

        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(m_events[i].data.ptr);
        channel->setRevents(m_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::removeChannel(Channel* channel)
{
    int fd = channel->getfd();
    m_channels.erase(fd);

    int index = channel->getIndex();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}


void EpollPoller::update(int operation, Channel* channel)
{
    epoll_event event;
    ::memset(&event, 0, sizeof(event));

    int fd = channel->getfd();
    event.events = channel->getEvents();
    event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(m_epollfd, operation, fd, &event) < 0)      // ....zzz...在
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR << "epoll_ctl del error, errno: " << errno;
        }
        else
        {
            LOG_FATAL << "epoll_ctl add/mod error, errno: " << errno;
        }
    }
}












