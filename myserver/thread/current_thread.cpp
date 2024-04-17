
#include "../include/current_thread.h"

namespace CurrentThread
{
    thread_local int t_cachedTid = 0;
    void cacheTid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));    // zzz system call
                        // pid_t 是 int 的别名
        }
    }
}
