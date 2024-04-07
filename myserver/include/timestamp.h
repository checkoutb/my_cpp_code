#pragma once

#include <iostream>
#include <string>
#include <sys/time.h>

class Timestamp
{
public:
    Timestamp() : m_microSec { 0 } {}
    explicit Timestamp(int64_t microSec) : m_microSec { microSec } {}

    static Timestamp now();

    std::string toString() const;
    std::string format(bool showMicroSec = false) const;

    // microSecondsSinceEpoch
    int64_t getMicroSec() const { return m_microSec; }

    time_t getSecond() const
    {
        return static_cast<time_t>(m_microSec / kMicroSecondsPerSecond);
    }

    static Timestamp invalid()      // 这有什么作用。  可以单例吗？ zzz
    {
        return Timestamp();
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    // not milli second
    int64_t m_microSec;   // micro second from epoch
};

// 定时器需要比较时间戳
inline bool operator<(Timestamp lhs, Timestamp rhs)       // & 无法作用于临时对象。
{
    return lhs.getMicroSec() < rhs.getMicroSec();
}
inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.getMicroSec() == rhs.getMicroSec();
}

//
inline Timestamp addTime(Timestamp ts, double seconds)
{
    int64_t mcs = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);        // k means const ? zzz
    return Timestamp(ts.getMicroSec() + mcs);
}

