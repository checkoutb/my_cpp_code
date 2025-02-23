
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char tmpnam2[L_tmpnam];
    char *filename;
    FILE *tmpfp;

    filename = tmpnam(tmpnam2);

    printf("tmp file name is: %s\n", filename);
    tmpfp = tmpfile();  // parameter is void
    if (tmpfp)
        printf("open a tmp file ok\n");
    else
        perror("tmpfile");
    exit(0);
}

