
// thread, mutex, lock(), packaged_task, future

#include <thread>
#include <iostream>
#include <chrono>
#include <syncstream>
#include <vector>
// #include <algorithm>
#include <numeric>      // accumulate
#include <mutex>        // mutex, scoped_lock
#include <shared_mutex>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <future>
#include <stop_token>

#include <coroutine>
// #include <generator>         // C++23        // std=c++23 没有这个文件。
                        // https://en.cppreference.com/w/cpp/coroutine/generator



// void task(std::chrono::duration a = 100ms + 500us)
// void task(std::chrono::milliseconds a)

std::osyncstream oss {std::cout};

void task(int millis = 1000)
{
    // std::osyncstream oss(std::cout);     // (maybe) oss lock std::cout until this function over... so it will serial。。串行。
    // oss<<"before task..."<<std::endl;
    // oss<<"before task.."<<std::endl;     // ？使用外面的 oss， 顺序没有问题，但是在退出程序前，才会瞬间打印所有的日志。。。
    std::cout<<"before task..."<<std::endl;
    using namespace std::literals::chrono_literals;
    // std::this_thread::sleep_for(100ms + 500us);
    // std::this_thread::sleep_for(a);
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    std::cout<<"task over..."<<std::endl;
    // std::osyncstream oss2(std::cout);
    // oss2<<"...task over"<<std::endl;
    // oss<<"...task over"<<std::endl;
}

void task(std::string name, int millis = 1000)
{
    std::cout<<std::format("{} before begin...", name)<<std::endl;
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    std::cout<<std::format("{} task over...", name)<<std::endl;
}



// no argument, no return
struct F {
    void operator()();
};
void F::operator()()
{
    task(1231);
}

void f() {
    task(1231);
}



// has argument, no return
struct F2 {
    std::vector<double>& v;
    F2(std::vector<double>& vd) : v{vd} { }
    void operator()();
};
void F2::operator()()
{
    for (auto i : this->v)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);
        // oss<<i<<",F ";
        // oss.flush();        // 没用, 还是 最后的时候一起输出

        std::cout<<i<<",F ";
        std::cout.flush();      // 有用
    }
    std::cout<<std::endl;
}

void f2(std::vector<double>& vd)
{
    for (auto i : vd)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);
        // oss<<i<<",f ";
        // oss.flush();

        std::cout<<i<<",f ";
        std::cout.flush();
    }
    std::cout<<std::endl;
}




// input return
void f3(const std::vector<double>& vd, double* res)
{
    *res = std::accumulate(vd.begin(), vd.end(), 0.0);
}

// return
double f4(const std::vector<double>& vd)
{
    double d = std::accumulate(vd.begin(), vd.end(), 100.0);
    return d;
}

class F3 {
    const std::vector<double>& vd;
    double* res;
public:
    F3(const std::vector<double>& vd, double* p) : vd{vd}, res{p} { }
    void operator()();
};
void F3::operator()()
{
    *this->res = std::reduce(vd.begin(), this->vd.end(), 10000.0);
}





void fun02()
{
    std::thread t1 {f};
    std::thread t2 {F()};   // {F{}} also ok

    t1.join();
    t2.join();

    {
        std::jthread j1 {f};        // destructor will invoke join();
        std::jthread j2 {F{}};
    }

    std::vector<double> vd {1.1,2.2,3.3};
    std::vector<double> vd2 {5.5,6.6,7.7};

    std::thread t3 {f2, std::ref(vd)};      // ref , <functional> 但我不知道哪个头文件间接引入了它。
    std::jthread j3 {F2{vd2}};
    std::jthread j4 {F2{vd}};
    t3.join();



    double d1, d2, d3;
    // std::jth         // 不能用 jthread，  可以用，但是要 作用域，不然打印结果的时候，还没有计算完。。
    {
        std::jthread j5 {f3, std::cref(vd), &d1};          // const ref ?
        std::jthread j6 {[&](){d2 = f4(vd);}};
        std::jthread j7 {F3{vd, &d3}};
    }
    std::cout<<d1<<", "<<d2<<", "<<d3<<std::endl;

}





// p264
std::mutex mtx;
std::mutex mtx1;
int sh;
void f5() {
    std::scoped_lock lck {mtx, mtx1};       // get 2 lock one time
    sh += 1;
    task("f5", 1234);
}
void f6() {
    std::scoped_lock lck{mtx};
    task("f6", 1000);
}
std::shared_mutex smtx;     // read-write lock
int sh2;
void f_read() {
    std::shared_lock lck{smtx};
    // read
    task("f read", 600);
    sh2 += 2;       // ...
    std::cout<<sh2<<std::endl;
}
void f_write() {
    std::unique_lock lck{smtx};
    // write
    task("f write", 1000);
    sh2 += 2000;       // ...
    std::cout<<sh2<<std::endl;
}

// mutex 代价较大，atomic 较小
// 265, 好奇怪，好像是 双重校验锁，  atomic 代价小，所以先用这个。
std::mutex mtxa;
std::atomic<bool> init_a;   // false
int Aza;      // need init

void f7() {
    if (!init_a) {
        std::lock_guard<std::mutex> lck(mtxa);
        if (!init_a) {
            Aza = 123;
            task("f7", 333);
            init_a = true;
        }
    }
}

void fun03() {

    {
        // serial
        std::jthread j1 (f5);
        std::jthread j3 (f6);
        std::jthread j2 (f5);
    }

    {
        // all 3 thread start, not wait.
        std::jthread j1 (f_read);
        std::jthread j2 (f_read);
        std::jthread j3 (f_read);

        task("main", 100);

        // after 3 f_read over, serial
        std::jthread j4 (f_write);
        std::jthread j5 (f_write);
        std::jthread j6 (f_write);

        task("main1", 100);

        // main + main1 wait 200ms, j1,j2,j3 hasn't over, j7,8,9 can start immediately.
        // after j1,2,3,7,8,9 all over, j4,5,6 start.
        std::jthread j7 {f_read};
        std::jthread j8 {f_read};
        std::jthread j9 {f_read};

        // std::cout<<std::this_thread.
        // std::cout<<std::this_thread::get_id()<<std::endl;           // 140102360364160...  毫秒数作为id

        // // std::jthread
        // std::thread j10("a_name_of_thread", [](){            // 被baidu AI耍了，没这个的。。
        //     auto id = std::this_thread::get_id();
        //     std::ostringstream oss4;
        //     oss4<<id;
        //     std::string name = oss4.str();
        //     std::cout<<"thread name: "<<name<<std::endl;
        //     f5();
        // });
        // j10.join();

        {
            std::jthread j11 {f7};
            task("main2", 100);
            std::lock_guard a (mtxa);
            std::cout<<Aza<<std::endl;
            std::cout<<"\n\n\n\n"<<std::endl;
        }


    }

}


// 266 等待事件

// 最简单的 事件 就是 时间流逝
void f1111()
{
    using namespace std::chrono;
    auto t0 = high_resolution_clock::now();
    std::this_thread::sleep_for(milliseconds(20));
    auto t1 = high_resolution_clock::now();

    std::cout<<duration_cast<nanoseconds>(t1 - t0).count()<<" nanosecond passed"<<std::endl;
}

// condition_variable
std::queue<int> que;
std::condition_variable cv;
std::mutex mtx4;
void consume()
{
    int cnt = 10;
    while (cnt-- > 0)
    {
        std::unique_lock<std::mutex> lck {mtx4};
        cv.wait(lck, []{ return !que.empty();});

        auto m = que.front();
        que.pop();

        lck.unlock();

        std::cout<<std::format("got {} in {} round\n", m, cnt);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}

void produce()
{
    int cnt = 12;
    while (cnt-- > 0)
    {
        {
            std::scoped_lock lck {mtx4};
            que.push(cnt);
            cv.notify_one();
            std::cout<<"make one "<<cnt<<std::endl;
        }
        // cv.notify_one();     // 这个也可以，看到 交替执行。
        std::this_thread::sleep_for(std::chrono::milliseconds(10));            // 必须有这个，就是 释放锁后，必须要有 等待，不然 立刻又获得锁了，无法看到 交替执行的 情况。
        // for (int i = 1000000; i > 0; --i) ;           // 也可以。  没有上面 sleep 明显。

        // }
    }
}


void fun04()
{
    f1111();

    std::jthread j1 {consume};
    // task(20);
    // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::jthread j2 {produce};
}


// 268 任务间通信
// 在任务层进行操作，而不是 底层。
void f_promise(std::promise<int>& p)
{
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(1234));
        // int a = 123/0;           // 直接崩了，没有 catch...
        p.set_value(13532);
    } catch (...) {
        std::cout<<"promise exception"<<std::endl;
        p.set_exception(std::current_exception());
    }
}
void f_future(std::future<int>& f)
{
    auto st = std::chrono::high_resolution_clock::now();
    try {
        auto a = f.get();
        std::cout<<"got "<<a<<std::endl;
        auto en = std::chrono::high_resolution_clock::now();
        std::cout<<"used "<<std::chrono::duration_cast<std::chrono::milliseconds>(en - st).count()<<"ms..."<<std::endl;
    } catch (...) {

    }
}


// https://blog.csdn.net/weixin_45312249/article/details/127772999
void f_future2(std::future<int>& f)
{
    std::future_status stat;
    do {
        stat = f.wait_for(std::chrono::milliseconds(500));
        if (stat == std::future_status::deferred) {
            std::cout<<"deferred, hasn't start"<<std::endl;
        } else if (stat == std::future_status::timeout) {
            std::cout<<"timeout,"<<std::endl;
        } else if (stat == std::future_status::ready) {
            std::cout<<"task has over"<<std::endl;
        }
    } while (stat != std::future_status::ready);

    auto a = f.get();
    std::cout<<"got "<<a<<std::endl;

    // f.wait();
}
void f_promise2(std::promise<int>& p)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    p.set_value(554433);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout<<"promise2 over"<<std::endl;
}



// packaged_task 简化了 promise 和 future 的相关设置。
double accu(std::vector<double>::iterator st, std::vector<double>::iterator en, double init)
{
    return std::accumulate(&*st, &*en, init);
}
double f_packaged_task()
{
    std::packaged_task pt1 {accu};
    std::packaged_task pt2 {accu};

    std::future<double> f1 {pt1.get_future()};
    std::future f2 {pt2.get_future()};

    std::vector<double> vd = {1.1,2.2,3.3,4.4,111.111,222.222,333.333};
    double* fstp = &vd[0];


    // error  std::jthread arguments must be invocable after conversion to rvalues
    // std::jthread j1(std::move(pt1), fstp, fstp + vd.size(), 0);
    // std::jthread j2(std::move(pt2), fstp, fstp + vd.size(), 0);


    // ok
    // std::jthread t1 {std::move(pt1), vd.begin(), vd.begin() + vd.size() / 2, 0};
    // std::jthread t2 {std::move(pt2), vd.begin() + vd.size() / 2, vd.end(), 0};


    // ok
    std::thread t1 {std::move(pt1), vd.begin(), vd.begin() + vd.size() / 2, 0};
    std::thread t2 {std::move(pt2), vd.begin() + vd.size() / 2, vd.end(), 0};
    t1.join();      // 必须有，不然 terminate called without an active exception
    t2.join();              // 但是有 f1.get了，它会等待 t1 的完成吧。


    // return f1.get();
    return f1.get() + f2.get();
}


//
double f_async(std::vector<double>& vd)
{
    if (vd.size() < 1)
    {
        return accu(vd.begin(), vd.end(), 0.0);
    }

    auto v0 = &vd[0];
    auto sz = vd.size();

    // error
    // auto f0 = std::async(accu, v0, v0 + sz / 4, 0.0);
    // auto f1 = std::async(accu, v0 + sz / 4, v0 + sz / 2, 0.0);
    // auto f2 = std::async(accu, v0 + sz / 2, v0 + sz * 3 / 4, 0.0);
    // auto f3 = std::async(accu, v0 + sz * 3 / 4, v0 + sz, 0.0);

    auto f0 = std::async(accu, vd.begin(), vd.begin() + sz / 4, 0.0);
    auto f1 = std::async(accu, vd.begin() + sz / 4, vd.begin() + sz / 2, 0.0);
    auto f2 = std::async(accu, vd.begin() + sz / 2, vd.begin() + sz * 3 / 4, 0.0);
    auto f3 = std::async(accu, vd.begin() + sz * 3 / 4, vd.end(), 0.0);

    return f0.get() + f1.get() + f2.get() + f3.get();

}


void f123()
{
    {
        std::promise<int> proms;
        std::future fut = proms.get_future();

        std::jthread j1 {f_promise, std::ref(proms)};
        std::jthread j2 {f_future, std::ref(fut)};
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout<<"\n\n\n=========\n\n\n";
    {
        std::promise<int> p2;
        std::future<int> f2 = p2.get_future();
        std::jthread j4 {f_future2, std::ref(f2)};
        std::this_thread::sleep_for(std::chrono::milliseconds(700));        // 没有办法触发 future的 deferred。 需要 std::async
        std::jthread j3 {f_promise2, std::ref(p2)};
    }




    double d = f_packaged_task();
    std::cout<<"packaged_task "<<d<<std::endl;




    std::vector<double> vd {1.1,2.2,3.3,111.111,222.222,333.333,4.4};
    double d2 = f_async(vd);
    std::cout<<"async "<<d2<<std::endl;


}


// stop other threads, when you got result
std::atomic<int> res_idx = -1;       // result's index
template<typename T> struct Range { T* first; T* last; };
void find(std::stop_token tok, const int* base, const Range<int> r, const int target)
{
    for (int* p = r.first; p != r.last && !tok.stop_requested(); ++p)
    {
        std::cout<<"to check "<<*p<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        if (target == *p)
        {
            res_idx = p - base;
            return;
        }
    }
}
void f_stop_other()
{
    std::vector<int> vi;
    for (int i = 0; i < 100; ++i)
    {
        vi.push_back(i);
    }
    int key = 65;

    // ---------
    int md = vi.size() / 2;         // 2 thread
    int* pvi = &vi[0];

    std::stop_source ss1{};
    std::jthread t1 {find, ss1.get_token(), pvi, Range{pvi, pvi + md}, key};

    std::stop_source ss2{};
    std::jthread t2 {find, ss2.get_token(), pvi, Range{pvi + md, pvi + vi.size()}, key};

    using namespace std::literals::chrono_literals;
    while (res_idx == -1)
        std::this_thread::sleep_for(10ms);

    ss1.request_stop();
    ss2.request_stop();

    std::cout<<"stop_thread.. "<<res_idx<<std::endl;
}


void fun05()
{
    // f123();
    f_stop_other();
}




// // coroutine
// std::generator<long long> fib()          // c++23
// // long long fib()
// {
//     long long a{0}, b{1};
//     while (a < b) {
//         auto next = a + b;
//         co_yield next;
//         a = b;
//         b = next;
//     }
//     co_return 0;
// }
//
// void f_fib()
// {
//     for (int i = 0; i < 10; ++i)
//     {
//         std::cout<<fib()<<" ";
//     }
// }


// wirte generator by youself...  (at bottom of the web page)
// https://en.cppreference.com/w/cpp/language/coroutines

void fun06()
{
    // f_fib();
}


int main()
{
    // fun02();

    // fun03();

    // fun04();

    // fun05();

    fun06();

    return 0;
}
