#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/shm.h>

#include "shm_com.h"

int main()
{
    int running = 1;
    void *shared_memory = (void *) 0;
    struct shared_use_st *shared_stuff;
    char buffer[BUFSIZ];
    int shmid;

    shmid = shmget((key_t) 1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    if (shmid == -1)
    {
        fprintf(stderr, "shmget fail\n");
        exit(EXIT_FAILURE);
    }
    shared_memory = shmat(shmid, (void *) 0, 0);
    if (shared_memory == (void *) -1)
    {
        fprintf(stderr, "shmat fail");
        exit(EXIT_FAILURE);
    }
    printf("memory attach at %p", shared_memory);

    shared_stuff = (struct shared_use_st *) shared_memory;
    while (running)
    {
        while (shared_stuff->written_by_you == 1)
        {
            sleep(1);
            printf("waiting for client \n");
        }
        printf("enter some text: ");
        fgets(buffer, BUFSIZ, stdin);

        strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
        shared_stuff->written_by_you = 1;

        if (strncmp(buffer, "end", 3) == 0)
        {
            running = 0;
        }
    }
    if (shmdt(shared_memory) == -1)
    {
        fprintf(stderr, "shmdt fail");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
