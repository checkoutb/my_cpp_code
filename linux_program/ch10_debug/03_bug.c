
#include <stdio.h>


int f(int a)
{
    return a / 0;
}

int main()
{
    int a = 123;
    printf("%d\n", a);

    f(a);

    return 0;
}
