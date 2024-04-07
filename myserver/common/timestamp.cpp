
#include "../include/timestamp.h"

Timestamp Timestamp::now()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);        // time.h

    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

std::string Timestamp::format(bool showMicroSec) const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(m_microSec / kMicroSecondsPerSecond);

    tm* tm_time = localtime(&seconds);
    if (showMicroSec)
    {
        int microSec = static_cast<int>(m_microSec % kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d.%06d",
            tm_time->tm_year + 1900,
            tm_time->tm_mon + 1,
            tm_time->tm_mday,
            tm_time->tm_hour,
            tm_time->tm_min,
            tm_time->tm_sec,
            microSec);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d",
            tm_time->tm_year + 1900,
            tm_time->tm_mon + 1,
            tm_time->tm_mday,
            tm_time->tm_hour,
            tm_time->tm_min,
            tm_time->tm_sec);
    }
    return buf;
}

