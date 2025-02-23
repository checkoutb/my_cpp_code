
#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"


MYSQL conn;
MYSQL_RES *req_ptr;
MYSQL_ROW sqlrow;

void display_row();         // display

int main(int agrc, char *argv[])
{
    int res;

    mysql_init(&conn);
    if (mysql_real_connect(&conn, "localhost", "rick", "secretpwd", "foo", 0, NULL, 0))
    {
        printf("conn success\n");
        
        res = mysql_query(&conn, "select childno, fname, age from children where age > 5");

        if (!res)
        {
            // printf("inserted %lu rows\n", (unsigned long) mysql_affected_rows(&conn));

            // one select all row
            // res_ptr = mysql_store_result(&conn);
            // if (res_ptr)
            // {
            //     printf("retrieve %lu rows\n", (unsigned long) mysql_num_rows(res_ptr));
            //     while ((sqlrow = mysql_fetch_row(res_ptr)))
            //     {
            //         printf("fetched data...\n");
            //     }
                // end


            // one select one row
            res_ptr = mysql_use_result(&conn);
            if (res_ptr)
            {
                whlie ((sqlrow = mysql_fetch_row(res_ptr)))
                {
                    printf("fetched data...\n");
                    
                    display_row();         // display
                }
                // end



                if (mysql_errno(&conn))
                {
                    fprintf(stderr, "retrieve error %s\n", mysql_error(&conn));
                }
                mysql_free_result(res_ptr);
            }
        }
        else
        {
            fprintf(stderr, "select error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
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

// gcc -I/usr/include/mysql 04_select.c -L/usr/lib/mysql -lmysqlclient -o select

void display_row()
{
    unsigned int field_count;

    field_count = 0;
    while (field_count < mysql_field_count(&conn))
    {
        printf("%s ", sqlrow[field_count]);
        ++field_count;
    }
    printf("\n");
}

