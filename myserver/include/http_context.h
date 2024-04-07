#pragma once

#include "http_request.h"

class Buffer;

class HttpContext
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine, // 解析请求行状态
        kExpectHeaders, // 解析请求头部状态
        kExpectBody, // 解析请求体状态
        kGotAll, // 解析完毕状态
    };

    HttpContext() : m_state {kExpectRequestLine}
    {}

    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    // gotAll()
    bool isGotAll() const { return m_state == kGotAll; }

    void reset()
    {
        m_state = kExpectRequestLine;
        HttpRequest dummy;
        m_request.swap(dummy);
    }

    const HttpRequest& getRequest() const { return m_request; }

    HttpRequest& getRequest() { return m_request; }

private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState m_state;
    HttpRequest m_request;
};
