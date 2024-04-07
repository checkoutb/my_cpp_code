#pragma once

#include <vector>
#include <string>
#include <algorithm>



// zzzzzz ，这个设计的原因 是什么
// ring buffer 会不会更好?


/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
class Buffer
{
public:

    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    // zzz m_buffer 不能用 {} ，不然初始化为 一个 char...
    explicit Buffer(size_t initSize = kInitialSize) : m_buffer(kCheapPrepend + initSize), m_readerIndex  {kCheapPrepend}, m_writerIndex{kCheapPrepend}
    {}

    size_t readableBytes() const { return m_writerIndex - m_readerIndex; }

    size_t writableBytes() const { return m_buffer.size() - m_writerIndex; }

    size_t prependableBytes() const { return m_readerIndex; }

    const char* peek() const { return begin() + m_readerIndex; }

    void retrieveUntil(const char* end)
    {
        retrieve(end - peek());
    }

    void retrieve(size_t len)
    {
        if (len < readableBytes())
            m_readerIndex += len;
        else
            retrieveAll();
    }

    void retrieveAll()
    {
        m_readerIndex = kCheapPrepend;
        m_writerIndex = kCheapPrepend;
    }

    // for debug
    std::string getBufferAsString()
    {
        size_t len = readableBytes();
        std::string result(peek(), len);
        return result;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
            makeSpace(len);
    }

    void append(const std::string& str)
    {
        append(str.data(), str.size());
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        m_writerIndex += len;
    }

    const char* findCRLF() const
    {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    char *beginWrite()
    {
        return begin() + m_writerIndex;
    }

    const char* beginWrite() const
    {
        return begin() + m_writerIndex;
    }

    ssize_t readFd(int fd, int* saveErrno);
    ssize_t writeFd(int fd, int* saveErrno);

private:
    char* begin() { return &(*m_buffer.begin()); }
    const char* begin() const { return &(*m_buffer.begin()); }

    void makeSpace(int len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
            m_buffer.resize(m_writerIndex + len);
        else
        {
            size_t readable = readableBytes();
            std::copy(begin() + m_readerIndex, begin() + m_writerIndex, begin() + kCheapPrepend);
            m_readerIndex = kCheapPrepend;
            m_writerIndex = m_readerIndex + readable;
        }
    }

    std::vector<char> m_buffer;
    size_t m_readerIndex;
    size_t m_writerIndex;
    static const char kCRLF[];
};
