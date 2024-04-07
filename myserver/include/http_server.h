#pragma once // 不是C++标准内容。

#include <string>       // 推荐 先导入官方的，为了避免可能的命名冲突和不稳定的编译依赖。 这里理由 感觉等于没有。。可能还没有到那个层次

#include "tcp_server.h"
#include "noncopyable.h" // 头文件大小写 是一般怎么规定的？zzz 看了下，推荐 全小写的。改了
#include "logging.h"


// zzz 缺少 命名空间


class HttpRequest;
class HttpResponse;

class HttpServer : noncopyable
{
public:
    using HttpCallback = std::function<void (const HttpRequest&, HttpResponse*)>;   // HttpResponse 估计是 output param

    // 我才发现，我指针是跟着 变量的， 引用是跟着 类型的。 int *a, b;  a是指针，b是int
    // 不确定了，之前 我是怎么写的。
    // ..方法返回值，我是 *跟着type的。。
    HttpServer(EventLoop *eLoop, const InetAddress& listenAddr, const std::string& name, TcpServer::Option option = TcpServer::kNoReusePort);    // kNoReusePort 名字好怪。 是个 enum值， 另一个是 kReusePort ， 是否重用端口。 什么意思？zzz

    EventLoop* getLoop() const { return this->m_server.getLoop(); }
    void setHttpCallback(const HttpCallback& cb)
    {
        this->m_httpCallback = cb;      // 这种 this-> 加好 不加好？ zzz
    }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer *buf, Timestamp receiveTime);   // C++不是 time_point/Date/DateTime 之类的吗 zzz
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);

    TcpServer m_server;
    HttpCallback m_httpCallback;
};
