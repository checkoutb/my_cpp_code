#pragma once

# include <functional>
# include <vector>
# include <memory>
# include <atomic>
# include <mutex>

# include "noncopyable.h"
# include "timestamp.h"
# include "current_thread.h"
# include "timer_queue.h"


class Channel;
class Poller;

class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    Timestamp pollReturnTime() const { return m_pollReturnTime; }

    // 在当前线程同步调用函数
    void runInLoop(Functor cb);

    void queueInLoop(Functor cb); // 入队，唤醒loop所在线程执行cb
    void wakeup();  // 唤醒loop所在线程

    void updateChannel(Channel*);
    void removeChannel(Channel*);
    bool hasChannel(Channel*);

    bool isInLoopThread() const { return m_tid == CurrentThread::tid(); }

    void runAt(Timestamp& ts, Functor&& cb)
    {
        m_timerQueue->addTimer(std::move(cb), ts, 0.0);
    }
    void runAfter(double waitTime, Functor&& cb)
    {
        Timestamp ts(addTime(Timestamp::now(), waitTime));
        runAt(ts, std::move(cb));
    }
    void runEvery(double interval, Functor&& cb)
    {
        Timestamp ts(addTime(Timestamp::now(), interval));
        m_timerQueue->addTimer(std::move(cb), ts, interval);
    }

private:

    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    std::atomic_bool m_loop;
    std::atomic_bool m_quit;    // 标志退出事件循环
    std::atomic_bool m_callPendingFunctors; // 当前loop是否有需要执行的回调操作

    const pid_t m_tid;  // thread id
    Timestamp m_pollReturnTime; // poller返回的事件的 channel的返回时间
    std::unique_ptr<Poller> m_poller;
    std::unique_ptr<TimerQueue> m_timerQueue;

    int m_wakeupFd;
    std::unique_ptr<Channel> m_wakeupChannel;

    ChannelList m_activeChannels;
    Channel* m_currentActiveChannel;

    std::mutex m_mutex; // 保护 m_pendingFunctors 线程安全
    std::vector<Functor> m_pendingFunctors; // 存储loop 跨线程需要执行的所有回调操作

};

