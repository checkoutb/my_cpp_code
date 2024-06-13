

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define MMAP_SIZE 32
#define MMAP_FILENAME "qqq.ignore"


// https://www.cnblogs.com/bwbfight/p/11181631.html



struct AAA {
  int id;
  char name[20];
};
int mmap_2_process() {
  pid_t pid = fork();

  if (pid == 0) {
    struct AAA aaa = {123, "zzaaqq"};
    int fd = open(MMAP_FILENAME, O_RDWR | O_CREAT, 0777);
    ftruncate(fd, sizeof(aaa));

    // printf("test. %d, %d", sizeof(aaa), sizeof(struct AAA));  24, 24
    
    char *mem = mmap(NULL, sizeof(aaa), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
      perror("mmap failed");
      exit(EXIT_FAILURE);
    }
    close(fd);

    memcpy(mem, &aaa, sizeof(aaa));

    munmap(mem, sizeof(aaa));

    exit(EXIT_SUCCESS);
  }
  else {
    //sleep(1);
    int fd = open(MMAP_FILENAME, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
      perror("open failed");
      exit(EXIT_FAILURE);
    }
    int sz1 = sizeof(struct AAA);
    struct AAA *mem = mmap(NULL, sz1, PROT_READ, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
      perror("mmap error");
      exit(EXIT_FAILURE);
    }
    close(fd);

    sleep(1); // 第一次执行，文件中可能还没有数据，会 总线错误
    printf("id: %d, name: %s\n", mem->id, mem->name);

    munmap(mem, sz1);
    exit(EXIT_SUCCESS);
  }
}


int mmap_anony() {

  char *p = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
  if (p == MAP_FAILED) {
    perror("mmap error");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();

  if (pid == 0) {
    sprintf(p, "%s", "sub mmap anno.");
    munmap(p, MMAP_SIZE);
    exit(EXIT_SUCCESS);
  }
  else {
    sleep(1);
    printf("%s\n", p);
    munmap(p, MMAP_SIZE);
    exit(EXIT_SUCCESS);
  }
  
}


// 这个有问题，不知道是什么功能。无法从文件中读取数据，也无法写入到文件中。每次执行完后，文件会被改成二进制
int mmap_file_memory() {
  int fd = open("_mmap_01.c", O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }
  int len = lseek(fd, 3, SEEK_SET);
  printf("len1 : %d\n", len);
  write(fd, "\0", 1);
  len = lseek(fd, 3, SEEK_END);
  printf("len2 : %d\n", len);

  char *mem = mmap(NULL, len, PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (mem == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
  }
  close(fd);

  strcpy(mem, "asd");
  printf("%s\n", mem);

  munmap(mem, len);
  return 0;
}






int main() {
  //mmap_anony();

  //mmap_file_memory(); // error

  mmap_2_process();
  
}


