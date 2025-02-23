
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ONE_MB (1024 * 1024)
#define ALL_MEM (1024 * 5)          // 按照实际调整，本次虚拟机分配了5g内存

int main()
{
    char *some_mem;
    size_t allocate_size = ONE_MB;
    int megs_obtained = 0;

    while (megs_obtained < (ALL_MEM * 2))
    {
        some_mem = (char *) malloc((size_t) allocate_size);
        if (some_mem != NULL)
        {
            ++megs_obtained;
            sprintf(some_mem, "hi world");
            printf("%s - now allocated % d MB\n", some_mem, megs_obtained);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}

