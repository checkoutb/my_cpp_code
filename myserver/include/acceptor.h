#pragma once

#include "noncopyable.h"
#include "mysocket.h"
#include "channel.h"

class EventLoop;
class InetAddress;


 // * Acceptor运行在mainLoop中
 // * TcpServer发现Acceptor有一个新连接，则将此channel分发给一个subLoop
class Acceptor
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { m_newConnectionCallback = cb; }

    bool getListenning() const { return m_listenning; }
    void listen();

private:
    void handleRead();

    EventLoop* m_loop;
    MySocket m_acceptSocket;
    Channel m_acceptChannel;
    NewConnectionCallback m_newConnectionCallback;
    bool m_listenning;
};
