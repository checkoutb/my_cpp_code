

#include <sys/socket.h>

#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>  // sockaddr_in
#include <stdlib.h>

// 应该缺少错误处理

// setsockopt

// `gcc/g++ select_01.cpp`    `./a.out`


const int PORT = 13579;

int main() {

    int pid = fork();

    if (pid == 0) {  // child  clien
        int mxi = 10;
        for (size_t i = 0; i < mxi; ++i) {
            
            sleep(1);

            int fd = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in addr;  // target
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);

            if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
                perror("client failed to inet_pton");
                close(fd);
                exit(EXIT_FAILURE);
            }

            if (connect(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
                perror("client failed to connected");
                close(fd);
                exit(EXIT_FAILURE);
            }

            char buf[100] = "hi__________\0";
            buf[5] = 'A' + (i % 26);

            if (i == mxi - 1) {
                buf[0] = 'Z';
            }
            
            write(fd, buf, strlen(buf));
            printf("              client sent\n");

            int res = recv(fd, buf, sizeof(buf), 0);
            if (res == -1) {
                perror("  cccccccccccccc client recv failed");
            } else {
                buf[res] = '\0';
                printf(" client recv from server: %d, %s\n", res, buf);
            }

            shutdown(fd, SHUT_RDWR);  // 关闭 双向数据传输
            close(fd);
            
        }
        printf(" client end ...\n");
    } else {  // parent    server
        printf(" server start...\n");

        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            perror("server socket error");
            return 1;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(PORT);


        if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
            perror("server bind error");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (listen(fd, 5) == -1) {
            perror("server listen failed");
            close(fd);
            exit(EXIT_FAILURE);
        }
        printf(" ---- server is ready on PORT\n");


        int mxfd = fd;
        socklen_t slen = sizeof(addr);
        fd_set fds;
        struct timeval tv;

        int arr[100] = {0};
        arr[fd] = 1;
        int res = 0;
        char buf[100] = {'\0'};
        int mxi = 60;
        for (int i = 0; i < mxi; ++i) {
            tv.tv_sec = 10;  // 这个也要放里面，会被 select 改写的。
            tv.tv_usec = 0;
            FD_ZERO(&fds);
            for (int i = 3; i <= mxfd; ++i) {
                if (arr[i]) {
                    // printf(" set fd: %d to fds\n", i);
                    // perror(" asdasd...");
                    FD_SET(i, &fds);
                }
            }
            int res = select(mxfd + 1, &fds, NULL, NULL, &tv);
            if (res == -1) {
                perror("select failed");
            } else if (res) {
                printf("data is aviable\n");

                for (int i = 3; i <= mxfd; ++i) {   // 好像确实只能遍历啊
                    if (arr[i] == 0)
                        continue;
                    
                    if (FD_ISSET(i, &fds)) {
                        if (i == fd) {   // accept
                            int new_fd = accept(fd, (struct sockaddr*) &addr, &slen);
                            if (new_fd < 0) {
                                perror("--------- accept failed");
                            } else {
                                printf(" NNNNNNNNNNew connect\n");
                                arr[new_fd] = 1;
                                if (new_fd > mxfd) {
                                    mxfd = new_fd;
                                }
                            }
                        } else {
                            // data
                            buf[0] = '\0';
                            res = recv(i, buf, 99, 0);
                            if (res == -1) {
                                perror(" --------- recv error");
                            } else if (res == 0) {
                                printf("   client closed... %d \n", i);
                                arr[i] = 0;
                                //shutdown(i, SHUT_RDWR);
                                close(i);
                            } else {
                                buf[res] = '\0';   // 必须的，因为 增加 server to client后，这里 多了` server`。。
                                printf("-->>>>>>>  recv %d  got : %d, :: %s\n", i, res, buf);
                                if (buf[0] == 'Z') {
                                    // mxi = i;  // break for
                                    printf(" ________data never come------\n");
                                }

                                // server to client.
                                strcpy(buf, " AA msg from server");
                                res = send(i, buf, strlen(buf), 0);
                                if (res == -1) {
                                    perror(" server send___ failed..");
                                }
                            }
                        }
                    }
                }
            } else {
                printf("no data in 10secs, quit server\n");
                break;  // no data, client has closed.
            }
        }
        shutdown(fd, SHUT_RDWR);
        close(fd);
        

        // ok
        // // accept client's connect request
        // int fd_c;
        // socklen_t slen = sizeof(addr);
        // if ((fd_c = accept(fd, (struct sockaddr*) &addr, &slen)) < 0) {
        //     perror("accept failed");
        //     exit(EXIT_FAILURE);
        // }
        // fd_set rfds; // I need read
        // FD_ZERO(&rfds);
        // FD_SET(fd_c, &rfds);
        // struct timeval tv;
        // tv.tv_sec = 10;
        // tv.tv_usec = 10;
        // int res = select(fd_c + 1, &rfds, NULL, NULL, &tv);
        // if (res == -1) {
        //     perror("select failed");
        // } else if (res) {
        //     printf("data is available\n");
        //     char buf[100] = {'\0'};
        //     res = read(fd_c, buf, 99);
        //     printf("got: %d: %s\n", res, buf);
        // } else {
        //     printf("no data in 10 sec\n");
        // }
        // close(fd_c);
        // close(fd);
        // printf(" server end...\n");
    }
    
}



// file 总是 可读可写，所以 select无法阻塞
int main__() {

    FILE* fp = fopen("empty.ignore", "r");
    if (fp == NULL) {
        perror("fopen failed\n");
        return 1;
    }

    int fd = fileno(fp);
    printf("I got fd: %d\n", fd);

    int pid = fork();

    if (pid == 0) {  // child

        sleep(2);

        printf("child starting...\n");
        
        FILE* fp2 = fopen("empty.ignore", "w");
        if (fp2 == NULL) {
            perror("fopen failed in child process\n");
            return 1;     // 没有 fclose(fp)
        }

        int fd2 = fileno(fp);
        printf("Child got fd: %d\n", fd2);

        char buf[100] = "ni hao.";
        
        fwrite(buf, strlen(buf), 1, fp2);

        fclose(fp2);
        fclose(fp);

        printf("end of child\n");
        
    } else {    // parent

        fd_set rfd;  // read fd
        FD_ZERO(&rfd);
        FD_SET(fd, &rfd);

        struct timeval tv; // timeout
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        printf("...to select()\n");
        char buf2[100] = {'\0'};
        int rdcnt2 = read(fd, buf2, 100);
        printf("    %d, %s\n\n", rdcnt2, buf2);
        
        
        // int res = select(fd + 1, &rfd, NULL, NULL, &tv);
        int res = select(fd + 1, &rfd, NULL, NULL, NULL); // 监听是否可读
        // int res = select(fd + 1, NULL, &rfd, NULL, &tv); // 监听是否可写
        if (res == -1) {
            perror("select() failed");
        } else if (res) {
            printf("data is available.\n");
            char buf[100];
            int rdcnt = read(fd, buf, 100);
            printf("i read %d.\n", rdcnt);
            printf("content: %s\n", buf);
        } else {
            printf("No data in 10secs\n");
        }

        fclose(fp);

        printf("end of parent\n");
    }

    return 0;
}


