
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>
// #include <sys/socket.h>

#include "../include/mysocket.h"
#include "../include/logging.h"
#include "../include/inet_address.h"

MySocket::~MySocket()
{
    ::close(m_sockfd);
}

void MySocket::bindAddress(const InetAddress& localAddr)
{
    if (::bind(m_sockfd, (sockaddr *)localAddr.getSockAddr(), sizeof(sockaddr_in)) != 0)
        LOG_FATAL << "bind sockfd " << m_sockfd << " failed.";
}

void MySocket::listen()
{
    if (0 != ::listen(m_sockfd, 1024))
        LOG_FATAL << "listen sockfd " << m_sockfd << " failed.";
}

int MySocket::accept(InetAddress *peerAddr)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    ::memset(&addr, 0, sizeof(addr));

    int connfd = ::accept4(m_sockfd, (sockaddr*) &addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (connfd >= 0)
        peerAddr->setSockAddr(addr);
    else
        LOG_ERROR << "accept4 failed";

    return connfd;
}


void MySocket::shutdownWrite()
{
    if (::shutdown(m_sockfd, SHUT_WR) < 0)
        LOG_ERROR << "shutdown error";
}


void MySocket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

// 设置地址复用，其实就是可以使用处于Time-wait的端口
void MySocket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

// 通过改变内核信息，==zzz, 多个进程可以绑定同一个地址==。通俗就是多个服务的ip+port是一样
void MySocket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void MySocket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}













