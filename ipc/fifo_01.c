
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

// https://zhuanlan.zhihu.com/p/672264623

// named pipe

int fifo_1() {
  const char* fifo_file = "/tmp/my_fifo_123";
  mkfifo(fifo_file, 0666);

  if (fork() == 0) {
    // sub
    char msg[] = "hi fifo";
    int fd = open(fifo_file, O_WRONLY);
    int t2 = write(fd, msg, sizeof(msg));
    printf("write %d\n", t2);
    close(fd);
  }
  else {
    char buffer[16] = {0};
    int fd = open(fifo_file, O_RDONLY);
    int t2 = read(fd, buffer, sizeof(buffer));
    printf("read, %d, %d, %s\n", fd, t2, buffer); // 3, 8, hi fifo
    close(fd);
  }
  return 0;
}


int main() {
  fifo_1();
}


