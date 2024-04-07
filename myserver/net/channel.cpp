
#include "../include/channel.h"
#include "../include/event_loop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : m_eLoop {loop}, m_fd {fd}, m_events {0}, m_revents {0}, m_index {-1}, m_tied {false}
{}

Channel::~Channel()
{
    //TODO:析构操作和断言，判断是否是在当前线程
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    m_tie = obj;    // zzz weak_ptr be assigned shared_ptr
    m_tied = true;
}

void Channel::update()
{
    //TODO:Channel::update()
    m_eLoop->updateChannel(this);
}

void Channel::remove()
{
    //TODO:Channel::remove()
    m_eLoop->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    if (m_tied)
    {
        std::shared_ptr<void> guard = m_tie.lock(); // zzz
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
        // guard为空情况，说明Channel的TcpConnection对象已经不存在了
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    // 对端关闭事件
    if ((m_revents & EPOLLHUP) && !(m_revents & EPOLLIN))
    {
        if (m_closeCallback)
            m_closeCallback();
    }

    // 错误事件
    if (m_revents & EPOLLERR)
    {
        LOG_ERROR << "channel.handle Error event, the fd = " << this->getfd();
        if (m_errorCallback)
            m_errorCallback();
    }

    // 读事件
    if (m_revents & (EPOLLIN | EPOLLPRI))
    {
        LOG_DEBUG << "channel have read events, thd fd = " << this->getfd();
        if (m_readCallback)
        {
            LOG_DEBUG << "channel call readCallback(), the fd = " << this->getfd();
            m_readCallback(receiveTime);
        }
    }

    // 写事件
    if (m_revents & EPOLLOUT)
    {
        if (m_writeCallback)
            m_writeCallback();
    }
}



