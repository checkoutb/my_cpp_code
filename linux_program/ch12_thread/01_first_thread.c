
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *thread_function(void *arg);

char message[] = "hi world";



int main()
{
    #ifdef _REENTRANT
    printf(" there is _reentrant\n");
    #endif

    int res;
    pthread_t a_thread;
    void *thread_result;

    res = pthread_create(&a_thread, NULL, thread_function, (void *) message);
    if (res != 0)
    {
        perror("thread create failed\n");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    printf("waiting thread finish\n");
    res = pthread_join(a_thread, &thread_result);
    if (res != 0)
    {
        perror("thread join failed\n");
        exit(EXIT_FAILURE);
    }
    printf("thread join finish, it return : %s\n", (char *) thread_result);
    printf("message now is %s\n", message);
    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
    printf("thread_function running, arg is %s\n", (char *)arg);
    sleep(2);
    strcpy(message, "bye");
    pthread_exit("thank you.  thread fun");
}

