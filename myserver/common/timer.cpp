#include "../include/timer.h"

void Timer::restart(Timestamp now)
{
    m_expiration = m_repeat ? addTime(now, m_interval) : Timestamp();
    // if (m_repeat)
    // {
    //     m_expiration = addTime(now, m_interval);
    // }
    // else
    // {
    //     m_expiration = Timestamp();
    // }
}
