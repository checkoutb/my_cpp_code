
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    int file_desc;
    int save_errno;

    file_desc = open("/tmp/lock.text.lk", O_RDWR | O_CREAT | O_EXCL, 0444);

    if (file_desc == -1)
    {
        save_errno = errno;
        printf("failed : %d\n", save_errno);
    } 
    else 
    {
        printf("success\n");
    }
    exit(EXIT_SUCCESS);
}
