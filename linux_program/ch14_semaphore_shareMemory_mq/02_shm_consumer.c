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
    int shmid;

    srand((unsigned int) getpid());

    // 创建共享内存段
    shmid = shmget((key_t) 1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    if (shmid == -1)
    {
        fprintf(stderr, "shmget fail\n");
        exit(EXIT_FAILURE);
    }

    // 让程序可以访问这个共享内存
    shared_memory = shmat(shmid, (void *) 0, 0);
    if (shared_memory == (void *) -1)
    {
        fprintf(stderr, "shmat fail\n");
        exit(EXIT_FAILURE);
    }

    // printf("memory attached at %X\n", (int) (shared_memory));
    printf("memory attached at %p\n", shared_memory);

// 循环将一直执行到在written_by_you中找到
// end字符串为止。sleep调用强迫消费者程序在临界
// 区域多待一会儿，让生产者程序等待
    shared_stuff = (struct shared_use_st *) shared_memory;
    shared_stuff->written_by_you = 0;
    while (running)
    {
        if (shared_stuff->written_by_you)
        {
            printf("you wrote: %s", shared_stuff->some_text);
            sleep(rand() % 4);
            shared_stuff->written_by_you = 0;
            if (strncmp(shared_stuff->some_text, "end", 3) == 0)
            {
                running = 0;
            }
        }
    }

    // 共享内存被分离
    if (shmdt(shared_memory) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    // 共享内存被删
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("shmctl IPC_RMID fail");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
