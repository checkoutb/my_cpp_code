
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    char *mem = (char *) 0;

    printf("a read : %s\n", mem);

    sprintf(mem, "write\n");
    exit(EXIT_SUCCESS);
}
