#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_NAME "/tmp/my_fifo2"
#define BUFFER_SIZE PIPE_BUF

int main()
{
    int pipe_fd;
    int res;
    int open_mode = O_RDONLY;
    char buffer[BUFFER_SIZE + 1];
    int bytes_read = 0;

    memset(buffer, '\0', sizeof(buffer));

    printf("process %d opening fifo O_RDONLY\n", getpid());
    pipe_fd = open(FIFO_NAME, open_mode);
    printf("process %d result %d\n", getpid(), pipe_fd);

    if (pipe_fd != -1)
    {
        do {
            // pipe_fd 是阻塞管道
            res = read(pipe_fd, buffer, BUFFER_SIZE);
            bytes_read += res;
        } while (res > 0);
        (void) close(pipe_fd);
    }
    else
    {
        exit(EXIT_FAILURE);
    }
    printf("process %d finished, read %d bytes\n", getpid(), bytes_read);
    exit(EXIT_SUCCESS);
}

