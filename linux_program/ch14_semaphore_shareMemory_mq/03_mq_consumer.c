
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/msg.h>

struct my_msg_st
{
    long int my_msg_type;
    char some_text[BUFSIZ];
};

int main()
{
    int running = 1;
    int msgid;
    struct my_msg_st some_data;
    long int msg_to_receive = 0;

    // 建立消息队列
    msgid = msgget((key_t) 1234, 0666 | IPC_CREAT);
    
    if (msgid == -1)
    {
        fprintf(stderr, "msgget failed, errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    // 从队列中获取消息
    while (running)
    {
        if (msgrcv(msgid, (void *) &some_data, BUFSIZ, msg_to_receive, 0) == -1)
        {
            fprintf(stderr, "msgrcv failed, errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        printf("you write %s", some_data.some_text);
        if (strncmp(some_data.some_text, "end", 3) == 0)
        {
            running = 0;
        }
    }

    // 删除消息队列
    if (msgctl(msgid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl IPC_RMID failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
