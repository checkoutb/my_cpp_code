
#include <semaphore.h>

#include "../include/mythread.h"
#include "../include/current_thread.h"

std::atomic_int32_t MyThread::s_numCreated {0};


// 默认参数在定义和声明中只能出现一次，不能声明和定义都有默认参数
MyThread::MyThread(ThreadFunc func, const std::string& name)
    : m_startd {false}, m_joined {false}, m_tid {0}, m_func {std::move(func)}, m_name {name}
{
    setDefaultName();
}

MyThread::~MyThread()
{
    if (m_startd && !m_joined)
    {
        m_thread->detach();
    }
}

void MyThread::start()
{
    m_startd = true;
    sem_t sem;
    sem_init(&sem, false, 0);

    m_thread = std::shared_ptr<std::thread>(new std::thread([&](){
        m_tid = CurrentThread::tid();
        sem_post(&sem);
        m_func();
    }));

    // 如果不使用信号量操作，则别的线程访问tid时候，可能上面的线程还没有获取到tid
    // 所以必须等待
    sem_wait(&sem);
}

void MyThread::join()
{
    m_joined = true;
    m_thread->join();
}

void MyThread::setDefaultName()
{
    int num = ++s_numCreated;
    if (m_name.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread-%d", num);
        m_name = buf;
    }
}
