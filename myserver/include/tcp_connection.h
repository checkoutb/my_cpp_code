#pragma once

#include <memory>
#include <string>
#include <atomic>
#include <string>

#include "noncopyable.h"
#include "callback.h"
#include "buffer.h"
#include "timestamp.h"
#include "inet_address.h"


class Channel;
class EventLoop;
class MySocket;

        // zzz 将enable_shared_from_this<T>作为基类来继承（其中T为智能指针持有的类型），使得派生类拥有可以从this指针安全的构造shared_ptr的能力
        // 反例： 对一个对象 调用2次 shared_ptr， 生成2个 shared_ptr, 出范围的时候， 2个shared_ptr 都归零， 会 2次 析构 一个对象。
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:

    TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return m_loop; }
    const std::string& getName() const { return m_name; }
    const InetAddress& getLocalAddr() const { return m_localAddr; }
    const InetAddress& getPeerAddr() const { return m_peerAddr; }

    bool connected() const { return m_state == kConnected; }

    // 发送数据
    void send(const std::string& buf);
    void send(Buffer* buf);

    void shutdown();

    void setConnectionCallback(const ConnectionCallback& cb) { m_connectionCallback = cb;}
    void setMsgCallback(const MessageCallback& cb) { m_msgCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { m_writeCompleteCallback = cb; }
    void setCloseCallback(const CloseCallback& cb) { m_closeCallback = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
    {
        m_highWaterMarkCallback = cb;
        m_highWaterMark = highWaterMark;
    }

    void connectEstablished(); // TcpServer调用，连接建立
    void connectDestroyed();

private:
    enum StateE
    {
        kDisconnected, kConnecting, kConnected, kDisconnecting,
    };

    void setState(StateE state) { m_state = state; }

    void handleRead(Timestamp);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const void* msg, size_t len);
    void sendInLoop(const std::string& msg);
    void shutdownInLoop();

    EventLoop *m_loop;  // 属于哪个subLoop（如果是单线程则为mainLoop）
    const std::string m_name;
    std::atomic_int m_state;  // 连接状态
    bool m_reading;

    std::unique_ptr<MySocket> m_socket;
    std::unique_ptr<Channel> m_channel;

    const InetAddress m_localAddr;
    const InetAddress m_peerAddr;

    ConnectionCallback m_connectionCallback;
    MessageCallback m_msgCallback;
    WriteCompleteCallback m_writeCompleteCallback;
    CloseCallback m_closeCallback;
    HighWaterMarkCallback m_highWaterMarkCallback;
    size_t m_highWaterMark;

    Buffer m_inputBuffer;
    Buffer m_outputBuffer;
};

