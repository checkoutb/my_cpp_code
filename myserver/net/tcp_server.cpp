
#include <functional>
#include <string.h>

#include "../include/tcp_server.h"
#include "../include/tcp_connection.h"
#include "../include/logging.h"

static EventLoop* checkLoopNotNull(EventLoop* loop)
{
    if (loop == nullptr)
        LOG_FATAL << "mainloop is null";
    return loop;
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name, Option option)
    : m_loop {checkLoopNotNull(loop)}, m_ipPort {listenAddr.toIpPort()}, m_name {name},
        m_acceptor {new Acceptor(loop, listenAddr, option == kReusePort)},
        m_threadPool {new EventLoopThreadPool(loop, m_name)},
        m_connectionCallback {}, m_msgCallback {}, m_writeCompleteCallback {},
        m_threadInitCallback {}, m_started {0}, m_nextConnId {1}
{
    m_acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (auto& item : m_connections)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();   // 把原始的智能指针复位 让栈空间的TcpConnectionPtr conn指向该对象 当conn出了其作用域 即可释放智能指针指向的对象
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}


void TcpServer::setThreadNum(int num)
{
    m_threadPool->setThreadNum(num);
}

void TcpServer::start()
{
    if (m_started++ == 0)
    {
        m_threadPool->start(m_threadInitCallback);
        m_loop->runInLoop(std::bind(&Acceptor::listen, m_acceptor.get()));
    }
}


void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    EventLoop* ioLoop = m_threadPool->getNextLoop();

    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "-%s#%d", m_ipPort.c_str(), m_nextConnId);

    ++m_nextConnId;

    std::string connName = m_name + buf;

    LOG_INFO << "TcpServer::newConnection [" << m_name.c_str() << "] - new Connection [" << connName.c_str() << "] from " << peerAddr.toIpPort().c_str();

    sockaddr_in local;
    ::memset(&local, 0, sizeof(local));
    socklen_t addrlen = sizeof(local);
    if (::getsockname(sockfd, (sockaddr*) &local, &addrlen) < 0)
    {
        LOG_ERROR << "Sockets::getLocalAddr() failed";      // zzz ? 信息不是这里的啊
    }

    InetAddress localAddr(local);
    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));

    m_connections[connName] = conn;
    conn->setConnectionCallback(m_connectionCallback);
    conn->setMsgCallback(m_msgCallback);
    conn->setWriteCompleteCallback(m_writeCompleteCallback);

    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    m_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << m_name.c_str() << "] - Connection is " << conn->getName().c_str();

    m_connections.erase(conn->getName());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

















