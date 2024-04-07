#pragma once

#include <string>

#include "fixed_buffer.h"
#include "noncopyable.h"

class GeneralTemplate : noncopyable
{
public:
    const char* m_data;
    int m_len;

    GeneralTemplate() : m_data {nullptr}, m_len {0}
    {}

    explicit GeneralTemplate(const char* data, int len) : m_data {data}, m_len {len}
    {}

};

class LogStream : noncopyable
{
public:
    using Buffer = FixedBuffer<kSmallBuffer>;
    void append(const char* data, int len) { m_buffer.append(data, len); }
    const Buffer& buffer() const { return m_buffer; }
    void resetBuffer() { m_buffer.reset(); }

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(float);
    LogStream& operator<<(double);

    LogStream& operator<<(char);
    LogStream& operator<<(const void* data);
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const unsigned char* str);
    LogStream& operator<<(const std::string& str);
    LogStream& operator<<(const Buffer&);

    LogStream& operator<<(const GeneralTemplate&);

private:
    static const int kMaxNumberSize = 48;

    template <typename T>
    void formatInteger(T);

    Buffer m_buffer;
};
