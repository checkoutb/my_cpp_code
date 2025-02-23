
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("run ps with execlp\n");
    
    execlp("ps", "ps", "ax", 0);

    printf("done\n");       // won't print, for process has been replaced
    exit(0);
}

// char *const ps_argv[] = {"ps", "ax", 0};
// char *const ps_envp[] = {"PATH=/bin:/usr/bin", "TERM=console", 0};

// execl("/bin/ps", "ps", "ax", 0);
// execlp("ps", "ps", "ax", 0);
// execle("/bin/ps", "ps", "ax", 0, ps_envp);

// execv("/bin/ps", ps_argv);
// execvp("ps", ps_argc);
// execve("/bin/ps", ps_argv, ps_envp);