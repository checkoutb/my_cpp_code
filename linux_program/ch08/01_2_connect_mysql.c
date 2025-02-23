

#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"

int main(int argc, char *argv[])
{
    // MYSQL *pconn;
    MYSQL conn;

    // pconn = mysql_init(NULL);
    mysql_init(&conn);
    // if (!pconn)
    // {
    //     fprintf(stderr, "mysql init failed\n");
    //     return EXIT_FAILURE;
    // }

    // pconn = mysql_real_connect(pconn, "localhost", "root", "1234", "down_txt", 0, NULL, 0);
    if (mysql_real_connect(&conn, "localhost", "rick", "i do not know", "foo", 0, NULL, 0))
    {
        printf("connect success\n");
        mysql_close(&conn);
    }
    else
    {
        fprintf(stderr, "conn fail\n");
        if (mysql_errno(&conn))
        {
            fprintf(stderr, "conn error %d: %s\n",
                mysql_errno(&conn), mysql_error(&conn));
        }
    }
    // if (pconn)
    // {
    //     printf("connect success\n");
    // }
    // else
    // {
    //     printf("connect fail\n");
    // }

    // mysql_close(pconn);

    return EXIT_SUCCESS;
}


