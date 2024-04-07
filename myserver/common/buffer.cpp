
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

#include "../include/buffer.h"
#include "../include/logging.h"

const char Buffer::kCRLF[] = "\r\n";



 // * 从fd上读取数据 Poller工作在LT模式
 // * Buffer缓冲区是有大小的！ 但是从fd上读取数据的时候 却不知道tcp数据的最终大小
 // *
 // * 从socket读到缓冲区的方法是使用readv先读至buffer_，
 // * Buffer_空间如果不够会读入到栈上65536个字节大小的空间，然后以append的
 // * 方式追加入buffer_。既考虑了避免系统调用带来开销，又不影响数据的接收。
ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    char extrabuf[65536] = {0};         // zzz 每次都要申请 6万5 字节，代价应该挺大的吧。
                                // zzz 有点疑问了。。。 我知道这里 肯定没有办法 知道 http 头中的 长度 属性， 毕竟 读都没有读。
                                    // 内核 知道 fd中数据的 长度吗？估计也不知道，而且可以0拷贝，不走内核。
                                    // 还有就是。。 TCP是流， 那么 一个 很长的流， 我把头+一部分body读取以后， 后续的body 来了以后， 我怎么 和 之前的 合并？  难道之前的 我还没有处理掉？ 还是说，一次肯定会读完 头+body？  那 缓冲区不够呢？

    struct iovec vec[2];
    const size_t writable = writableBytes();

    vec[0].iov_base = begin() + m_writerIndex;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;

    // 从文件描述符中读取数据到多个缓冲区中
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (n <= writable)
    {
        m_writerIndex += n;
    }
    else
    {
        m_writerIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}


ssize_t Buffer::writeFd(int fd, int* saveErrno)
{
    ssize_t n = ::write(fd, peek(), readableBytes());
    if (n < 0)
        *saveErrno = errno;
    return n;
}


