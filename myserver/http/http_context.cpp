
#include "../include/http_context.h"
#include "../include/buffer.h"

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool success = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');

    // 不是最后一个空格，并且成功获取了method并设置到request_
    if (space != end && m_request.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                m_request.setPath(start, question);
                m_request.setQuery(question, space);
            }
            else
            {
                m_request.setPath(start, space);
            }
            start = space + 1;
            success = (end - start == 8 && std::equal(start, end - 1, "HTTP/1."));
            if (success)
            {
                if (*(end - 1) == '1')
                {
                    m_request.setVersion(HttpRequest::Http11);
                }
                else if (*(end - 1) == '0')
                {
                    m_request.setVersion(HttpRequest::Http10);
                }
                else
                {
                    success = false;
                }
            }
        }
    }
    return success;
}


bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    bool ok = false;
    bool hasMore = true;
    while (hasMore)
    {
        if (m_state == kExpectRequestLine)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    m_request.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    m_state = kExpectHeaders;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (m_state == kExpectHeaders)     // zzz 可以直接下来吧。 应该都是 头，body的顺序  不需要外面的 while
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':');      // ..zzz 这里需要 while
                if (colon != crlf)
                {
                    m_request.addHeader(buf->peek(), colon, crlf);
                }
                else
                {
                    m_state = kGotAll;
                    hasMore = false;

                    // zzz 这里没有修改状态为 kExpectBody, 而是直接 退出了 while。
                    // 如果 post的body中有 : ，会被识别为 header 吧？

                }
                buf->retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if (m_state == kExpectBody)
        {
            // 解析请求体，可以看到这里没有做出处理，只支持GET请求
            // ....zzz..............................这个坑有点大。
        }
    }
    return ok;
}


