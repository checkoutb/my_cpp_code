
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *filename;

    if (argc != 2)
    {
        fprintf(stderr, "use upper file\n");
        exit(1);
    }

    filename = argv[1];

    if (!freopen(filename, "r", stdin))
    {
        fprintf(stderr, "could not redirect stdin from file %s\n", filename);
        exit(2);
    }
    execl("./upper", "upper", 0);

    // 如果没有错误，下面的不会执行。
    perror("could not exec ./upper\n");
    exit(3);


    // 把下面的编译成 upper， 让上面的调用。
    // int ch;
    // while ((ch = getchar()) != EOF) // ctrl+d 退出
    // {
    //     putchar(toupper(ch));
    // }
    // exit(0);
}
