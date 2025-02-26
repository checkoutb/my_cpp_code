
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg);

char message[] = "hi world";
int thread_finished = 0;

int main()
{
    int res;
    pthread_t a_thread;

    pthread_attr_t thread_attr;

    res = pthread_attr_init(&thread_attr);

    if (res != 0)
    {
        perror("init attr failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (res != 0)
    {
        perror("set detach state failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&a_thread, &thread_attr, thread_function, (void *) message);
    if (res != 0)
    {
        perror("create thread failed");
        exit(EXIT_FAILURE);
    }

    (void) pthread_attr_destroy(&thread_attr);

    while (!thread_finished) {
        printf("wait thread end\n");
        sleep(1);
    }
    printf("main end, bye\n");
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
   printf("thread is running. arg: %s\n", (char *) arg);
   sleep(4);
   printf("thread fcuntion end\n");
   thread_finished = 1;
   pthread_exit(NULL);
}

