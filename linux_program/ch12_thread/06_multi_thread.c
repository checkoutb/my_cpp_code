
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 6

void *thread_fun(void *arg);

int main()
{
    int res;
    pthread_t a_thread[NUM_THREADS];
    void *thread_result;
    int lots_of_threads;

    for (lots_of_threads = 0; lots_of_threads < NUM_THREADS; ++lots_of_threads)
    {
        res = pthread_create(&(a_thread[lots_of_threads]), NULL, thread_fun, (void *)&lots_of_threads);
        if (res != 0)
        {
            perror("thread create fail");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
    printf("waiting thread finish\n");
    for (lots_of_threads = NUM_THREADS - 1; lots_of_threads >= 0; --lots_of_threads)
    {
        res = pthread_join(a_thread[lots_of_threads], &thread_result);
        if (res == 0)
        {
            printf("pick up a thread\n");
        }
        else
        {
            printf("pthread join fail\n");
        }
    }
    printf("all done\n");
    exit(EXIT_SUCCESS);
}

void *thread_fun(void *arg)
{
    int my_num = *(int *)arg;
    int rand_num;
    printf("thread fun is running. arg: %d\n", my_num);
    rand_num = 1 + (9.0 * rand()) / (RAND_MAX + 1.0);
    sleep(rand_num);
    printf("bye from %d\n", my_num);
    pthread_exit(NULL);
}
