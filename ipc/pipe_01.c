

#include <unistd.h> // pipe fork
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// copy from https://zhuanlan.zhihu.com/p/405664139

// 匿名管道 anonymouse pipe

// fork
int pipe_2() {
  int data_processed;
  int file_pipes[2];
  const char some_data[] = "azzz";
  char buffer[BUFSIZ + 1];
  pid_t fork_result;
  memset(buffer, '\0', sizeof(buffer));

  if (pipe(file_pipes) == 0) {
    fork_result = fork();
    if (fork_result == -1) {
      fprintf(stderr, "fork failed");
      exit(EXIT_FAILURE);
    }
    else if (fork_result == 0) {
      // sub process, read
      close(file_pipes[1]);
      data_processed = read(file_pipes[0], buffer, BUFSIZ);
      printf("read %d bytes: %s\n", data_processed, buffer);
      close(file_pipes[0]);
      exit(EXIT_SUCCESS);
    }
    else {
      close(file_pipes[0]);
      data_processed = write(file_pipes[1], some_data, strlen(some_data));
      close(file_pipes[1]);
      printf("write %d bytes\n", data_processed);
    }
  }
  exit(EXIT_SUCCESS);
}



int popen_1() {
  FILE *read_fp;
  printf("%d\n", BUFSIZ);  // 8192
  char buffer[BUFSIZ + 1];
  int chars_read;
  memset(buffer, '\0', sizeof(buffer));
  read_fp = popen("ls -la", "r"); // fork，然后子进程执行 `/bin/sh -c 'ls -la'`
  if (read_fp != NULL) {
    chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);
    if (chars_read > 0) {
      printf("output was:-\n%s\n", buffer);
    }
    pclose(read_fp);
    exit(EXIT_SUCCESS);
  }
  exit(EXIT_FAILURE);
}


int popen_2() {
  FILE *write_fp;
  char buffer[BUFSIZ + 1];
  memset(buffer, '\0', sizeof(buffer));
  sprintf(buffer, "abc123baa777cs");
  write_fp = popen("grep -E '[0-9]+' -o", "w"); // 原文是 '\\d+', 但是执行的时候，grep警告 d前面有多余的\， 没有输出(或者输出空) ；
      // 改成 '\d+' , 编译警告，执行 没有输出(或输出空);
      // 想命令行执行，没有办法，这个好像必须带 文件名，没办法直接一个 string？；
      // 最后改成 '[0-9]+' 可以，输出 123(换行)777
  
  if (write_fp != NULL) {
    fwrite(buffer, sizeof(char), strlen(buffer), write_fp);
    //printf("%s", buffer); // 就是 上面的 abc123aa777cs
    pclose(write_fp);
    exit(EXIT_SUCCESS);
  }
  exit(EXIT_FAILURE);
}



int pipe_1() {
  int data_processed;
  int file_pipes[2];
  const char some_data[] = "12中文3";
  char buffer[BUFSIZ + 1];
  memset(buffer, '\0', sizeof(buffer));

  if (pipe(file_pipes) == 0) {
    data_processed = write(file_pipes[1], some_data, strlen(some_data));
    printf("write %d bytes\n", data_processed); // 9byte, 3byte一个中文， utf-8?

    data_processed = read(file_pipes[0], buffer, BUFSIZ);
    printf("read %d bytes: %s\n", data_processed, buffer);
    exit(EXIT_SUCCESS);
  }
  return -1;
}




int main() {
  //popen_1();

  //popen_2();

  //pipe_1();

  pipe_2();
  
}


