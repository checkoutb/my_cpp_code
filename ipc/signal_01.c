

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// https://zhuanlan.zhihu.com/p/672264623
// https://blog.csdn.net/m0_56051805/article/details/125768927

// 信号， 带参数，不带参数

void sHandler(int signum) {
  printf("pid: %d, interrupt signal %d\n", getpid(), signum);
  // if (signum == xx) { xxx } else if (xx) {xx}
  exit(signum);
}

int signal_1() {
  signal(SIGINT, sHandler); // ctrl + c
  while (1) {
    printf("waiting..\n");
    sleep(1);
  }
  return 0;
}


/////////////////////

int signal_no_arg() {
  signal(SIGUSR1, sHandler);
  signal(SIGUSR2, sHandler);

  pid_t pid = fork();

  if (pid == 0) {
    // sub
    while (1) {
      printf("sub process %d...\n", getpid());
      sleep(1);
    }
  }
  else {
    sleep(3);
    printf("main SIGUSE2...\n");
    kill(pid, SIGUSR2);
    
  }
}


///////////////////////

void sArgHandler(int num, siginfo_t* pinfo, void* pvo) {
  int res = pinfo->si_int;
  printf("pid: %d, res: %d, signum: %d", getpid(), res, num);
  exit(num); // 不然 得 手动kill
}

int signal_arg() {
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    // sub
    struct sigaction act;
    act.sa_sigaction = sArgHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &act, NULL);
    while (1) {
      printf("sub process %d\n", getpid());
      sleep(1);
    }
  }
  else {
    sleep(3);
    union sigval value;
    value.sival_int = 332244;
    sigqueue(pid, SIGUSR2, value);
  }
  
}


int main() {
  //signal_1();

  //signal_no_arg();

  signal_arg();
}


