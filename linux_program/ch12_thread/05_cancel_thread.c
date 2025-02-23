
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_fun(void *);

int main()
{
    int res;
    pthread_t a_thread;
    void *thread_result;

    res = pthread_create(&a_thread, NULL, thread_fun, NULL);
    if (res != 0)
    {
        perror("Thread create fail");
        exit(EXIT_FAILURE);
    }

    sleep(3);
    printf("to cancel thread\n");
    res = pthread_cancel(a_thread);
    if (res != 0)
    {
        perror("cancel fail");
        exit(EXIT_FAILURE);
    }
    printf("waiting thread finish\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0)
    {
        perror("thread join fail");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void *thread_fun(void *arg)
{
    int i, res;
    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (res != 0)
    {
        perror("set cancel state fail");
        exit(EXIT_FAILURE);
    }
    res = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (res != 0)
    {
        perror("set cancel type fail");
        exit(EXIT_FAILURE);
    }
    printf("thread fun is running\n");
    for (i = 0; i < 10; ++i)
    {
        printf("thread fun is still running %d ..\n", i);
        sleep(1);
    }
    pthread_exit(0);
}
