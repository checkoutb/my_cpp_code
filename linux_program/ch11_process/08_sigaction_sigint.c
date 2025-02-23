
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ouch(int sig)
{
    printf("got signal: %d\n", sig);
    // (void) signal(SIGINT, SIG_DFL);
}

// must use ctrl+\ to end it.

int main()
{
    struct sigaction act;
    act.sa_handler = ouch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);

    // (void) signal(SIGINT, ouch);

    while (1)
    {
        printf("hi\n");
        sleep(1);
    }
}
