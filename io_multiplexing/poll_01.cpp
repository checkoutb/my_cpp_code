
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int const PORT = 11223;

// `gcc/g++ poll_01.cpp`   `./a.out`

int main() {

    int pid = fork();

    if (pid == 0) {
        // child client
        int req_sz = 10;
        for (int i = 0; i < req_sz; ++i) {
            sleep(2);

            printf("client start...\n");
            
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);

            if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) <= 0) {
                perror(" client failed to inet_pton");
                close(fd);
                exit(EXIT_FAILURE);
            }

            if (connect(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
                perror(" client failed to connect");
                close(fd);
                exit(EXIT_FAILURE);
            }

            char buf[100] = "HI_poll__________";
            buf[10] = 'A' + (i % 26);
            if (i == req_sz - 1) {
                buf[0] = 'Z';
            }

            send(fd, buf, strlen(buf), 0);
            printf("client sent!");
            int res = recv(fd, buf, sizeof(buf), 0);
            if (res == -1) {
                perror("  client failed to recv data from server");
            } else {
                buf[res] = '\0';
                printf(">>>>>>>> client got: %d, %s\n", res, buf);
            }

            shutdown(fd, SHUT_RDWR);
            close(fd);
        }
    } else {
        // parent server

        printf("server starting...\n");
        
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            perror("server socket error");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in addr;
        // memset(&addr, 0, sizeof(addr));   // need?
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(PORT);

        if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
            perror("server failed bind");
            close(fd);
            exit(EXIT_FAILURE);
        }

        if (listen(fd, 5) == -1) {
            perror("server listen failed");
            close(fd);
            exit(EXIT_FAILURE);
        }

        printf("server is listening...\n");
        
        // int mxfd = fd;
        socklen_t slen = sizeof(addr);
        // fd_set fds;
        // struct timeval tv;

        // int arr[100] = {0};
        // arr[fd] = 1;
        int res = 0;
        char buf[100] = {'\0'};

        struct pollfd pfds[20];

        // 不行，最后还是 arr记录fd，然后每次poll前设置一遍 pollfds
        int pfds_idx = fd;    // valid element's max index
        pfds[fd].fd = fd;
        pfds[fd].events = POLLIN;  // [fd] 浪费012，但是不需要自己跟踪 pollfd 是否有效。

        for (int i = 0; i < fd; ++i) {
            pfds[i].fd = fd;
            pfds[i].events = POLLIN | POLLOUT;
        }

        int arr[100] = {0};
        arr[fd] = 1;
        int mxfd = fd;
        while (true) {
            // tv.tv_sec = 10;
            // tv.tv_usec = 0;
            // FD_ZERO(&fds);            
            // for (int i = 3; i <= mxfd; ++i) {
            //     if (arr[i]) {
            //         FD_SET(i, &fds);
            //     }
            // }

            pfds_idx = 0;
            for (int i = 0; i <= mxfd; ++i) {
                if (arr[i]) {
                    // printf("fd: %d\n", i);
                    pfds[pfds_idx].fd = i;
                    pfds[pfds_idx].events = POLLIN;
                    pfds[pfds_idx].revents = 0;
                    ++pfds_idx;
                }
            }

            // sleep(1);
            // printf("ready to poll\n");
            fflush(stdout);
            res = poll(pfds, pfds_idx, 5000);  // 5000ms

            
            
            if (res == -1) {
                perror("poll failed");
            } else if (res == 0) {
                perror("timeout. quit server");
                break;
            } else {
                // printf("server has data to process\n");
                for (int i = 0; i < pfds_idx; ++i) {

                    // printf("%d, %d\n", pfds[i].fd, pfds[i].revents);
                    
                    if (pfds[i].revents & POLLIN) {
                        printf(" %d fd has data\n", pfds[i].fd);
                        if (pfds[i].fd == fd) {
                            int new_fd = accept(fd, (struct sockaddr*) &addr, &slen);
                            if (new_fd < 0) {
                                perror("server accept failed");
                            } else {
                                printf("new connection %d", new_fd);
                            }
                            arr[new_fd] = 1;
                            if (new_fd > mxfd) {
                                mxfd = new_fd;
                            }
                            // pfds[new_fd].fd = new_fd;
                            // pfds[new_fd].events = POLLIN;
                            // if (new_fd > pfds_idx) {
                            //     pfds_idx = new_fd;
                            // }
                            continue;
                        }
                        
                        char buf[100];
                        int len = recv(pfds[i].fd, buf, sizeof(buf), 0);
                        if (len < 0) {
                            perror(" server recv failed");
                        } else if (len == 0) {
                            printf("client close connection, set %d 0\n", i);
                            // pfds[i].fd = -1;
                            arr[pfds[i].fd] = 0;
                            close(pfds[i].fd);
                        } else {
                            buf[len] = '\0';
                            printf("server got: %d, %s\n", len, buf);

                            if (buf[0] == 'Z') {
                                printf(" ------------ no more data ---------\n");
                            }
                            
                            strcpy(buf, "QQQ");
                            send(pfds[i].fd, buf, strlen(buf), 0);
                        }
                    }
                }
            }
        }
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

}


