
// #define __USE_XOPEN     // time.h 中 strptime 有 #ifdef
#define _XOPEN_SOURCE

/*
最终这个可运行，ubuntu 22

编译失败，找不到strptime，visual studio code 点开 time.h。看到
#ifdef __USE_XOPEN
extern char *strptime (const char *__restrict __s,
		       const char *__restrict __fmt, struct tm *__tp)
     __THROW;
#endif

所以有了第一行的 #define __USE_XOPEN， 不过无效。 还是找不到。

baidu后，说编译时 增加 -D _XOPEN_SOURCE
也给了图，图中是 #define _XOPEN_SOURCE。


vs code在 time.h 中点击 __USE_XOPEN 后会跳到 features.h ，可以看到如下的宏

#ifdef	_XOPEN_SOURCE
# define __USE_XOPEN	1
# if (_XOPEN_SOURCE - 0) >= 500


。。不过还是不清楚，为什么 我直接 #define __USE_XOPEN 不行。
看到了， features.h 中
#undef	__USE_XOPEN

。。太贱啦！！！

*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // compiler need for strcpy


int main()
{
    struct tm *tm_ptr, timestruct;
    time_t the_time;
    char buf[256];
    char *result;

    (void) time(&the_time);
    tm_ptr = localtime(&the_time);
    strftime(buf, 256, "%A %d %B, %I:%S %p", tm_ptr);

    printf("strftime gives: %s\n", buf);

    strcpy(buf, "Thu 1 July 2022, 12:34 will do fine");

    printf("calling strptime with %s\n", buf);
    tm_ptr = &timestruct;

    result = strptime(buf, "%a %d %b %Y, %R", tm_ptr);
    printf("strptime consume to : %s\n", result);

    printf("strptime gives:\n");
    printf("date: %02d/%02d/%02d\n", tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
    printf("time: %02d:%02d\n", tm_ptr->tm_hour, tm_ptr->tm_min);

    exit(0);
}

