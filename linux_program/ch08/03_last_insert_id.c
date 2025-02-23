
#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"

int main(int agrc, char *argv[])
{
    MYSQL conn;
    MYSQL_RES *req_ptr;
    MYSQL_ROW sqlrow;
    int res;

    mysql_init(&conn);
    if (mysql_real_connect(&conn, "localhost", "rick", "secretpwd", "foo", 0, NULL, 0))
    {
        printf("conn success\n");
        
        res = mysql_query(&conn, "insert into children(fname, age) values ('Ann', 3)");

        if (!res)
        {
            printf("inserted %lu rows\n", (unsigned long) mysql_affected_rows(&conn));
        }
        else
        {
            fprintf(stderr, "insert error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
        }

        // ----------------start------------------
        res = mysql_query(&conn, "select last_insert_id()");
        if (res)
        {
            printf("select error: %s\n", mysql_error(&conn));
        }
        else
        {
            res_ptr = mysql_use_result(&conn);
            if (res_ptr)
            {
                while ((sqlrow = mysql_fetch_row(res_ptr)))
                {
                    printf("we inserted childno: %s\n", sqlrow[0]);
                }
                mysql_free_result(res_ptr);
            }
        }
        // -----------------end-------------------

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

