#pragma once

#include <functional>
#include <string>
#include <memory>
#include <unordered_map>
#include <atomic>

#include "event_loop.h"
#include "event_loop_threadpool.h"
#include "acceptor.h"
#include "inet_address.h"       // 我记得 linux 有官方的
#include "noncopyable.h"
#include "callback.h"
#include "tcp_connection.h"


class TcpServer : noncopyable
{
public:
    using ThreadInitCallback = std::function<void (EventLoop*)>;

    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    // const 是看 声明 还是 定义？ zzz 形参必须都放，不然不是同一个方法。
    TcpServer(EventLoop* , const InetAddress&, const std::string&, Option option = kNoReusePort);

    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback& cb) { m_threadInitCallback = cb; }
    void setConnectionCallback(const ConnectionCallback& cb) { m_connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { m_msgCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { m_writeCompleteCallback = cb; }

    void setThreadNum(int);

    void start(); // 开启服务器监听

    EventLoop* getLoop() const { return m_loop; }
    const std::string& getName() { return m_name; }      // 直接这里直接 name 方法，改成 getName, 而且 直接返回 string， 现在返回 string&
    const std::string& getIpPort() { return m_ipPort; } // ipPort 不太理解，2个放一起了？

private:

    void newConnection(int, const InetAddress&);
    void removeConnection(const TcpConnectionPtr&);
    void removeConnectionInLoop(const TcpConnectionPtr&);

    // using TcpConnectionPtr = std::shared_ptr<TcpConnection>
    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop* m_loop;
    const std::string m_ipPort;
    const std::string m_name;   // TcpServer名字
    std::unique_ptr<Acceptor> m_acceptor;   // Acceptor对象负责监视

    std::shared_ptr<EventLoopThreadPool> m_threadPool;

    ConnectionCallback m_connectionCallback;    // 有新连接时的回调函数
    MessageCallback m_msgCallback;
    WriteCompleteCallback m_writeCompleteCallback;

    ThreadInitCallback m_threadInitCallback;

    std::atomic_int m_started;

    int m_nextConnId;   // 连接索引
    ConnectionMap m_connections;// 保存所有的连接
};










