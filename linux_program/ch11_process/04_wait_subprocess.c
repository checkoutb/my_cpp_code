
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid;
    char *msg;
    int n;
    int exit_code;

    printf("fork starting\n");

    pid = fork();

    switch(pid)
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        msg = "this is child";
        n = 5;
        exit_code = 37;
        break;
    default:
        msg = "this is parent";
        n = 3;
        exit_code = 0;
        break;
    }
    for (; n > 0; --n)
    {
        puts(msg);
        sleep(1);
    }
    if (pid != 0)
    {
        int stat_val;
        pid_t child_pid;

        child_pid = wait(&stat_val);

        printf("child has finished. pid = %d\n", child_pid);

        printf("aaaaaaa %d\n", stat_val);

        if (WIFEXITED(stat_val))
            printf("child exited with code %d\n", WEXITSTATUS(stat_val));       // 37
        else
            printf("child terminated abnormally\n");
    }
    exit(exit_code);
}
