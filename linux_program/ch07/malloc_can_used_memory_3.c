
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ONE_K (1024)

int main()
{
    char *some_mem;
    int allo_size = ONE_K;
    int megs = 0;
    int ks = 0;

    // while (true)     // need <stdbool.h> 。。。。。unbelievable.
    while (1)
    {
        for (ks = 0; ks < 1024; ++ks)
        {
            some_mem = (char *) malloc(allo_size);
            if (some_mem == NULL)
                exit(EXIT_FAILURE);
            
            sprintf(some_mem, "hi world");
        }
        ++megs;
        printf("now allocated %d MB\n", megs);
    }
    exit(EXIT_SUCCESS);
}

