
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    time_t timeval;

    (void) time(&timeval);
    printf("date is %s\n", ctime(&timeval));

    exit(0);
}

