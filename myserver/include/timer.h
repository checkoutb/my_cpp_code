#pragma once

#include <functional>

#include "noncopyable.h"
#include "timestamp.h"


class Timer : noncopyable
{
public:
    using TimerCallback = std::function<void()>;

    Timer(TimerCallback cb, Timestamp when, double interval)
        : m_callback(std::move(cb)), m_expiration {when}, m_interval {interval}, m_repeat {interval > 0.0 }
    {}

    void run() const { m_callback(); }

    Timestamp expiration() const { return m_expiration; }
    bool repeat() const { return m_repeat; }
    void restart(Timestamp now);

private:
    const TimerCallback m_callback;
    Timestamp m_expiration;
    const double m_interval;
    const bool m_repeat;
};
