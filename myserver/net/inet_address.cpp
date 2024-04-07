
#include "../include/inet_address.h"

InetAddress::InetAddress(uint16_t port, std::string ip)
{
    ::bzero(&m_addr, sizeof(m_addr));   // 将一段内存区域清零,即将这段内存区域中的所有字节都设置为0
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = ::htons(port);    // 将一个无符号短整型数值转换为网络字节序,即大端模式(big-endian)
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

std::string InetAddress::toIp() const
{
    char buf[64] = {0};     // zzz 不用这么长吧。 IPv6也不需要这么长。
    ::inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof(buf));
    return buf;
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &m_addr.sin_addr, buf, sizeof(buf));
    size_t end = ::strlen(buf);
    uint16_t port = ::ntohs(m_addr.sin_port);
    sprintf(buf+end, ":%u", port);
    return buf;
}

uint16_t InetAddress::toPort() const
{
    return ::ntohs(m_addr.sin_port);
}
