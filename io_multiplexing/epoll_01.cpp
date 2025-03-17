


#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <arpa/inet.h>


// `gcc select_01.cpp`  `./a.out`

static int const PORT = 11335;

int main() {

    int pid = fork();
    int res;
    if (pid == 0) {
        // child client
        int mxreq = 10;
        for (int i = 0; i < mxreq; ++i) {

            sleep(2);
            
            int skfd = socket(AF_INET, SOCK_STREAM, 0);
            // if skfd == -1

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);

            if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) <= 0) {
                // error
            }

            if (connect(skfd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
                // error
            }

            char buf[30] = "ePo11_________";

            buf[10] = 'A' + (i % 26);
            if (i + 1 == mxreq) {
                buf[0] = 'Z';
            }
            send(skfd, buf, strlen(buf), 0);

            res = recv(skfd, buf, 30, 0);
            // if res == -1
            buf[res] = '\0';
            printf("  >>>>>>>  client got: %d, %s\n", res, buf);
            shutdown(skfd, SHUT_RDWR);
            close(skfd);
        }
    } else {
        // parent server
        int skfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        // memset
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(PORT);

        socklen_t addrlen = sizeof(addr);
        
        bind(skfd, (struct sockaddr*) &addr, sizeof(addr));
        listen(skfd, 5);

        printf("server is listenint...\n");

        // epoll ...  from  `man epoll`
        int const MX_EV = 10;
        struct epoll_event ev, events[MX_EV];
        int listen_sock, conn_sock, nfds, epollfd;
        
        epollfd = epoll_create1(0);
        // if (epollfd == -1)

        ev.events = EPOLLIN;
        ev.data.fd = skfd;   // listen_sock
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, skfd, &ev) == -1) {
            // error
        }

        for (int i = 0, mxi = 40; i < mxi; ++i) {

            nfds = epoll_wait(epollfd, events, MX_EV, 5000); // timeout 5000ms
            if (nfds == -1) {
                // error
            } else if (nfds == 0) {
                // timeout
                printf("-- timeout, quit server ---\n");
                break;
            }

            for (int n = 0; n < nfds; ++n) {
                if (events[n].data.fd == skfd) {
                    // new connection
                    conn_sock = accept(skfd, (struct sockaddr*) &addr, &addrlen);
                    if (conn_sock == -1) {
                        // error
                    }
                    // setnonblocking(conn_sock);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = conn_sock;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                        // error
                    }
                    printf("==== add fd %d\n", conn_sock);
                } else {
                    // process data
                    char buf[40];
                    res = recv(events[n].data.fd, buf, sizeof(buf), 0);
                    if (res == -1) {
                        // error
                    } else if (res == 0) {
                        // close connection
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1) {
                            // error
                        }
                        printf(" --- remove fd %d from epoll\n", events[n].data.fd);
                        close(events[n].data.fd);
                    } else {
                        buf[res] = '\0';
                        printf("____ server got: %d, %s\n", res, buf);
                        if (buf[0] == 'Z') {
                            printf(" >> no data will come <<\n");
                        }
                        strcpy(buf, "___EDCRFV___");
                        send(events[n].data.fd, buf, strlen(buf), 0);
                    }
                }
            }
            
        }
        close(epollfd);
        close(skfd);
    }
}

