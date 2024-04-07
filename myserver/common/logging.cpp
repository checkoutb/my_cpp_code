
#include "../include/logging.h"
#include "../include/current_thread.h"

namespace ThreadInfo
{
    thread_local char t_errnobuf[512];
    thread_local char t_time[64];
    thread_local time_t t_lastSecond;
};

const char* getErrnoMsg(int savedErrno)
{
    return strerror_r(savedErrno, ThreadInfo::t_errnobuf, sizeof(ThreadInfo::t_errnobuf));
}

    // zzz LEVEL_COUNT 666
const char* getLevelName[Logger::LogLevel::LEVEL_COUNT]
{
    "TRACE ", "DEBUG", "INFO ", "WARN ", "ERROR ", "FATAL ",
};

Logger::LogLevel initLogLevel()
{
    return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

static void defaultOutput(const char* data, int len)
{
    fwrite(data, len, sizeof(char), stdout);
}

static void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;


Logger::Impl::Impl(Logger::LogLevel level, int savedErrno, const char* file, int line)
    : m_time(Timestamp::now()), m_stream {}, m_level {level}, m_line (line), m_file {file}
{
    formatTime();
    m_stream << GeneralTemplate(getLevelName[level], 6);

    if (savedErrno != 0)
    {
        m_stream << getErrnoMsg(savedErrno) << " errno is " << savedErrno;
    }
}

void Logger::Impl::formatTime()
{
    Timestamp now = Timestamp::now();
    time_t seconds = static_cast<time_t>(now.getMicroSec() / Timestamp::kMicroSecondsPerSecond);
    int microSec = static_cast<int>(now.getMicroSec() % Timestamp::kMicroSecondsPerSecond);

    struct tm* tm_time = localtime(&seconds);

    snprintf(ThreadInfo::t_time, sizeof(ThreadInfo::t_time), "%4d/%02d/%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour,
             tm_time->tm_min, tm_time->tm_sec);

    ThreadInfo::t_lastSecond = seconds;

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%06d", microSec);

    m_stream << GeneralTemplate(ThreadInfo::t_time, 17) << GeneralTemplate(buf, 7);
}

void Logger::Impl::finish()
{
    m_stream << " - " << GeneralTemplate(m_file.m_data, m_file.m_size) << " : " << m_line << "\n";
}

Logger::Logger(const char* file, int line) : m_impl {INFO, 0, file, line}
{}

Logger::Logger(const char* file, int line, Logger::LogLevel level) : m_impl {level, 0, file, line}
{}

Logger::Logger(const char* file, int line, Logger::LogLevel level, const char* func)
    : m_impl {level, 0, file, line}
{
    m_impl.m_stream << func << ' ';
}

Logger::~Logger()
{
    m_impl.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (m_impl.m_level == FATAL)
    {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc f)
{
    g_output = f;
}

void Logger::setFlush(FlushFunc f)
{
    g_flush = f;
}






