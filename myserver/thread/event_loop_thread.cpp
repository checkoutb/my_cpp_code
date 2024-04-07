
#include "../include/event_loop_thread.h"
#include "../include/event_loop.h"


// zzz m_thread {xx, yy} keyi?
EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
    : m_loop {nullptr}, m_exiting {false}, m_thread {std::bind(&EventLoopThread::threadFunc, this), name},
        m_mutex {}, m_cond {}, m_initCallback {cb}
{
}

EventLoopThread::~EventLoopThread()
{
    m_exiting = true;
    if (m_loop != nullptr)
    {
        m_loop->quit();
        m_thread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    m_thread.start();

    EventLoop* loop = nullptr;
    {
        // 等待新线程执行threadFunc完毕，所以使用cond_.wait
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_loop == nullptr)
        {
            m_cond.wait(lock);
        }
        loop = m_loop;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    if (m_initCallback)
        m_initCallback(&loop);

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_loop = &loop;
        m_cond.notify_one();
    }

    // 执行EventLoop的loop() 开启了底层的Poller的poll()
    loop.loop();

    // loop.loop 是循环， 跳出，说明 不再处理事件了， 需要关闭
    std::unique_lock<std::mutex> lock(m_mutex);
    m_loop = nullptr;
}
