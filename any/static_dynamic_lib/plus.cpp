
#include "api.h"

int g_cnt = 0;

int plus(int a, int b)
{
    g_cnt++;
    return a + b;
}
