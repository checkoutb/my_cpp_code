
#include <ev.h>
#include <iostream>
#include <ev.h>
#include <streambuf>
#include <fstream>

// g++ 01_client_select.cpp -lev

// just some test

static ev_idle idle;

static void idle_cb(struct ev_loop* loop, ev_idle *idle, int revents) {
    std::cout<<"idle start"<<std::endl;
    ev_idle_stop(loop, idle);
}

void func_idle() {
    struct ev_loop* loop = EV_DEFAULT;

    ev_idle_init(&idle, idle_cb);
    ev_idle_start(loop, &idle);
    
    ev_run(loop, 0);
}

// ------------------------------------------



// ./a.out < makefile
// echo "aa" | ./a.out
// cat makefile | ./a.out

// ./a.out  会5.5sec 后timeout


ev_io stdin_watcher;
ev_timer timeout_watcher;

static void stdin_cb(EV_P_ ev_io* w, int revents) {
    std::cout<<"stdin ready"<<std::endl;

    // ---------------
    //  ok!
    // char buffer[1000] = {0};
    // int res = read(0, &buffer, 999);
    // if (res > 0) {
    //     std::cout<<"ok read: "<<res<<" - "<<buffer<<std::endl;
    // } else {
    //     std::cout<<"failed read: "<<res<<std::endl;
    // }


    // ok.
    // char buf[10];
    // int res = -1;
    // // 第三个参数必须 10-1，  10太巧合了：
    // // 读取10个字符，所以导致 res 是10， ascii 10 是换行。
    // // 小端模式，所以 res 的内存是 10 0 0 0， 用char解读就是 1个回车 + 3个 \0
    // // 所以 输出 buf 的时候 除了最后一次，每次都带上一个 回车。然后 \0 断开字符串。
    // // 好奇怪，使用 echo "1" | asd.out 时， 123后面自带一个 回车。不知道为什么。
    // // 而且，读取10个字符，没有问题，因为这里是读取，所以你必须自己 进行连接，连接成 真正的 input，而不是 像现在这样 只是 输出一下。 而自己连接的时候， 是不需要 \0的。 
    // while ((res = read(0, &buf, 9)) > 0) {
    //     std::cout<<"ok "<<res<<" - "<<buf<<std::endl;
    //     std::cout<<int(buf[1])<<std::endl;
    //     // std::cout<<buf[9]<<std::endl;
    //     // std::cout<<int(buf[10])<<std::endl;
    //     // std::cout<<int(buf[11])<<std::endl;
    // }


    // FAIL: fb.open(int(fd) or FILE*, mode) is not supported
    // FILE* fp = fdopen(0, "r");
    // if (!fp) {
    //     std::cerr<<"cannot open 0"<<std::endl;
    //     return;
    // }
    // std::filebuf fb;
    // if (fb.open(fp, std::ios::in) == nullptr) {
    //     std::cerr<<"cannot open fd 0"<<std::endl;
    //     return;
    // }
    // std::istream is(&fb);

    // std::string line;
    // while (std::getline(is, line)) {
    //     std::cout<<"get: "<<line<<std::endl;
    // }


    // ......... 读到第一个空格或换行
    std::string line;
    std::cin>>line;
    std::cout<<line<<std::endl;
    std::cin>>line;
    std::cout<<line<<std::endl;
    std::cin>>line;
    std::cout<<lin e<<std::endl;


    
    ev_io_stop(EV_A_ w);
    ev_break(EV_A_ EVBREAK_ALL);
}

static void timeout_cb(EV_P_ ev_timer* w, int revents) {
    std::cout<<"timeout"<<std::endl;
    ev_break(EV_A_ EVBREAK_ONE);
}

static void func_2() {
    struct ev_loop* loop = EV_DEFAULT;

    ev_io_init(&stdin_watcher, stdin_cb, 0, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    ev_timer_init(&timeout_watcher, timeout_cb, 5.5, 0.);
    ev_timer_start(loop, &timeout_watcher);

    ev_run(loop, 0);
}


// -------------------------------








int main() {

   
    // char *p = "01234\0";   // with \0 or not, both are 5
    // std::cout<<strlen(p)<<std::endl;

    // std::string str = "01234\n";
    // std::cout<<str.size()<<std::endl;  // with \0, 6 ;;;;   no \0, 5;
    
    
    std::cout<<"client starting..."<<std::endl;

    // func_idle();   // ok

    // func_2();

    




    
    std::cout<<"client end successfully..."<<std::endl;
    return 0;
}

