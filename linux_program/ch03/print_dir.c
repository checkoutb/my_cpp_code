#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
// #include <errno.h>

void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL)        // md, dp == opendir .....
    {
        // printf("..%d, %d \n", dp, NULL);
        // printf("%d", dp == NULL);

        perror("error desc");       // success...
        fprintf(stderr, "cannot open dir: %s\n", dir);
        return;
    }

    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            printf("%*s%s/\n", depth, " ", entry->d_name);

            printdir(entry->d_name, depth + 4);
        }
        else 
            printf("%*s%s\n", depth, " ", entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

int main()
{
    printf("dic scan\n");
    // printdir("/sdwf/qwer", 0);      // perror : no such file or dir
    // printdir("/boot", 0);
    printdir("/home", 0);       // ok, but tooooo much files....
    printf("done\n");

    exit(0);
}

