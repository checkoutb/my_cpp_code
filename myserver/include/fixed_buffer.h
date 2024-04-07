#pragma once

#include <assert.h>
#include <string.h>
#include <strings.h>
#include <string>

#include "noncopyable.h"

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer : noncopyable
{
    const char* end() const { return m_data + sizeof(m_data); }
    char m_data[SIZE];
    char* m_cur;

public:
    FixedBuffer() : m_cur {m_data}
    {}

    void append(const char* buf, size_t len)
    {
        if (static_cast<size_t>(avail()) > len)
        {
            memcpy(m_cur, buf, len);
            m_cur += len;
        }
        // zzz avail 不够怎么处理？ 而且哪里fsync 到磁盘？
    }

    const char* data() const { return m_data; }
    int length() const { return static_cast<int>(end() - m_data); }

    char* current() { return m_cur; }
    int avail() const { return static_cast<int>(end() - m_cur); }
    void add(size_t len) { m_cur += len; }

    void reset() { m_cur = m_data; }
    void bzero() { ::bzero(m_data, sizeof(m_data)); }

    std::string toString() const { return std::string(m_data, length()); }
};

