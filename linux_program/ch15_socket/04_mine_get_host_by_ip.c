#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


// https://blog.csdn.net/weixin_42839065/article/details/131065640
int main()
{
    // char* ip = "180.101.50.242";    // baidu
    // char* ip2 = ""

    // baidu, blog.csdn
    // 2.0.0.1 是 虚拟机的 宿主机的联系IP
    // 131 是宿主机的自己的局域网IP , 1是网关，98是131 arp -a 中随便找了一台
    // 127.0.0.1 ke yi, 其他都是 unknow host，
    char* ips[] = {"180.101.50.242","220.185.184.29","127.0.0.1","2.0.0.1"
    ,"192.168.18.131","192.168.18.1","192.168.18.98"};

    struct hostent *host;
    struct sockaddr_in addr;
    for (int i = 0; i < sizeof(ips) / sizeof(ips[0]); ++i)
    {
        fprintf(stdout, "ip: %s, %lu, %lu, %d, %d\n", ips[i], sizeof(ips), sizeof(ips[i]), (int) (sizeof(ips) / sizeof(ips[0])), i);
        
        // addr.sin_addr.s_addr = inet_addr(ips[i]);
        if (!inet_aton(ips[i], &addr.sin_addr)) 
        {
            printf("inet_aton error\n");
            continue;
        }

        host = gethostbyaddr((void *) &addr.sin_addr, 4, AF_INET);
        
        printf("host: %p\n", host);     // nil
        // nil + errno==0, 确实没有找到

        if (!host)
        {
            fprintf(stderr, "get host error: %s\n", ips[i]);
            // errno - errno.h , h_errno - netdb.h
            fprintf(stderr, " -- %d -- %d\n", errno, h_errno);     // 0  2
            // 之前用的 strerror
            fprintf(stderr, " -- %s\n", hstrerror(h_errno)); // No such file or directory
            continue;
        }
        printf("official name: %s\n", host -> h_name);

        for (int i = 0; i < sizeof(host -> h_aliases) / sizeof(host -> h_aliases[0]); ++i)
        {
            printf("alias: %d: %s\n", i, host -> h_aliases[i]);
        }

        printf("address type: %s\n", host -> h_addrtype == AF_INET ? "AF_INET" : "AF_INET6");

        for (int i = 0; i < sizeof(host -> h_addr_list) / sizeof(host -> h_addr_list[0]); ++i)
        {
            printf("ip address: %d: %s\n", i, inet_ntoa(*(struct in_addr *) host -> h_addr_list[i]));
        }
    }
    exit(EXIT_SUCCESS);
}