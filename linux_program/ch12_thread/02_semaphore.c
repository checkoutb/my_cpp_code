
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


void *thread_function(void *arg);
sem_t bin_sem;

// char message[] = "hi world";

#define WORK_SIZE 1024
char work_area[WORK_SIZE];

int main()
{
    #ifdef _REENTRANT
    printf(" there is _reentrant\n");
    #endif

    int res;
    pthread_t a_thread;
    void *thread_result;

    res = sem_init(&bin_sem, 0, 0);
    if (res != 0)
    {
        perror("semephore init failed\n");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    // 新thread已经开始了。如果res==0。
    if (res != 0)
    {
        perror("thread create failed\n");
        exit(EXIT_FAILURE);
    }
    // sleep(1);
    printf("intput text, 'end' to finish\n");

    while (strncmp("end", work_area, 3) != 0)
    {
        fgets(work_area, WORK_SIZE, stdin);
        sem_post(&bin_sem);
    }

    printf("wait thread end");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0)
    {
        perror("thread join failed\n");
        exit(EXIT_FAILURE);
    }
    printf("thread join finish, it return : %s\n", (char *) thread_result);
    // printf("message now is %s\n", message);
    sem_destroy(&bin_sem);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
    sem_wait(&bin_sem);
    while (strncmp("end", work_area, 3) != 0)
    {
        printf("you input %ld chars\n", strlen(work_area) - 1);
        sem_wait(&bin_sem);
    }
    pthread_exit("end of thread");
    // printf("thread_function running, arg is %s\n", (char *)arg);
    // sleep(2);
    // strcpy(message, "bye");
    // pthread_exit("thank you.  thread fun");
}

