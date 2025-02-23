
#include <stdlib.h>
#include <stdio.h>

int main()
{
    printf("run ps with system\n");
    
    // system("ps ax");     // 输出完，然后 done
    system("ps ax &");      // 输出 和 done 同时发生

    printf("done\n");
    exit(0);
}

