

#include <cstdio>
#include <unistd.h>  // man fork
#include <stdio.h>
#include <sys/wait.h>


/*

僵尸进程： 自己是子进程，已执行完，但 父进程还在运行中，且 父进程没有 wait waitpid 获得 子进程的 返回码

gcc zombie_process_waitpid.c
./a.out &
ps -l | grep 'Z'   1分钟内可以看到，1分钟后 parent退出，就查不到了。时间改了，反正立刻执行就可以
  上面的命令需要在同一个终端中执行，才可以看到僵尸进程
ps aux | grep 'Z'  其他终端中运行这条命令可以看到僵尸进程


>>> man 2 waitpid <<<
调用waitpid，直接用了 子进程id，所以会 卡住(因为option传的是0)，等待 子进程结束。
第一个参数可以输入
- -1， 等待任意一个子进程
- 0， 等待进程组id相同的任意子进程
- 正数， 等待该进程
- 小于-1， 等待 进程组id等于它的绝对值的 任意子进程

第二个参数是 子进程的return 的值，需要小于128(?)

第三个参数是 控制waitpid的行为，最常用的是 WNOHANG。 立即返回，不会阻塞


... ps -ef | grep 'Z' 没有
... top  然后摁z，全变红了，所以也不行


*/

void fun01() {
    int pid = fork();

    if (pid == 0) {
        // child
        sleep(5);
        printf("child process bron, live, die\n");
        
    } else {
        // parent
        int stat = 0;
        printf("child process id %d.\n", pid);
        // waitpid(pid, &stat, 0);
        printf("check child stat: %d\n", WEXITSTATUS(stat));
        sleep(10);
        printf("wait wait wait\n");
        // waitpid(pid, &stat, 0);
        printf("222 waitpid, stat: %d\n", WEXITSTATUS(stat));
        sleep(30);
        printf("parent process die\n");
    }

}


int main(int argc, char **argv) {

    fun01();
    
    // return 0;
    return 0; // child process will use this...
}


