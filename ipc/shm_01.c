
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h> // man shm_open
#include <fcntl.h> // O_*

#incldue <sys/types.h>
//#include <linux/memfd.h>

#include <sys/syscall.h>

#define SHM_SIZE 32
#define SHM_PATH "shm_01.c"
#define SHM_NAME "ZZxxAA"


// https://www.cnblogs.com/zhw618/p/15854216.html
// https://zhuanlan.zhihu.com/p/574398364

// sys v, posix

// `ls /dev/shm/` 会发现 ZZxxAA
// `free` 的 shared 字段
// `ipcs`

// 使用 memfd_create 代替 shm_open

// 还真没有。。 /usr/include/sys/mman.h 中确实没有 memfd_create 。不知道去哪里找。  man menfd_create 说 sys/mman.h



// 无法共享。
// https://github.com/a-darwish/memfd-examples/tree/master
// 这个太复杂了。 找不到简单点的。。
int shm_mem_eate_error() {
  
  pid_t pid = fork();  
  
  if (pid == 0) {
    //int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    //int fd = memfd_create("nnn", 0);
    int fd = syscall(SYS_memfd_create, ".", FD_CLOEXEC);
    ftruncate(fd, SHM_SIZE);

    char* p = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //memset(p, 'A', SHM_SIZE);
    sprintf(p, "%s", "hi from sub shm posix");
    munmap(p, SHM_SIZE);
    printf("%d, %p, %p\n", fd, p, MAP_FAILED);
    return 0;
  }
  else {
    //int fd = shm_open(SHM_NAME, O_CREAT | O_RDONLY, 0666);
    //int fd = memfd_create("nnn", 0);
    sleep(1);
    int fd = syscall(SYS_memfd_create, ".", FD_CLOEXEC);
    ftruncate(fd, SHM_SIZE);
    char *p = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    //strcpy;
    sleep(1);
    printf("%s\n", p);
    munmap(p, SHM_SIZE);
    printf("%d, %p, %p\n", fd, p, MAP_FAILED);
    return 0;
  }
  return -1;
}


int shm_posix() {
  
  pid_t pid = fork();  
  
  if (pid == 0) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SHM_SIZE);

    char* p = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //memset(p, 'A', SHM_SIZE);
    sprintf(p, "%s", "hi from sub shm posix");
    munmap(p, SHM_SIZE);
    return 0;
  }
  else {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDONLY, 0666);
    ftruncate(fd, SHM_SIZE);
    char *p = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    //strcpy;
    sleep(1);
    printf("%s\n", p);
    munmap(p, SHM_SIZE);
    return 0;
  }
  return -1;
}


int shm_sys_v() {
  pid_t pid = fork();

  if (pid == 0) {
    key_t key = ftok(SHM_PATH, 123);
    int shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    char* chp = shmat(shm_id, NULL, 0);
    sprintf(chp, "%s", "hi from sub shm sysV");
    shmdt(chp);
    exit(EXIT_SUCCESS);
  }
  else {
    key_t key = ftok(SHM_PATH, 123);
    int shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    char* chp = shmat(shm_id, NULL, SHM_RDONLY);
    sleep(1);
    char buf[32];
    strcpy(buf, chp); // 防止被修改，不过 复制过程中可能被修改。 需要一个 跨进程的锁
    printf("%s  %s\n", buf, chp);
    shmdt(chp);
    exit(EXIT_SUCCESS);
  }
}


int main() {
  //shm_sys_v();

  //shm_posix();

  shm_mem_eate();
}

