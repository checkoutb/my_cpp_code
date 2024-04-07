#pragma once

#include <functional>
#include <memory>
#include <sys/epoll.h>

#include "noncopyable.h"
#include "timestamp.h"
#include "logging.h"

class EventLoop;
class Timestamp;

class Channel : noncopyable     // class name should be camelcase
{
public:
    using EventCallback = std::function<void ()>;
    using ReadEventCallback = std::function<void (Timestamp&)>;     // Timestamp + &

    Channel(EventLoop* eLoop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);   // + &

    void setReadCallback(ReadEventCallback cb) { this->m_readCallback = cb; }
    void setWriteCallback(EventCallback cb) { this->m_writeCallback = cb; }
    void setCloseCallback(EventCallback cb) { this->m_closeCallback = cb; }
    void setErrorCallback(EventCallback cb) { this->m_errorCallback = cb; }

    void tie(const std::shared_ptr<void>&);

    int getfd() const { return m_fd; }
    int getEvents() const { return m_events; }
    void setRevents(int revt) { m_revents = revt; }

    // 本质是调用 epoll_ctl
    void enableRead() { m_events |= kReadEvent; update(); }
    void disableRead() { m_events &= ~kReadEvent; update(); }        // &= ~ 很少写。
    void enableWrite() { m_events |= kWriteEvent; update(); }
    void disableWrite() { m_events &= ~kWriteEvent; update(); }
    void disableAll() { m_events &= kNoneEvent; update(); }

    // 返回fd当前事件状态
    bool isNoneEvent() const { return m_events == kNoneEvent; }
    bool isWrite() const { return m_events & kWriteEvent; }
    bool isRead() const { return m_events & kReadEvent; }

    int getIndex() { return m_index; }
    void setIndex(int idx) { m_index = idx; }

    EventLoop* getOwnerLoop() { return m_eLoop; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;    // 0
    static const int kReadEvent;    // EPOLLIN | EPOLLPRI
    static const int kWriteEvent;   // EPOLLOUT

    EventLoop *m_eLoop; // 当前channel 所属的 EventLoop
    const int m_fd; // poller监听对象
    int m_events;   // fd感兴趣的事件
    int m_revents;  // poller返回的具体发生的事件
    int m_index;    // 在poller上注册的情况

    std::weak_ptr<void> m_tie;  // 指向 TcpConnection
    bool m_tied;    // 是否调用过 Channel::tie 方法

    ReadEventCallback m_readCallback;
    EventCallback m_writeCallback;
    EventCallback m_closeCallback;
    EventCallback m_errorCallback;
};


