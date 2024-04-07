#pragma once

#include <vector>
#include <set>

#include "timestamp.h"
#include "channel.h"

class EventLoop;
class Timer;

class TimerQueue
{
public:
    using TimerCallback = std::function<void()>;

    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    // 回调函数，到期时间，是否重复
    void addTimer(TimerCallback cb, Timestamp when, double interval);

private:
    using Entry = std::pair<Timestamp, Timer*>;
    using TimerList = std::set<Entry>;

    void addTimerInLoop(Timer* timer);
    void handleRead();
    void resetTimerfd(int timerFd, Timestamp expiration);

    // 移除所有已到期的定时器
    // 1.获取到期的定时器
    // 2.重置这些定时器（销毁或者重复定时任务）
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    EventLoop* m_loop;
    const int m_timerFd;
    Channel m_timerFdChannel;
    TimerList m_timers;  // 定时器队列（内部实现是红黑树） 按过期时间排序
    bool m_callingExpiredTimers;
};
