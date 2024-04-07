#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>

#include "noncopyable.h"
#include "mythread.h"

// one loop per thread
class EventLoop;

class EventLoopThread : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), const std::string& name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* m_loop;
    bool m_exiting;
    MyThread m_thread;
    std::mutex m_mutex;     // zzz 这个mutex 不和其他对象共享啊，锁不锁 都一样啊。
                        // 不，这个是为了 sync  listen/main-thread 和 process-thread
                        // startLoop 中 启动了 线程。 所以要同步。
    std::condition_variable m_cond;
    ThreadInitCallback m_initCallback;
};

