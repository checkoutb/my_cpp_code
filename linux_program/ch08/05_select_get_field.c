
#include <stdlib.h>
#include <stdio.h>

#include "mysql.h"


MYSQL conn;
MYSQL_RES *req_ptr;
MYSQL_ROW sqlrow;

void display_header();
void display_row();         // display

int main(int agrc, char *argv[])
{
    int res;
    int first_row = 1;

    mysql_init(&conn);
    if (mysql_real_connect(&conn, "localhost", "rick", "secretpwd", "foo", 0, NULL, 0))
    {
        printf("conn success\n");
        
        res = mysql_query(&conn, "select childno, fname, age from children where age > 5");

        if (!res)
        {
            // one select one row
            res_ptr = mysql_use_result(&conn);
            if (res_ptr)
            {
                whlie ((sqlrow = mysql_fetch_row(res_ptr)))
                {
                    if (first_row)
                    {
                        first_row = 0;
                        display_header();
                    }
                    display_row();         // display
                }

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
// 上面是 04 的， 本文件 05 需要吗？

void display_row()
{
    unsigned int field_count;

    field_count = 0;
    while (field_count < mysql_field_count(&conn))
    {
        if (sqlrow[field_count])
            printf("%s ", sqlrow[field_count]);
        else
            printf("NULL");
        ++field_count;
    }
    printf("\n");
}

void display_header()
{
    MYSQL_FIELD *field_ptr;
    printf("column detail:\n");

    while ((field_ptr = mysql_fetch_field(res_ptr)) != NULL)
    {
        printf("\t Name: %s\n", field_ptr->name);
        printf("\t Type: ");
        if (IS_NUM(field_ptr->type))
        {
            printf("Numeric field\n");
        }
        else
        {
            switch (field_ptr->type)
            {
            case FIELD_TYPE_VAR_STRING:
                printf("varchar\n");
                break;
            case FIELD_TYPE_LONG:
                printf("long\n");
                break;
            default:
                printf("type is %d, check in mysql_com.h\n", field_ptr->type);
                break;
            }
        }
        printf("\t max witdh %ld\n", field_ptr->length);
        if (field_ptr->flags & AUTO_INCREMENT_FLAG)
        {
            printf("\t auto inc\n");
        }
        printf("\n");
    }
}
