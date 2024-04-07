
#include <stdio.h>
#include <string.h>

#include "../include/http_response.h"
#include "../include/buffer.h"

void HttpResponse::appendToBuffer(Buffer* output) const
{
    char buf[32];
    memset(buf, '\0', sizeof(buf));
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", m_statusCode);
    output->append(buf);
    output->append(m_statusMsg);
    output->append("\r\n");

    if (m_closeConnection)
    {
        output->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", m_body.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }
                // zzz 好麻烦。。。其他server工程 是怎么实现的。
    for (const auto& header : m_headers)
    {
        output->append(header.first);
        output->append(": ");
        output->append(header.second);
        output->append("\r\n");
    }
    output->append("\r\n");
    output->append(m_body);
}
