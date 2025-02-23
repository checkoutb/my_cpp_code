#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *arg);
pthread_mutex_t mtx;

#define WORK_SIZE 1024
char work_area[WORK_SIZE];
int time_to_exit = 0;

int main()
{
    int res;
    pthread_t a_thread;
    void *thread_result;
    res = pthread_mutex_init(&mtx, NULL);
    if (res != 0)
    {
        perror("mutex init fail");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&a_thread, NULL, thread_function, NULL);
    if (res != 0)
    {
        perror("thread create fail");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&mtx);
    printf("input some text, 'end' to quit\n");
    while (!time_to_exit)
    {
        fgets(work_area, WORK_SIZE, stdin);
        pthread_mutex_unlock(&mtx);
        while (1)
        {
            pthread_mutex_lock(&mtx);
            if (work_area[0] != '\0')
            {
                pthread_mutex_unlock(&mtx);
                sleep(1);
            }
            else
            {
                break;
            }
        }
    }
    pthread_mutex_unlock(&mtx);
    printf("\n wait thread finish\n");
    res = pthread_join(a_thread, &thread_result);

    if (res != 0)
    {
        perror("thread join fail");
        exit(EXIT_FAILURE);
    }
    printf("thread join success\n");
    pthread_mutex_destroy(&mtx);
}

void *thread_function(void *arg)
{
    sleep(1);
    pthread_mutex_lock(&mtx);
    while (strncmp("end", work_area, 3) != 0)
    {
        printf("you input %ld chars\n", strlen(work_area) - 1);
        work_area[0] = '\0';
        pthread_mutex_unlock(&mtx);
        while (work_area[0] == '\0')
        {
            pthread_mutex_unlock(&mtx);
            sleep(1);
            pthread_mutex_lock(&mtx);
        }
    }
    time_to_exit = 1;
    work_area[0] = '\0';
    pthread_mutex_unlock(&mtx);
    pthread_exit(0);
}
