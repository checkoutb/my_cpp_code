
#include <unistd.h>
#include <sys/eventfd.h>
#include <fcntl.h>

#include "../include/event_loop.h"
#include "../include/logging.h"
#include "../include/poller.h"


// __thread EventLoop *t_loop = nullptr;
thread_local EventLoop *t_loop = nullptr;

const int kPollTimsMS = 10000;  // Poller IO复用接口的超时时间


// zzz eventfd 是从内核2.6.22开始支持的一种新的事件等待/通知机制。用来通知事件的文件描述符，它不仅可以用于进程间的通信，还可以用户内核发信号给用户层的进程。简而言之：eventfd 就是用来触发事件通知

int createEventFd()
{
    int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evfd < 0)
    {
        LOG_FATAL << "eventfd error: "<<errno;
    }
    return evfd;
}

EventLoop::EventLoop() :
    m_loop(false),
    m_quit(false),      // 可以直接放 class声明中
    m_callPendingFunctors(false),
    m_tid(CurrentThread::tid()),
    m_poller(Poller::newDefaultPoller(this)),
    m_timerQueue { new TimerQueue(this) },
    m_wakeupFd { createEventFd() },
    m_wakeupChannel { new Channel(this, m_wakeupFd) },
    m_currentActiveChannel { nullptr }
{
    LOG_DEBUG << "eventloop created " << this << " the index is " << m_tid;
    LOG_DEBUG << "eventloop created wakteup fd " << m_wakeupChannel->getfd();
    if (t_loop)
    {
        LOG_FATAL << "another eventloop " << t_loop << " exists in this thread " << m_tid;
    }
    else
    {
        t_loop = this;
    }
    m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannel->enableRead();
}

EventLoop::~EventLoop()
{
    m_wakeupChannel->disableAll();
    m_wakeupChannel->remove();
    ::close(m_wakeupFd);
    t_loop = nullptr;
}

void EventLoop::loop()
{
    m_loop = true;
    m_quit = false;
    LOG_INFO << "Eventloop " << this << " start looping";

    while (!m_quit)
    {
        m_activeChannels.clear();
        m_pollReturnTime = m_poller->poll(kPollTimsMS, &m_activeChannels);
        for (Channel* channel : m_activeChannels)
        {
            channel->handleEvent(m_pollReturnTime);
        }
         // * IO thread：mainLoop accept fd 打包成 chennel 分发给 subLoop
         // * mainLoop实现注册一个回调，交给subLoop来执行，wakeup subLoop 之后，让其执行注册的回调操作
         // * 这些回调函数在 std::vector<Functor> pendingFunctors_; 之中
        doPendingFunctors();
    }
    m_loop = false;
}

void EventLoop::quit()
{
    m_quit = true;

     // * 生产者消费者队列派发方式和muduo的派发方式
     // * 有可能是别的线程调用quit(调用线程不是生成EventLoop对象的那个线程)
     // * 比如在工作线程(subLoop)中调用了IO线程(mainLoop)
     // * 这种情况会唤醒主线程
    if (isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
        cb();
    else
        queueInLoop(cb);
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_pendingFunctors.emplace_back(cb);
    }

     // * std::atomic_bool callingPendingFunctors_; 标志当前loop是否有需要执行的回调操作
     // * 这个 || callingPendingFunctors_ 比较有必要，因为在执行回调的过程可能会加入新的回调
     // * 则这个时候也需要唤醒，否则就会发生有事件到来但是仍被阻塞住的情况
    if (!isInLoopThread() || m_callPendingFunctors)
    {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR << "eventloop::wakeup write " << n << " bytes instead of 8.";
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(m_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_ERROR << "eventloop::handleRead() read " << n << " bytes instead of 8.";
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    m_poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return m_poller->hasChannel(channel);
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    m_callPendingFunctors = true;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }
    for (const Functor &functor : functors)
    {
        functor();
    }
    m_callPendingFunctors = false;
}





































