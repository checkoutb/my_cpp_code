#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int alarm_fired = 0;

void ding(int sig)
{
    alarm_fired = 1;
}

int main()
{
    pid_t pid;
    printf("alarm app starting\n");

    pid = fork();
    switch(pid)
    {
    case -1:
        perror("fork failed...\n");
        exit(1);
    case 0:
        // child
        sleep(2);
        kill(getppid(), SIGALRM);
        exit(0);
    }

    printf("ppid wait alarm\n");
    (void) signal(SIGALRM, ding);  // 安排好捕获SIGALRM信号的工作

    pause();    // pause,它的作用很简单,就是把程序的执行挂起直到有一个信号出现为止。

    if (alarm_fired)
        printf("Ding\n");
    
    printf("done\n");
    exit(0);
}
