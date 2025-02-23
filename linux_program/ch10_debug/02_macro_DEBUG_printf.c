

#include <stdio.h>
#include <stdlib.h>

int main()
{
#ifdef DEBUG
    printf("Compiled: " __DATE__ " @ " __TIME__ "\n");
#endif

    printf("hi\n");
    exit(0);
}
