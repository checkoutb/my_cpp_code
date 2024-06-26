#pragma once

#include <unistd.h>
#include <sys/syscall.h>



namespace CurrentThread
{
    extern thread_local int t_cachedTid;        // 在 current_thread.cpp 中定义。
    void cacheTid();
    inline int tid()    // inline函数必须在调用该函数的每个文本文件中定义，所以放.h。 不建议声明和定义分离
    {
        // 用于向编译器提供关于某个条件表达式预期结果的提示
        // 只有第一次是 t_cachedTid==0 的(即1)， 之后全是0, 所以预期结果是0
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}
