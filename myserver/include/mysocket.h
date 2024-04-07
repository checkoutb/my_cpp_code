#pragma once

#include "noncopyable.h"

class InetAddress;

class MySocket : noncopyable
{
public:
    explicit MySocket(int sockfd) : m_sockfd{sockfd}
    {}

    ~MySocket();

    int getFd() const { return m_sockfd; }
    void bindAddress(const InetAddress& localAddr); // 绑定sockfd
    void listen();  // 使sockfd为可接受连接状态
    int accept(InetAddress* peerAddr);

    void shutdownWrite(); // 设置半关闭

    void setTcpNoDelay(bool on);    // 设置Nagel算法
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int m_sockfd;     // ...也是，every is file。 没有专门的 Socket类。。
};
