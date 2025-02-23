
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

const char *lock_file = "/tmp/lock.test2.lk";

int main()
{
    int f_desc;
    int tries = 10;

    while (tries--)
    {
        f_desc = open(lock_file, O_RDWR | O_CREAT | O_EXCL, 0444);
        if (f_desc == -1)
        {
            printf("%d - lock already present\n", getpid());
            sleep(3);
        }
        else
        {
            // 临界区开始
            printf("%d - have exclusive access\n", getpid());
            sleep(1);
            (void) close(f_desc);
            (void) unlink(lock_file);
            // 临界区结束
            sleep(2);
        }
    }
    exit(EXIT_SUCCESS);
}