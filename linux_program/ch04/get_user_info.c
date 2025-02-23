
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    uid_t uid;
    gid_t gid;

    struct passwd *pw;
    uid = getuid();
    gid = getgid();

    printf("user is %s\n", getlogin());

    pw = getpwuid(uid);

    printf("name=%s, uid=%d, gid=%d, home=%s, shell=%s\n", pw->pw_name, pw->pw_uid, pw->pw_gid, pw->pw_dir, pw->pw_shell);;

    exit(0);
}

