
#include "black_box.h"

void crash(int *i)
{
    *i = 1;
}

void f(int *i)
{
    int *j = i;
    // j = 
    sophisticated(j);
    j = complicated(j);     // 。。都是复杂的 的意思。。
    crash(j);
}

int main()
{
    int i;
    f(&i);
}


// ... 不给看 black_box.h 的内容啊。。
// 视频中说 对空指针 反应用。。 所以 是 j 被赋予了 nullptr ？

// 是的， github搜到了。  https://github.com/C0DER11101/QuickGDB    内容差不多，但是文件名之类的不同。


/*


Program received signal SIGSEGV, Segmentation fault.
0x0000000000401136 in crash (i=0x0) at 02_example.cpp:6
warning: Source file is more recent than executable.
6	    *i = 1;











*/

