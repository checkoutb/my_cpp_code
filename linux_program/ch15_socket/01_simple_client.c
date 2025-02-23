
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int sockfd;
    int len;
    struct sockaddr_un address;
    int result;
    char ch = 'A';

    // 创建一个套接字
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 根据服务器的情况给套接字命名
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, "server_socket");
    len = sizeof(address);

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