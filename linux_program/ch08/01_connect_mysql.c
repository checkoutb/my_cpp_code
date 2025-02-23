
#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"

int main(int argc, char *argv[])
{
    MYSQL *pconn;

    pconn = mysql_init(NULL);
    if (!pconn)
    {
        fprintf(stderr, "mysql init failed\n");
        return EXIT_FAILURE;
    }

    pconn = mysql_real_connect(pconn, "localhost", "root", "1234", "down_txt", 0, NULL, 0);
    
    if (pconn)
    {
        printf("connect success\n");
    }
    else
    {
        printf("connect fail\n");
    }

    mysql_close(pconn);

    return EXIT_SUCCESS;
}

