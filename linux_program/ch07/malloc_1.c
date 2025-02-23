
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define ONE_MB (1024 * 1024)

int main()
{
    char *some_memory;
    int mb = ONE_MB;
    int exit_code = EXIT_FAILURE;

// #ifndef __SIZE_TYPE__
// #define __SIZE_TYPE__ long unsigned int
// #endif
// #if !(defined (__GNUG__) && defined (size_t))
// typedef __SIZE_TYPE__ size_t;

    some_memory = (char *) malloc((size_t) mb);
    if (some_memory != NULL)
    {
        sprintf(some_memory, "hi world\n");
        printf("mem: %s", some_memory);
        exit_code = EXIT_SUCCESS;
    }
    exit(exit_code);
}
