#pragma once

#include <unordered_map>

#include "noncopyable.h"
#include "timestamp.h"


class HttpRequest
{
public:
    enum Method { Invalid, GET, POST, HEAD, PUT, DELETE};
    enum Version { Unknown, Http10, Http11 };

    HttpRequest() : m_method {Invalid}, m_version {Unknown}
    {}

    void setVersion(Version v) { m_version = v; }

    Version getVersion() const { return m_version; }

    bool setMethod(const char* start, const char* end)
    {
        std::string m (start, end);
        if (m == "GET")
            m_method = Method::GET;
        else if (m == "POST")
            m_method = POST;
        else if (m == "HEAD")
            m_method = HEAD;
        else if (m == "PUT")
            m_method = PUT;
        else if (m == "DELETE")
            m_method = DELETE;
        else
            m_method = Invalid;

        return m_method != Invalid;
    }

    Method getMethod() const { return m_method; }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch(m_method)
        {
            case GET:
                result = "GET";
                break;
            case Method::POST:
                result = "POST";
                break;
            case HEAD:
                result = "HEAD";
                break;
            case PUT:
                result = "PUT";
                break;
            case DELETE:
                result = "DELETE";
                break;
            default :
                break;
        }
        return result;
    }

    void setPath(const char* start, const char* end)
    {
        m_path.assign(start, end);
    }
    const std::string& getPath() const { return m_path; }

    void setQuery(const char* start, const char* end)
    {
        m_query.assign(start, end);
    }
    const std::string& getQuery() const { return m_query; }

    void setReceiveTime(Timestamp t) { m_receiveTime = t; }
    Timestamp getReceiveTime() const { return m_receiveTime; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        std::string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon))
            ++colon;

        std::string value(colon, end);

        while (!value.empty() && isspace(value.back()))
            value.pop_back();       // value.resize(value.size() - 1)

        m_headers[field] = value;
    }
    std::string getHeader(const std::string& field) const
    {
        std::string result;
        auto it = m_headers.find(field);
        if (it != m_headers.end())
            result = it->second;
        return result;
    }

    const std::unordered_map<std::string, std::string>& headers() const
    {
        return m_headers;
    }

    void swap(HttpRequest &rhs)
    {
        using std::swap;
        swap(m_method, rhs.m_method);       // zzz 临时变量，然后 httpRequest 整个swap。  目前这个，并发应该会有问题吧。 但是，会有并发吗？
        swap(m_version, rhs.m_version);
        m_path.swap(rhs.m_path);
        m_query.swap(rhs.m_query);
        swap(m_receiveTime, rhs.m_receiveTime);
        m_headers.swap(rhs.m_headers);
    }

private:

    Method m_method;
    Version m_version;
    std::string m_path;
    std::string m_query;
    Timestamp m_receiveTime;
    std::unordered_map<std::string, std::string> m_headers;

};

