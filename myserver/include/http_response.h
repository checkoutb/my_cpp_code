#pragma once

#include <unordered_map>
#include <string>       // ...zzz 原文没有这个

class Buffer;

class HttpResponse
{
public:
    enum HttpStatusCode
    {
        kUnknown, k200_ok = 200, k301_movedPermanently = 301, k400_badRequest = 400, k404_notFound = 404,
    };

    explicit HttpResponse(bool close) : m_statusCode { kUnknown }, m_closeConnection { close }
    {}

    void setStatusCode(HttpStatusCode code) { m_statusCode = code; }
    void setStatusMsg(const std::string& msg) { m_statusMsg = msg; }
    void setCloseConnection(bool on) { m_closeConnection = on; }
    bool getCloseConnection() const { return m_closeConnection; }

    void addHeader(const std::string& key, const std::string& value) { m_headers[key] = value; }
    void setBody(const std::string& body) { m_body = body; }
    void appendToBuffer(Buffer* output) const;

    void setContentType(const std::string& contentType) { addHeader("Content-Type", contentType); }


private:
    std::unordered_map<std::string, std::string> m_headers;
    HttpStatusCode m_statusCode;
    std::string m_statusMsg;
    bool m_closeConnection;
    std::string m_body;
};
