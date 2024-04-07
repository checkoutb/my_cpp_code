#pragma once

#include <thread>
#include <functional>
#include <memory>
#include <string>
#include <atomic>
#include "noncopyable.h"




class MyThread : noncopyable
{
public:

    using ThreadFunc = std::function<void()>;
    explicit MyThread(ThreadFunc, const std::string& name = std::string());
    ~MyThread();

    void start();
    void join();

    bool isStarted() const { return m_startd; }
    pid_t getTid() const { return m_tid; }
    const std::string& getName() const { return m_name; }

    static int getNumCreated() { return s_numCreated; }

private:
    void setDefaultName();  // set thread's name
    bool m_startd;
    bool m_joined;
    std::shared_ptr<std::thread> m_thread;
    pid_t m_tid;
    ThreadFunc m_func;
    std::string m_name;
    static std::atomic_int32_t s_numCreated;
};
