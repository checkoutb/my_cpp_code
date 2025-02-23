#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main ()
{
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char ch = 'A';

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 主机字节序, 网络字节序
    // address.sin_port = 9999;
    address.sin_port = htons(9999);

    len = sizeof(address);

    // 下面和 01_simple_client 中一样。所以复制
    
    // 将我们的套接字连接到服务器的套接字
    result = connect(sockfd, (struct sockaddr *) &address, len);
    if (result == -1)
    {
        perror("client connect failed");
        exit(EXIT_FAILURE);
    }

    // 通过sockfd进行读写操作
    write(sockfd, &ch, 1);
    read(sockfd, &ch, 1);
    printf("char from server: %c", ch);
    close(sockfd);
    
    exit(EXIT_SUCCESS);

}



