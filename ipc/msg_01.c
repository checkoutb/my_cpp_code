

#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> // fork


// https://zhuanlan.zhihu.com/p/672264623

// message queue

struct msg {
  long mtype;
  char mtext[32];
};



int msg_2() {
  int t2 = fork();
  if (t2 == 0) {
    // sub
    key_t key = ftok(".", 22);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    printf("sub %d %d\n", key, msgid);
    struct msg msg2;
    msg2.mtype = 432;
    strcpy(msg2.mtext, "hi from sub, mq");
    msgsnd(msgid, &msg2, sizeof(msg2), 0);
    printf("sent...\n");
  }
  else {
    //sleep(1);
    //printf("main %d\n", t2);
    key_t key = ftok(".", 22);
    //printf("main %d\n", key);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    printf("main %d %d\n", key, msgid);
    struct msg msg2;
    msgrcv(msgid, &msg2, sizeof(msg2), 432, 0); // msg2.mtype
    printf("got: %d %s\n", msg2.mtype, msg2.mtext);
    msgctl(msgid, IPC_RMID, NULL);
  }
  return 0;
}


int msg_1() {
  key_t key;
  int msgid;

  key = ftok(".", 65); // pathname, int

  printf("key: %d\n", key);
  printf("test: %d\n", ftok(".", 65)); // == key
  
  msgid = msgget(key, 0666 | IPC_CREAT);

  struct msg msg2;
  msg2.mtype = 1;
  strcpy(msg2.mtext, "hello message queue");

  msgsnd(msgid, &msg2, sizeof(msg2), 0); // 使用 sizeof(msg2.mtext) 也有的，试了可以用，但是 应该是 整个结构吧。 因为 msg_2 中 可以输出 mtype， 说明 mtype 也复制了。
  // 而且 mtype 就是 msgrcv 的 倒数第二个参数。

  msgrcv(msgid, &msg2, sizeof(msg2), 1, 0); // 1 == msg2.mtype

  printf("received: %s\n", msg2.mtext);

  msgctl(msgid, IPC_RMID, NULL);

  return 0;
}



int main() {
  //msg_1();

  msg_2();
}


