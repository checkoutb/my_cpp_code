
#include "../include/http_server.h"
#include "../include/http_request.h"
#include "../include/http_response.h"
#include "../include/http_context.h"

#include <memory>


// 我知道 声明的时候 可以不写 形参名， 定义的时候，没用到，也可以不写， 应该第一次知道
void defaultHttpCallback(const HttpRequest&, HttpResponse *resp)
{
    resp->setStatusCode(HttpResponse::k404_notFound);        // k代表什么意思？ zzz 第二次遇到了，第一次是 .h 的 TcpServer::kNoReusePort   ..总不能是 kernel啊
    resp->setStatusMsg("Not Found");    // statusCode, statusMessage 应该是 配对的，应该可以 放一起，zzz
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* eLoop, const InetAddress& listenAddr, const std::string& name, TcpServer::Option option) : m_server {eLoop, listenAddr, name, option}, m_httpCallback {defaultHttpCallback}
{
    m_server.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    m_server.setThreadNum(4);
}

void HttpServer::start()
{
    LOG_INFO << "HttpServer [" << m_server.getName().c_str() << "] starts listening on " << m_server.getIpPort().c_str();
    m_server.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        LOG_INFO << "new Connection arrived";
    }
    else
    {
        LOG_INFO << "Connection closed";
    }
}


void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    std::unique_ptr<HttpContext> context(new HttpContext);  // zzz new 不加 ()，不会初始化，即使是 基本类型， 也是随即值。。
                // 不，这个是 默认初始化，对于 类/struct/union 是调用 默认构造器， 对于 数组，标量， 就是 不确定的值。

#if 0   // false
    std::string request = buf->getBufferAsString();
    std::cout<<request<<std::endl;
#endif

    if (!context->parseRequest(buf, receiveTime))
    {
        LOG_INFO << "parse request failed";
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->isGotAll())
    {
        LOG_INFO << "parse request success";
        onRequest(conn, context->getRequest());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    const std::string& connection = req.getHeader("Connection");

    bool close = connection == "close" || (req.getVersion() == HttpRequest::Http10 && connection != "Keep-Alive");

    close = true;       // zzz  没有用到上面的 判断。。

    HttpResponse response(close);

    // httpCallback_ 由用户传入，怎么写响应体由用户决定
    // 此处初始化了一些response的信息，比如响应码，回复OK
    m_httpCallback(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);

#if 0
    LOG_INFO << "\n\n=========\n\n";
    LOG_INFO << buf.peek();
    LOG_INFO << "\n\n=========\n\n";
#endif

    // 需要重载 TcpConnection::send 使其可以接收一个缓冲区
    conn->send(&buf);
    if (response.getCloseConnection())
        conn->shutdown();
}

