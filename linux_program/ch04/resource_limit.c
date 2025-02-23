
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// work 函数将一个字符串写入一个临时文件 10000次，然后做一些算术运算以产生CPU负载
void work()
{
    FILE *f;
    int i;
    double x = 4.5;

    f = tmpfile();
    for (i = 0; i < 10000; ++i)
    {
        fprintf(f, "do sth output\n");
        if (ferror(f)) {
            fprintf(stderr, "error writing to tmp file\n");
            exit(1);
        }
    }
    for (i = 0; i < 1000000; ++i)
    {
        x = log((double) x*x + 3.21);
    }
}


int main()
{
    struct rusage r_usage;
    struct rlimit r_limit;
    int priority;

    // 调用work，通过 getrusage 获得消耗的CPU 时间
    work();
    getrusage(RUSAGE_SELF, &r_usage);

    printf("cpu usage: user: %ld.%06ld, system: %ld.%06ld\n", 
        r_usage.ru_utime.tv_sec, r_usage.ru_utime.tv_usec,
        r_usage.ru_stime.tv_sec, r_usage.ru_stime.tv_usec
    );


    // 设置 优先级和 文件大小限制
    priority = getpriority(PRIO_PROCESS, getpid());
    printf("now priority: %d\n", priority);

    getrlimit(RLIMIT_FSIZE, &r_limit);
    printf("now file size limit: soft: %ld, hard: %ld\n", r_limit.rlim_cur, r_limit.rlim_max);


    // setrlimit 设置文件大小限制，并再次调用work，这次work 会失败，因为它试图创建一个太大的文件
    r_limit.rlim_cur = 2048;
    r_limit.rlim_max = 4096;
    printf("set 2k to file size limit\n");
    setrlimit(RLIMIT_FSIZE, &r_limit);

    work();
    exit(0);
}



