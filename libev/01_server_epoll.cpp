
#include <cstdarg>
#include <iostream>
#include <ev.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>



// port: 18080

// g++ 01_server_epoll.cpp -std=c++20 -lev
// echo "hihih" | nc 127.0.0.1 18080
// telnet 127.0.0.1 18080        Ctrl + ]  quit


static int listener_fd = -1;



// AI generate.. copilot
static void process_client_data(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    if (EV_ERROR & revents) {
        std::cerr << "Got invalid event" << std::endl;
        return;
    }

    char buffer[1024];
    ssize_t nread = read(watcher->fd, buffer, sizeof(buffer) - 1);
    if (nread < 0) {
        std::cerr << "Read error" << std::endl;
        return;
    } else if (nread == 0) {
        std::cout << "Client disconnected" << std::endl;
        ev_io_stop(loop, watcher);
        close(watcher->fd);
        free(watcher);
        return;
    }

    buffer[nread] = '\0';
    std::cout << "Received: " << buffer << std::endl;

    // Echo back to client
    write(watcher->fd, buffer, nread);
}

// 本来想 一个loop监听listener，一个loop监听client。但是好像很麻烦，再说啦。而且 2个loop好像没什么用，因为 thread只有一个。。
static void generate_evio_for_client_conn(int client_fd) {
    // ev_io client_watcher;
    // ev_io_init(&client_watcher, process_client_data, client_fd, EV_READ);
    // ev_io_start(ev_default_loop(0), &client_watcher);

    // 指针，不然出方法就没了
    ev_io *client_watcher = (ev_io*)malloc(sizeof(ev_io));
    ev_io_init(client_watcher, process_client_data, client_fd, EV_READ);
    ev_io_start(ev_default_loop(EVBACKEND_EPOLL), client_watcher);
}


static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {

    int client_fd = accept(listener_fd, nullptr, nullptr);
    if (client_fd < 0) {
        std::cerr << "Failed to accept connection" << std::endl;
        return;
    }
    std::cout << "Accepted connection: " << client_fd << std::endl;
    generate_evio_for_client_conn(client_fd);
}


int main() {
    struct ev_loop *loop = ev_default_loop(EVBACKEND_SELECT);

    // Here you would set up your server socket and accept connections
    // For demonstration, we will just run the event loop

    listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_fd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(18080);
    if (bind(listener_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        close(listener_fd);
        return 1;
    }
    if (listen(listener_fd, 5) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(listener_fd);
        return 1;
    }

    ev_io accept_watcher;
    ev_io_init(&accept_watcher, accept_cb, listener_fd, EV_READ);
    ev_io_start(loop, &accept_watcher);
    

    
    std::cout << "Starting event loop..." << std::endl;
    ev_run(loop, 0);

    ev_loop_destroy(loop);
    return 0;
}




