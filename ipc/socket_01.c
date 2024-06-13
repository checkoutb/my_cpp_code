

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


// https://zhuanlan.zhihu.com/p/672264623



int socket_1() {
  pid_t pid = fork();
  if (pid == 0) {
    // sub, client
    sleep(1);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4455);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    connect(client_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

    char buffer[32] = "hi ipc";
    write(client_fd, buffer, sizeof(buffer));
    close(client_fd);
    
  }
  else {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4455);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    //////
    // while (1)
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_len);
    char buffer[16];
    memset(buffer, '\0', sizeof(buffer));
    read(client_fd, buffer, sizeof(buffer));
    printf("main got: %s\n", buffer);
    close(client_fd);

    /////
    close(server_fd);
  }
  return 0;
}



int main() {
  socket_1();
}





