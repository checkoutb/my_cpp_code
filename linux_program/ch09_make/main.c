
#include <stdlib.h>
#include "a.h"

extern void fun2();
extern void fun3();

int main()
{
    fun2();
    fun3();
    exit(EXIT_SUCCESS);
}