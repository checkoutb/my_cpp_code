

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>


#include <errno.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <string.h>

#define MMAP_FNAME "zzz.ignore"
#define SEM_NAME "/EEddCC"  // ls /dev/shm/sem.EEddCC

// 不知道哪里的问题。。
// 目前看起来 父进程 和 子进程，的 sem 是 不同的对象。
// 但是 都是 这样写的啊。
// 放堆上也不行啊。
// 不知道为什么。
// g

// 确实， 父子进程， 是不同的 进程， 进程里面的 对象 ，虽然 内存位置 一样，但是 值不同。  用到了 COW， 那么 父子进程 的 sem 肯定不是同一个对象， 即使在 堆上， 也不是 不同的堆 上的 相同的 地址，  也是不同对象。
// md, 那IPC 信号量 怎么回事？ 不过 有好几个 sem 实现
// sys/sem.h 不知道行不行。 不行，这个是， 可以认为是 老版的

/*

POSIX 信号量还为您提供了选择是否需要进程本地信号量（用于多线程环境，甚至在某些情况下，单线程程序中的信号处理程序）或进程共享信号量，
在后一种情况下，您还可以选择是让系统按名称在共享命名空间中分配它，还是自己获取共享内存并在共享内存中初始化它

// 确实是这个问题： 进程空间 是完全不同的对象， 上面写了 要在 共享命名空间，或 shmem 中初始化。。  mmap 可以吗？

共享命名空间 好像挺麻烦的。 unshare， setns

*/


// 可以通过shm 来同步 sem 吗？ 好像没有， 而且应该 冲突了， sem_open  shm_open 应该是 同一级别的，所以应该无法通过 shm 来同步 sem 吧






// ok
// https://www.cnblogs.com/BroccoliFighter/p/18046556  无代码
int sem_mmap_named() {
  if (fork() == 0) {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
      perror("sem_open failed");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 5; ++i) {
      sleep(1);
      printf("produce ... %d\n", i);
      sem_post(sem);
      
    }
    sem_close(sem);
    exit(EXIT_SUCCESS);
  }
  else {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
      perror(" sem  open  failed ");
      exit(EXIT_FAILURE);
    }
    printf("consume started\n");
    for (int i = 0; i < 5; ++i) {
      sem_wait(sem);
      printf("consume .. %d\n", i);
    }
    sem_close(sem);
    exit(EXIT_SUCCESS);
  }
}



// ok
// https://www.cnblogs.com/aizquan/p/6007306.html
int sem_mmap_anon() {
  sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
  if (sem == MAP_FAILED) {
    perror("map failed");
    exit(EXIT_FAILURE);
  }
  if (sem_init(sem, 1, 0) < 0) {
    perror("sem_init failed");
    exit(EXIT_FAILURE);
  }
  if (fork() == 0) {
    for (int i = 0; i < 5; ++i) {
      sleep(1);
      printf("produce .. %d\n", i);
      sem_post(sem);
    }
    munmap(sem, sizeof(sem_t));    
    exit(EXIT_SUCCESS);
  }
  else {
    printf("consume.. started..\n");
    for (int i = 0; i < 5; ++i) {
      sem_wait(sem);
      printf("consume .. %d\n", i);
    }
    munmap(sem, sizeof(sem_t));
    exit(EXIT_SUCCESS);
  }
}




// 不行，还是 2个sem 对象，看起来是 sem_init, 或 sem_open， 不能 open + sem_init
int sem_mmap_error() {
  pid_t pid = fork();
  if (pid == 0) {
    sem_t sem;
    sem_init(&sem, 1, 0);
    int sz1 = sizeof(sem_t);
    int fd = open(MMAP_FNAME, O_RDWR | O_CREAT, 0664);
    ftruncate(fd, sz1);

    // sem_t * ?
    char *mm = mmap(NULL, sz1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    memcpy(mm, &sem, sz1);

    for (int i = 0; i < 5; ++i) {
      sleep(1);
      printf("produce ... %d\n", i);
      sem_post(&sem);
    }

    munmap(mm, sz1);
    exit(EXIT_SUCCESS);
  }
  else {
    sleep(1);
    int fd = open(MMAP_FNAME, O_RDWR | O_CREAT, 0664);
    int sz1 = sizeof(sem_t);
    ftruncate(fd, sz1);

    sem_t *mm = mmap(NULL, sz1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    //sleep(1); // wait sem_init
    for (int i = 0; i < 5; ++i) {
      printf("consume... start wait\n");
      sem_wait(mm);
      printf("consume... %d\n", i);
    }
    munmap(mm, sz1);
    exit(EXIT_SUCCESS);
  }
}





sem_t *sem = {};

int sem_1_error() {

  //sem_t sem;

  //sem_t *sem;

  if (sem_init(sem, 1, 0) != 0) { // sem_open
    perror("sem init failed\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();

  if (pid == 0) {
    // sub
    sleep(1);
    int t3 = -1;
    sem_getvalue(sem, &t3);
    printf("sub1 %d\n", t3);
    int t2 = sem_post(sem);
    printf("sub add sem, %d, %p\n", t2, sem);

    sem_getvalue(sem, &t3);
    printf("sub2 %d\n", t3);
    //t2 = sem_post(&sem);
    //printf("sub add ... %d\n", t2);
    //printf("sub3 %ld\n", sem);

    exit(EXIT_SUCCESS);
  }
  //else {
  pid = fork();
  if (pid == 0) {
    sleep(3);
    int t3 = -1;
    sem_getvalue(sem, &t3);
    printf("main start, %p\n", sem);
    printf("main %d\n", t3); // 始终是 init 时设置的值，没有受到 子进程的 sem_post 的影响
    int t2 = sem_wait(sem);
    
    printf("main end\n");
    sem_destroy(sem);

    exit(EXIT_SUCCESS);
  } else {
    //printf(" --- end");
    sleep(5);
    printf("end of allllll");
  }
  return 0;
}



// https://blog.superpat.com/semaphores-on-linux-sem_init-vs-sem_open
int sem_2_error() {
  
  int fd, i,count=0,nloop=5,zero=0,*ptr;
  sem_t mutex;

  //open a file and map it into memory

  /* fd = open("log.txt",O_RDWR|O_CREAT,S_IRWXU); */
  /* write(fd,&zero,sizeof(int)); */
  /* ptr = mmap(NULL,sizeof(int),PROT_READ |PROT_WRITE,MAP_SHARED,fd,0); */
  /* close(fd); */

  /* create, initialize semaphore */
  if( sem_init(&mutex,1,1) < 0)
    {
      perror("semaphore initilization");
      exit(0);
    }
  
  if (fork() == 0) { /* child process*/
    for (i = 0; i < nloop; i++) {
      //sem_wait(&mutex);
      printf("child entered crititical section: %d\n", i);
      //sleep(2);
      printf("child leaving critical section\n");
      sem_post(&mutex);
      //sleep(1);
    }
    exit(0);
  }
  /* back to parent process */
  for (i = 0; i < nloop; i++) {
    sem_wait(&mutex);
    printf("parent entered critical section: %d\n", i);
    //sleep(2);
    printf("parent leaving critical section\n");
    //sem_post(&mutex);
    //sleep(1);
  }
  exit(0);
}


int main() {

  //sem_1();

  //sem_2();

  //sem_mmap_anon();
  //sem_mmap_error();

  sem_mmap_named();
}


