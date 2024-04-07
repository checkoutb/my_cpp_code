
#include "../include/current_thread.h"

namespace CurrentThread
{
    thread_local int t_cachedTid = 0;
    void cacheTid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));        // zzz system call
                                                // 为什么转成 pid_t 然后 赋给 int ？
        }
    }
}
