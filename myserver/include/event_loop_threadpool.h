#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* loop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { m_numThreads = numThreads; }

    // 启动线程池
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();
    std::vector<EventLoop*> getAllLoops();

    bool started() const { return m_started; }
    const std::string getName() const { return m_name; }


private:
    EventLoop* m_loop;
    std::string m_name;
    bool m_started;
    int m_numThreads;
    size_t m_next;
    std::vector<std::unique_ptr<EventLoopThread>> m_v_threads;
    std::vector<EventLoop*> m_v_loops;
};


