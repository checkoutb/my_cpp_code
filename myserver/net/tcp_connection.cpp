
#include <functional>
#include <string>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/tcp.h>

#include "../include/tcp_connection.h"
#include "../include/logging.h"
#include "../include/mysocket.h"
#include "../include/channel.h"
#include "../include/event_loop.h"

static EventLoop* checkLoopNotNull(EventLoop* loop)
{
    if (loop == nullptr)
        LOG_FATAL << "mainLoop is null;";
    return loop;
}

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
    : m_loop {checkLoopNotNull(loop)}, m_name {name}, m_state {kConnecting}, m_reading {true},
        m_socket {new MySocket(sockfd)},        // zzz m_socket 是unique_ptr， new MySocket 不是
        m_channel {new Channel(loop, sockfd)}, m_localAddr {localAddr},
        m_peerAddr {peerAddr}, m_highWaterMark {64 * 1024 * 1024} // // 64M 避免发送太快对方接受太慢

{
    // zzz use lambda ?
    m_channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    m_channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    LOG_INFO << "TcpConnection::ctor[" << m_name.c_str() << "] at fd=" << sockfd;
    m_socket->setKeepAlive(true);
}


TcpConnection::~TcpConnection()
{
    LOG_INFO << "TcpConnection::dtor[" << m_name.c_str() << "] at fd=" << m_channel->getfd() << " state=" << static_cast<int>(m_state);
}


void TcpConnection::send(const std::string& buf)
{
    if (m_state == kConnected)
    {
        if (m_loop->isInLoopThread())
            sendInLoop(buf.c_str(), buf.size());
        else
        {
            void (TcpConnection::*fp)(const void* data, size_t len) = &TcpConnection::sendInLoop;
            m_loop->runInLoop(std::bind(fp, this, buf.c_str(), buf.size()));
        }
    }
}

void TcpConnection::send(Buffer *buf)
{
    if (m_state == kConnected)
    {
        if (m_loop->isInLoopThread())
        {
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        }
        else
        {
            void (TcpConnection::*fp)(const std::string& msg) = &TcpConnection::sendInLoop;
            m_loop->runInLoop(std::bind(fp, this, buf->retrieveAllAsString()));
        }
    }
}

void TcpConnection::sendInLoop(const std::string& msg)
{
    sendInLoop(msg.data(), msg.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if (m_state == kDisconnected)
    {
        LOG_ERROR << "disconnected, give up write";
        return;
    }

    if (!m_channel->isWrite() && m_outputBuffer.readableBytes() == 0)
    {
        nwrote = ::write(m_channel->getfd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && m_writeCompleteCallback)
            {
                m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
            }
        }
        else
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_ERROR << "TcpConnection::sendInLoop";
                if (errno == EPIPE || errno == ECONNRESET)
                    faultError = true;
            }
        }
    }

    // 说明一次性并没有发送完数据，剩余数据需要保存到缓冲区中，且需要改channel注册写事件
    if (!faultError && remaining > 0)
    {
        size_t oldLen = m_outputBuffer.readableBytes();
        if (oldLen + remaining >= m_highWaterMark && oldLen < m_highWaterMark && m_highWaterMarkCallback)
        {
            m_loop->queueInLoop(std::bind(m_highWaterMarkCallback, shared_from_this(), oldLen + remaining));
        }
        m_outputBuffer.append((char*) data + nwrote, remaining);
        if (!m_channel->isWrite())
        {
            m_channel->enableWrite(); // 这里一定要注册channel的写事件 否则poller不会给channel通知epollout
        }
    }
}

void TcpConnection::shutdown()
{
    if (m_state == kConnected)
    {
        setState(kDisconnecting);
        m_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{
    if (!m_channel->isWrite())
        m_socket->shutdownWrite();
}

void TcpConnection::connectEstablished()
{
    setState(kConnected);
    m_channel->tie(shared_from_this());
    m_channel->enableRead();

    m_connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if (m_state == kConnected)
    {
        setState(kDisconnected);
        m_channel->disableAll();    // 把channel的所有感兴趣的事件从poller中删除掉
        m_connectionCallback(shared_from_this());
    }
    m_channel->remove();
}


void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = m_inputBuffer.readFd(m_channel->getfd(), &savedErrno);

    if (n > 0)
    {
        m_msgCallback(shared_from_this(), &m_inputBuffer, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_ERROR << "TcpConnection::handleRead failed";
        handleError();
    }
}


void TcpConnection::handleWrite()
{
    if (m_channel->isWrite())
    {
        int saveErrno = 0;
        ssize_t n = m_outputBuffer.writeFd(m_channel->getfd(), &saveErrno);

        if (n > 0)
        {
            m_outputBuffer.retrieve(n);
            if (m_outputBuffer.readableBytes() == 0)
            {
                m_channel->disableWrite();
                if (m_writeCompleteCallback)
                {
                    m_loop->queueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
                }
                if (m_state == kDisconnecting)
                    shutdownInLoop();
            }
        }
        else
        {
            LOG_ERROR << "TcpConnection:;handleWrite fail";
        }
    }
    else
    {
        LOG_ERROR<<"TcpConnection fd="<<m_channel->getfd()<<" is down, no more write";
    }
}

void TcpConnection::handleClose()
{
    setState(kDisconnected);
    m_channel->disableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    m_connectionCallback(connPtr);
    m_closeCallback(connPtr);
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t optlen = sizeof(optval);
    int err = 0;

    if (::getsockopt(m_channel->getfd(), SOL_SOCKET, SO_ERROR, &optval, &optlen))
        err = errno;
    else
        err = optval;

    LOG_ERROR << "Tcpconnection::handleError name: " << m_name.c_str() << " - SO_ERROR: " << err;
}



