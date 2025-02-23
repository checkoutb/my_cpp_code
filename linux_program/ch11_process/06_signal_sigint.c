
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

// 信号出现时,程序调用该函数,它先打印一条
// 消息,然后将信号SIGINT(默认情况下,按下
// Ctrl+C将产生这个信号)的处理方式恢复为默认行
// 为。
void ouch(int sig)
{
    printf("got signal: %d\n", sig);
    (void) signal(SIGINT, SIG_DFL);
}

int main()
{
    (void) signal(SIGINT, ouch);
    while (1)
    {
        printf("hi\n");
        sleep(1);
    }
}
