

#include <stdio.h>
#include <string.h> // strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> // close, sleep
#include <arpa/inet.h> // close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/times.h> // FD_SET, FD_ISSET, FD_ZERO


#define TRUE 1
#define FALSE 0
#define PORT 3456


/*

https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/

使用 01_client 来访问这个server

持续 ./01_client, (大约十几次后) server 会出现 connection by peer 的错误。

read失败需要判断 errno，如果是 104 之类的，就可以 直接close。
*/


int main(int argc, char *argv[])
{
  int opt = TRUE;
  int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
  int max_sd;
  struct sockaddr_in address;
  char buffer[1025];
  fd_set readfds;
  char *message = "echo demo v1.0\r\n";

  for (i = 0; i < max_clients; i++) {
    client_socket[i] = 0;
  }

  if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
    perror("setsocket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(master_socket, (struct sockaddr*) &address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(master_socket, 3) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(address);
  puts("waiting for connections ...");

  while (TRUE) {
    FD_ZERO(&readfds);
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    for (i = 0; i < max_clients; ++i) {
      sd = client_socket[i];
      
      if (sd > 0)
        FD_SET(sd, &readfds);
      
      if (sd > max_sd)
        max_sd = sd;
    }

    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR)) {
      printf("select error");
    }
    printf("before iter readfds\n"); // 1次 01_client， 3次输出
    if (FD_ISSET(master_socket, &readfds)) {
      if ((new_socket = accept(master_socket, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
      }

      printf("new connection, socket fd is %d, ip is %s, port is %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      //sleep(1); // ..
      
      if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
        perror("send failed");
      }

      puts("welcome message sent successfully");

      for (i = 0; i < max_clients; ++i) {
        if (client_socket[i] == 0) {
          client_socket[i] = new_socket;
          printf("add new socket to list of socket as %d\n", i);
          break;
        }
      }
    }

    for (i = 0; i < max_clients; ++i) {
      sd = client_socket[i];
      if (FD_ISSET(sd, &readfds)) {
        // printf("has data to read %d\n", i);
        if ((valread = read(sd, buffer, 1024)) == 0) {
          printf(" == 0\n");
          getpeername(sd, (struct sockaddr*) &address, (socklen_t*) &addrlen);
          printf("host disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
          close(sd);
          client_socket[i] = 0;
        }
        else if (valread < 0) {
          printf("valread < 0, is %d\n", valread);
          printf("error info: %d, %s\n", errno, strerror(errno)); // 104, Connection reset by peer
          perror("another error info print: "); // 使用了 01_client.c 来访问这个server，01_client 会主动close，估计是 01_client 先close了， 导致这里 read 不了。 但是 怎么通过 select ？
          // 而且，很难理解，为什么 一次 01_client 会触发 这个 for 循环 3次。
          // 第一次 select返回， 01_client的 连接请求,  从select 返回后， 是 master_socket， 进行 accept，
          //   accept的时候 将 new_socket 放到了 client_socket 中，但是 没有经过 select， 所以 FS_ISSET 肯定是 否, 所以 这个 for 循环 空转一次 (当然，如果其他 socket 有数据，这里会处理的，所以这个for循环是必须的)。
          // 第二次 select返回， 01_client 的 send ， 从 select 返回后， master_socket 没有数据
          //   这个for循环 处理数据， 走的是最下面的 > 0 分支。 发送数据到 01_client
          // 第三次 select返回， 01_client 收到数据后，就close了， 所以 需要关闭 连接。
          //   所以 第三次for循环， 发现 read 的返回是 0， server 进行 关闭操作。
          //
          // 问题是，怎么触发了 104错误。 通过日志， 是第三次 for 循环触发了 104错误。 而且出错时 前面的日志没有问题。
          // 估计就是： 对方发送数据，然后 close， 我开始 read， 会发生什么？ 是直接报错？ 还是 读取对方发送的数据，下次read的时候 报错？
          //
          // server accept后 sleep 1秒， 删除 >0分支的 send，   client改成 send 后立刻close (不再 read)
          //   没有问题，还是一共3次select， 第一次是 accept， 第二次是 read client发送的数据， 第三次是 读取 client 的 close。
          //
          // 才发现 >0 分支的 send 的数据，01_client 不会接收的， 因为 01_client 只有一次 read，读的是 104行， accept 后的 send ， 是不是这个 send 和 对方的close 产生了一点奇怪的问题？ 但也应该是 send 的时候就报错了啊。
          // 应该不是send 的问题， 之前 sleep 的时候， 也没有问题。
          // 搞不清楚了， sleep 的情况下， 104的 send 没有问题，但是 >0 分支的 send 就有问题。 但是在 104行的时候， client 肯定已经关闭了的。
          //
          // 是否和 缓冲区有关？
          //
          // 不过 我看网上的写法，报错后，会判断下 errno 的，所以 估计 这种 connection reset by peer 这种错误，是需要 在 编码时 考虑到的。 不知道还有那些 是需要在 代码时 考虑进去。
          exit(EXIT_FAILURE);
        }
        else {
          printf(" > 0 , %d, %s\n", valread, buffer);
          buffer[valread] = '\0';
          send(sd, buffer, strlen(buffer), 0);
        }
      }
    }
  }
  return 0;
}













