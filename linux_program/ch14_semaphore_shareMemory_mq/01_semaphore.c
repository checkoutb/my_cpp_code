#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/sem.h>
#include "semun.h"

static int set_semvalue(void);
static void del_semvalue(void);
static int semaphore_p(void);
static int semaphore_v(void);

static int sem_id;

int main(int argc, char *argv)
{
    int i;
    int pause_time;
    char op_char = 'O';

    srand((unsigned int) getpid());
    sem_id = semget((key_t) 1234, 1, 0666 | IPC_CREAT);

    if (argc > 1)
    {
        if (!set_semvalue()) {
            fprintf(stderr, "failed to init semaphore\n");
            exit(EXIT_FAILURE);
        }
        op_char = 'x';
        sleep(2);
    }
    for (i = 0; i < 10; ++i)
    {
        // 调用semaphore_p函数，它在程序将进入临界区域时设置信号量以等待进入
        if (!semaphore_p())
            exit(EXIT_FAILURE);
        printf("%c", op_char);
        fflush(stdout);

        pause_time = rand() % 3;
        sleep(pause_time);
        printf("%c", op_char);
        fflush(stdout);

        // 临界区域之后，调用semaphore_v来将信号量设置为可用，然后等待一段随机的时间，再进入下一次循环。
        if (!semaphore_v())
            exit(EXIT_FAILURE);
        
        pause_time = rand() % 2;
        sleep(pause_time);
    }

    printf("\n%d - finished\n", getpid());

    if (argc > 1)
    {
        sleep(10);
        del_semvalue();
    }
    exit(EXIT_SUCCESS);
}

// 函数set_semvalue通过将semctl调用的command参数设置为SETVAL来初始化信号量。在使用信号量之前必须要这样做
static int set_semvalue(void)
{
    union semun sem_union;

    sem_union.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return (1);
}

// 函数del_semvalue的形式与上面的函数几
// 乎一样，只不过它通过将semctl调用的command
// 设置为IPC_RMID来删除信号量ID
static void del_semvalue(void)
{
    union semun sem_un;
    if (semctl(sem_id, 0, IPC_RMID, sem_un) == -1)
        fprintf(stderr, "fail to delete semaphore\n");
}

// semaphore_p对信号量做减1操作（等待）
static int semaphore_p(void)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = -1;      // P()
    sem_b.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_p fail\n");
        return (0);
    }
    return (1);
}

// semaphore_v和semaphore_p类似，不
// 同的是它将sembuf结构中的sem_op设置为1。这
// 是一个“释放”操作，它使信号量变为可用
static int semaphore_v(void)
{
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = 1;       // V()
    sem_b.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_v fail\n");
        return (0);
    }
    return (1);
}


