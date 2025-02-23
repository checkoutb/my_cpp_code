
#include <sys/utsname.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char computer[256];
    struct utsname uts;

    if (gethostname(computer, 255) != 0 || uname(&uts) < 0)
    {
        fprintf(stderr, "cannot get host info");
        exit(1);
    }

    printf("computer hostname is %s\n", computer);
    printf("system is %s on %s hardware\n", uts.sysname, uts.machine);
    printf("nodename is %s\n", uts.nodename);
    printf("version is %s, %s\n", uts.release, uts.version);

    printf("%ld", gethostid());
    exit(0);
}
