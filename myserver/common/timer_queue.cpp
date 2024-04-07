
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>

#include "../include/event_loop.h"
#include "../include/channel.h"
#include "../include/logging.h"
#include "../include/timer.h"
#include "../include/timer_queue.h"

int createTimerFd()
{

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_ERROR << "Failed to create timerfd";
    }
    return timerfd;
}

TimerQueue::TimerQueue(EventLoop* loop)
    : m_loop {loop}, m_timerFd(createTimerFd()), m_timerFdChannel {m_loop, m_timerFd}, m_timers()
{
    m_timerFdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_timerFdChannel.enableRead();
}

TimerQueue::~TimerQueue()
{
    m_timerFdChannel.disableAll();
    m_timerFdChannel.remove();
    ::close(m_timerFd);
    for (const Entry& timer : m_timers)
        delete timer.second;
}

void TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    bool earliestChanged = insert(timer);
    if (earliestChanged)
        resetTimerfd(m_timerFd, timer->expiration());
}

void TimerQueue::resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue; // zzz
    struct itimerspec oldValue;
    memset(&newValue, '\0', sizeof(newValue));
    memset(&oldValue, '\0', sizeof(oldValue));

    int64_t microSecDiff = expiration.getMicroSec() - Timestamp::now().getMicroSec();
    if (microSecDiff < 100)
        microSecDiff = 100;

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microSecDiff / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(microSecDiff % Timestamp::kMicroSecondsPerSecond * 1000);
    newValue.it_value = ts;
    if (::timerfd_settime(timerfd, 0, &newValue, &oldValue))        // m_timerfd
        LOG_ERROR << "timerfd_settime failed";
}

void readTimerFd(int timerfd)
{
    uint64_t read_byte;
    ssize_t readn = ::read(timerfd, &read_byte, sizeof(read_byte));
    if (readn != sizeof(read_byte))
        LOG_ERROR << "TimerQueue::readTimerFd read size < 0";
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = m_timers.lower_bound(sentry);
    std::copy(m_timers.begin(), end, back_inserter(expired));
    m_timers.erase(m_timers.begin(), end);
    return expired;
}

void TimerQueue::handleRead()
{
    Timestamp now = Timestamp::now();
    readTimerFd(m_timerFd);

    std::vector<Entry> expired = getExpired(now);

    m_callingExpiredTimers = true;
    for (const Entry& it : expired)
    {
        it.second->run();
    }
    m_callingExpiredTimers = false;

    reset(expired, now);
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;
    for (const Entry& it : expired)
    {
        if (it.second->repeat())
        {
            auto timer = it.second;
            timer->restart(Timestamp::now());
            insert(timer);
        }
        else
        {
            delete it.second;
        }

        if (!m_timers.empty())
        {
            resetTimerfd(m_timerFd, (m_timers.begin()->second)->expiration());
        }
    }
}

bool TimerQueue::insert(Timer* timer)
{
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = m_timers.begin();
    if (it == m_timers.end() || when < it->first)       // zzz
    {
        earliestChanged = true;
    }
    m_timers.insert(Entry(when, timer));
    return earliestChanged;
}
