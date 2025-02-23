#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int arg;
    for (arg = 0; arg < argc; ++arg)
    {
        if (argv[arg][0] == '-')
            printf("option: %s\n", argv[arg] + 1);          // 。。char *, 所以 +1 ，就是从 第二个char开始输出。 跳过了 - 。
        else
            printf("argument %d: %s\n", arg, argv[arg]);
    }
    exit(0);
}
