#pragma once

#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <functional>

#include "timestamp.h"
#include "log_stream.h"


class SourceFile
{
public:
    const char* m_data;
    int m_size;

    explicit SourceFile(const char* filename) : m_data {filename}
    {
        // 查找一个字符c在另一个字符串str中末次出现的位置（也就是从str的右侧开始查找字符c首次出现的位置
        const char* slash = strrchr(filename, '/');
        if (slash)
        {
            m_data = slash + 1;
        }
        m_size = static_cast<int>(strlen(m_data));
    }

};

class Logger
{
public:
    enum LogLevel
    {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL, LEVEL_COUNT,
    };

    Logger(const char* file, int line);
    Logger(const char* file, int line, LogLevel level);
    Logger(const char* file, int line, LogLevel level, const char* func);
    ~Logger();

    LogStream& stream() { return m_impl.m_stream; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel);

    using OutputFunc = std::function<void(const char* msg, int len)>;
    using FlushFunc = std::function<void()>;
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);        // zzz where to use it ?

private:
    class Impl
    {
    public:
        using LogLevel = Logger::LogLevel;
        Impl(LogLevel, int savedErrno, const char* file, int line);
        void formatTime();
        void finish();

        Timestamp m_time;
        LogStream m_stream;
        LogLevel m_level;
        int m_line;
        SourceFile m_file;
    };
    Impl m_impl;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel logLevel()
{
    return g_logLevel;
}

const char* getErrnoMsg(int errno);


// zzz SourceLocation

#define LOG_DEBUG if (logLevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (logLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()











