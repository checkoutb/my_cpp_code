
#include <memory>

#include "../include/event_loop_thread.h"
#include "../include/event_loop_threadpool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
    : m_loop {baseLoop}, m_name {name}, m_started {false}, m_numThreads {0}, m_next {0}
{}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    m_started = true;

    for (int i = 0; i < m_numThreads; ++i)
    {
        char buf[m_name.size() + 32];
        snprintf(buf, sizeof(buf), "%s_%d", m_name.c_str(), i);     // zzz sizeof后面是类型 必须加括号，变量，可以不加括号
        EventLoopThread *t = new EventLoopThread(cb, buf);
        m_v_threads.push_back(std::unique_ptr<EventLoopThread>(t));
        // 底层创建线程 绑定一个新的EventLoop 并返回该loop的地址
        // 此时已经开始执行新线程了
        m_v_loops.push_back(t->startLoop());
    }

    // 整个服务端只有一个线程运行baseLoop,使用caller线程
    if (m_numThreads == 0 && cb)
        cb(m_loop);
}


EventLoop* EventLoopThreadPool::getNextLoop()
{
    // 如果只设置一个线程 也就是只有一个mainReactor 无subReactor
    // 那么轮询只有一个线程 getNextLoop()每次都返回当前的baseLoop_ (。。m_loop)
    EventLoop* loop = m_loop;

    // 通过轮询获取下一个处理事件的loop
    // 如果没设置多线程数量，则不会进去，相当于直接返回baseLoop (。。m_loop)

    // zzz 所以这里可以算作是 调度 啊， 算不算 核心调度？
    if (!m_v_loops.empty())
    {
        loop = m_v_loops[m_next];
        ++m_next;
        if (m_next >= m_v_loops.size())  // ==
        {
            m_next = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if (m_v_loops.empty())
    {
        return std::vector<EventLoop*>(1, m_loop);
    }
    else
    {
        return m_v_loops;
    }
}









