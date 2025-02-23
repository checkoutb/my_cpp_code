
#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"

int main(int agrc, char *argv[])
{
    MYSQL conn;
    int res;

    mysql_init(&conn);
    if (mysql_real_connect(&conn, "localhost", "rick", "secretpwd", "foo", 0, NULL, 0))
// if (mysql_real_connect(&conn, "localhost", "rick", "secretpwd", "foo", 0, NULL, CLIENT_FOUND_ROWS))
    {
        printf("conn success\n");
        
        res = mysql_query(&conn, "insert into children(fname, age) values ('Ann', 3)");
// res = mysql_query(&conn, "update children set age=4 where fname='Ann'");

        if (!res)
        {
            printf("inserted %lu rows\n", (unsigned long) mysql_affected_rows(&conn));
        }
        else
        {
            fprintf(stderr, "insert error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
        }
        mysql_close(&conn);
    }
    else
    {
        fprintf(stderr, "conn fail\n");
        if (mysql_errno(&conn))
        {
            fprintf(stderr, "conn error %d : %s\n", mysql_errno(&conn), mysql_error(&conn));
        }
    }
    return EXIT_SUCCESS
}

