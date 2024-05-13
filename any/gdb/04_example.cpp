
#include <iostream>

#include "black_box.h"

void small(int &a)
{
    a /= 68;
}

int bar(int &p)
{
    p *= 3;
    return unknown(p);
}

void oof(int &n)
{
    n *= n - 20;
    n = n - bar(n);
}

void foo(int &z)
{
    z = bar(z);
    oof(z);
    unknown(z);
    small(z);
    ++z;
}

int main()
{
    int x = 10;
    foo(x);

    if (x == 0)
    {
        std::cerr<<"error: x is 0"<<std::endl;
        return 3;
    }
}


// .. 这个 unknown 真没有。。 推导了下， 赋值 -100, 可以了。

