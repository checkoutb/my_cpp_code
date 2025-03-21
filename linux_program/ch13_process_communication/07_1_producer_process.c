#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int main()
{
    int data_processed;
    int file_pipes[2];
    const char some_data[] = "asd123cvb";
    char buffer[BUFSIZ + 1];
    pid_t fork_result;

    memset(buffer, '\0', sizeof(buffer));
    
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
            sprintf(buffer, "%d", file_pipes[0]);
            (void) execl("07_2", "07_2", buffer, (char *) 0);
            exit(EXIT_FAILURE);
        }
        else
        {
            data_processed = write(file_pipes[1], some_data, strlen(some_data));
            printf("%d - wrote %d bytes\n", getpid(), data_processed);
        }
    }
    exit(EXIT_SUCCESS);
}
