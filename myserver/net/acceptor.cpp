
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/logging.h"
#include "../include/acceptor.h"
#include "../include/inet_address.h"

static int createNonBlock()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOG_FATAL << "listen socket create failed: " << errno;
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort)
    : m_loop {loop}, m_acceptSocket {createNonBlock()}, m_acceptChannel {m_loop, m_acceptSocket.getFd()},
        m_listenning {false}
{
    LOG_DEBUG << "acceptor create nonblock socket, fd is " << m_acceptChannel.getfd();

    m_acceptSocket.setReuseAddr(reusePort);
    m_acceptSocket.setReusePort(true);
    m_acceptSocket.bindAddress(listenAddr);     // zzz

    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    m_acceptChannel.disableAll();
    m_acceptChannel.remove();
}

void Acceptor::listen()
{
    m_listenning = true;
    m_acceptSocket.listen();
    m_acceptChannel.enableRead();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = m_acceptSocket.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (m_newConnectionCallback)
        {
            m_newConnectionCallback(connfd, peerAddr);
        }
        else
        {
            LOG_DEBUG << "no newConnectionCallback function";
            ::close(connfd);
        }
    }
    else
    {
        LOG_ERROR << "accept() failed";

        if (errno == EMFILE)    // 当前进程的fd已经用完了
        {
            LOG_ERROR << "sockfd reached limit";
        }
    }
}


