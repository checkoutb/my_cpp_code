
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int main()
{
    int data_processed;
    int file_pipes[2];
    const char some_data[] = "123";
    char buffer[BUFSIZ + 1];
    pid_t fork_result;

    memset(buffer, '\0', sizeof(buffer));
    printf("\n\ntest : %ld\n\n\n", sizeof(buffer));

    if (pipe(file_pipes) == 0)
    {
        fork_result = fork();
        if (fork_result == -1)
        {
            fprintf(stderr, "fork fail");
            exit(EXIT_FAILURE);
        }
        if (fork_result == 0)
        {
            // child
            data_processed = read(file_pipes[0], buffer, BUFSIZ);
            printf("read %d byte: %s\n", data_processed, buffer);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // parent
            data_processed = write(file_pipes[1], some_data, strlen(some_data));
            printf("write %d bytes\n", data_processed);
        }
    }
    exit(EXIT_SUCCESS);
}

