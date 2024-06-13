

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


#define PORT 3456


// see 01_server.c

int main(int argc, char const* argv[])
{
  int status, valread, client_fd;
  struct sockaddr_in serv_addr;
  char* hello = "hello from client";
  char buffer[1024] = {0};

  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket create error");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr) <= 0) {
    printf("invalid address, address not supported");
    return -1;
  }

  if ((status = connect(client_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
    printf("connect failed");
    return -1;
  }

  send(client_fd, hello, strlen(hello), 0);

  printf("hello msg sent");
  //valread = read(client_fd, buffer, 1024 - 1);

  //printf("%s\n", buffer);

  close(client_fd);
  return 0;
}







