#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *host;
    int sockfd;
    int len, result;
    struct sockaddr_in address;
    struct hostent *hostinfo;
    struct servent *servinfo;
    char buffer[128];

    if (argc == 1)
        host = "localhost";
    else
        host = argv[1];
    
    // 查找主机的地址
    hostinfo = gethostbyname(host);
    if (!hostinfo)
    {
        fprintf(stderr, "no host: %s\n", host);
        exit(1);
    }

    // 检查主机上是否有daytime服务
    // servinfo = getservbyname("daytime", "tcp");  // 。。这个也可以的。因为端口相同
    servinfo = getservbyname("daytime", "udp");
    if (!servinfo)
    {
        fprintf(stderr, "no daytime service\n");
        exit(EXIT_FAILURE);
    }
    printf("daytime port is %d\n", ntohs(servinfo->s_port));

    // 创建一个套接字
    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 构造connect调用要使用的地址
    address.sin_family = AF_INET;
    address.sin_port = servinfo->s_port;
    address.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
    len = sizeof(address);

    // 建立连接并取得有关信息
    // result = connect(sockfd, (struct sockaddr *) &address, len);
    // if (result == -1)
    // {
    //     perror("getdate error");
    //     exit(EXIT_FAILURE);
    // }
    // result = read(sockfd, buffer, sizeof(buffer));

    result = sendto(sockfd, buffer, 1, 0, (struct sockaddr *) &address, len);
    result = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &address, &len);

    buffer[result] = '\0';
    printf("get %d bytes: %s\n", result, buffer);

    close(sockfd);
    exit(EXIT_SUCCESS);
}