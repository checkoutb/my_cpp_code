
#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <stop_token>

#include <atomic>
#include <type_traits>

#include <shared_mutex>
#include <mutex>

#include <condition_variable>

#include <latch>
#include <barrier>
#include <semaphore>
#include <vector>

#include <future>

// cache line
// CPU有cache line，通常是64字节。向cache line 写会导致 锁定整行。
// 当2个线程 使用的数据 是一个 cache line 中的，会有严重的并发问题。
// 可以显式 内存对齐， C++17 <new> hardware_destructive_interference_size 可以用这个值 和 alignas 结合。









void counter(int id, int mx, int& ans)
{
    for (int i {1}; i <= mx; ++i)
        std::cout<<"counter "<<id<<", is : "<<i<<'\n';
    ans = id;
}

class Counter
{
public:
    Counter(int id, int mx) : m_id {id}, m_mx {mx} {}
    void operator()() const     // ()() ...
    {
        for (int i {1}; i <= m_mx; ++i)
        {
            std::cout<<"counter class "<<m_id<<", "<<i<<'\n';
        }
    }
private:
    int m_id;
    int m_mx;
};

class Request {
public:
    Request(int id) : m_id {id} {}
    void process() { std::cout<<"request.process "<<m_id<<'\n';}
private:
    int m_id;
};

void thread_2()
{
    int ans1 = 0;
    int ans2 = 0;
    std::thread t1 (counter, 1, 5, std::ref(ans1));         // 总是 值传递， 除非 <functional> std::ref, std::cref
    std::thread t2 {counter, 2, 7, std::ref(ans2)};
    t1.join();
    t2.join();

    std::cout<<ans1<<" - "<<ans2<<'\n';     // 1 - 2

///////////////////////

    std::thread t3 (Counter{3,5});
    Counter c {4,5};
    // std::thread t4 {c};     // 复制c
    std::thread t4 {std::ref(c)};
    t4.join();
    t3.join();

///////////////////////////

    int id = 5;
    int mx = 4;
    std::thread t5 {[id, mx]
        // () -> void           // 可以不写。
        {
        for (int i {0}; i < mx; ++i)
            std::cout<<"lambda "<<id<<", "<<i<<'\n';
    }};
    t5.join();

///////////////////////

    Request req { 111 };
    std::thread t6 {&Request::process, &req};
    t6.join();

/////////////////////////////


}


int k;
thread_local int n;
void threadLocal_fun(int id)
{
    std::cout<<"thread "<<id<<", "<<k<<", "<<n<<"\n";
    ++n;
    ++k;
}
void thread_local_2()
{
    std::thread t1 {threadLocal_fun, 1};        // 0,0
    t1.join();
    std::thread t2 {threadLocal_fun, 2};        // 1,0
    t2.join();

    // thread_local 在函数作用域内时， 变量的行为 和 static 变量 一样，只不过 每个线程都自己独立的副本。

}



void cancel_thread_2()
{
    // std::stop_token st;

    std::jthread job2 {[](std::stop_token st) {
        using namespace std::literals::chrono_literals;
        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(100ms);
            std::cout<<"in thread..."<<i<<'\n';
            if (st.stop_requested()) {            // <<<<<<<<<<<<<<
                std::cout<<"sb. request stop me, bye\n";
                return;
            }
        }
    }};

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(420ms);
    job2.request_stop();


//////////////////////
    // tour of cpp
//     void find(std::stop_token tok, const int* base, const Range<int> r, const int target)  // when find, will set res_idx
//
//     std::stop_source ss1{};
//     std::jthread t1 {find, ss1.get_token(), pvi, Range{pvi, pvi + md}, key};
//
//     std::stop_source ss2{};
//     std::jthread t2 {find, ss2.get_token(), pvi, Range{pvi + md, pvi + vi.size()}, key};
//
//     using namespace std::literals::chrono_literals;
//     while (res_idx == -1)
//         std::this_thread::sleep_for(10ms);
//
//     ss1.request_stop();          // <<<<<<<<<<<<<<<<
//     ss2.request_stop();


}


void throw_rethrow_exception_2()
{
    // exception_ptr current_exception() noexept;
    // [[noreturn]] void rethrow_exception(exception_ptr p);
    // template<class E> exception_ptr make_exception_ptr(E e) noexcept;
}

class Foo {private: int arr[123];};
class Bar {private: int i;};

void atomic_3()
{
    // atomic_bool    atomic<bool>
    // atomic_int     atomic<int>
    // ...

    std::atomic<Foo> f;
    std::cout<<std::is_trivially_copyable_v<Foo><<" "
    // <<f.is_lock_free()           // 不是 lock free 的，根本没有这个方法， 编译失败。
    <<'\n';
    std::atomic<Bar> b;
    std::cout<<std::is_trivially_copyable_v<Bar><<" "
    <<b.is_lock_free()      // compile success, so b is lock free
    <<'\n';


////////////////////////////
    // bool atomic<T>::compare_exchange_strong(T& expected, T desired);

    // atomic<T>::fetch_add()   返回旧值。  可以指定内存序


////////////////////////////

    // c++20  atomic<std::shared_ptr<T>> 。 之前不可以, 因为shared_ptr 不可拷贝
    // shared_ptr 的 引用计数器 一直是 线程安全的。 其它内容不是线程安全的。 如果在 多线程调用 同一个shared_ptr 的非 const方法 (如 reset()。。shared_ptr的方法，不是 内部的对象的方法。) 会导致 数据竞争。
    // 在shared_ptr 内部的对象 上 调用 非 const 依然不是线程安全的。 。。。 意思应该是 atomic<shared_ptr> 调用 内部的对象的 非const 方法 依然不安全。  但是 调用 shared_ptr 的 非const 方法 是安全的。

    // 。。不过 const方法 还有个 mutable 变量 在坑它 啊。    所以 mutable 类属性 得自己确保 原子性啊。 不能 int counter;


    // c++20 std::atomic_ref ,  这个是 引用，  atomic 总是会 拷贝一份。


////////////////////////////////////


    std::atomic<int> ato {5};

    std::jthread t1 ([&ato](){      // 。。我把 t1 漏了，编译成功， 但是下面的 change... 没有执行。
                            // 知道了，没有t1 就是临时变量， 在本行的最后 要析构，析构里 要 join，，就会一直等待 线程结束。卡住 main线程，导致 下面无法执行。  死锁了。
                        // 这个在书上有说到过。但不知道在哪里了。
        // using namespace std::literals::chrono_literals;
        // std::this_thread::sleep_for(100ms);
        std::cout<<"wait for change ... \n";
        ato.wait(5);
        std::cout<<"wake up..."<<ato<<'\n';
    });
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(300ms);


    ato = 2;
    std::cout<<"change ..\n";
    ato.notify_all();


}

/////////////////////////////
void inc(std::atomic<int>& cnt)
{
    for (int i = 0; i < 100; ++i)
        ++cnt;
}
void inc(int& cnt)
{
    std::atomic_ref<int> ato {cnt};
    for (int i = 0; i < 100; ++i)
        ++cnt;
}
void inc_best(std::atomic<int>& cnt)     // <<<<<<<<<<<<  best
{
    int t2 = 0;
    for (int i = 0; i < 100; ++i)
        ++t2;
    cnt += t2;
}
/////////////////////////////



// 自旋锁
std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
void spinlock_4(int id)
{
    for (int i = 0; i < 10; ++i)
    {
        while (spinlock.test_and_set()) ;         // wait the lock
        std::cout<<" got spinlock ... "<<id<<", "<<i<<'\n';
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(10ms);
        spinlock.clear();                // release lock
        std::this_thread::sleep_for(3ms);      // 不加这个，( 试 了 几 次 )不会交错。
    }
}

std::mutex mtx1;
std::mutex mtx2;
void process_get_2locks_in_1_time()
{
            // std::defer_lock_t 实例告知 unique_lock 不要在 构造期 获得锁。
    {
        std::unique_lock lk1 {mtx1, std::defer_lock};
        std::unique_lock lk2 {mtx2, std::defer_lock};
        std::lock(lk1, lk2);

        std::cout<<" lllloooock 2 lk\n";
    }   //        // lk1, lk2 auto release



////////////////////////

    std::scoped_lock lks {mtx1, mtx2};      // mutex, not lock

    std::cout<<" lllloooock 3 lk\n";

}

/////////////////////////
std::once_flag g_of;
void this_should_call_one()
{
    std::cout<<"just appear once\n";
}
void process_once_flag()
{
    std::call_once(g_of, this_should_call_one);
    std::cout<<"after call once\n";
}

//////////////////

std::timed_mutex tmtx;
void timed_lock(int a)
{
    using namespace std::literals::chrono_literals;
    std::unique_lock lk {tmtx, 50ms};
    if (lk)
    {
        std::cout<<a<<"   got timed lock ..sleep..hold lock..\n";
        std::this_thread::sleep_for(100ms);
        std::cout<<a<<"  wake up\n";
    }
    else
    {
        std::cout<<a<<" dont got the timed lock..\n";
    }
}

void mutex_4()
{
    {
        std::jthread t1 (spinlock_4, 1);
        std::jthread t2 (spinlock_4, 2);
    }

//////////////////////////////////

    // mutex, recursive_mutex, shared_mutex,  3个都支持 lock, try_lock, unlock
    //          share: lock_shared,try_lock_shared,unlock_shared

    // timed_mutex, timed_recursive_mutex, timed_shared_mutex, lock,try_lock,unlock,
        // try_lock_for(), try_lock_until()
            // share: lock_shared,try_lock_shared,unlock_shared, try_lock_shared_for, try_lock_shared_until

////////////////////////////////

    // lock_guard, unique_lock, shared_lock
    // lock 一次性获得多个锁

    process_get_2locks_in_1_time();      // std::lock, std::scoped_lock

    process_once_flag();        // 并发也 只会调用一次。
    process_once_flag();
    process_once_flag();


////////////////////

    {
        std::jthread t1 {timed_lock, 1};
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(5ms);
        timed_lock(2);
    }

}

////////////////////
std::mutex mtx5;
void cv_5()
{
    // std::condition_variable , std::condition_variable_any
    // notify_one, notify_all, wait(unique_lock<mutex>), wait_for, wait_until

    // 虚假唤醒， 使用 带谓词的 wait， 或 手动 谓词下。

    std::condition_variable cv;
    std::unique_lock lk{mtx5};
    //
    cv.notify_all();    // 无论当前是否有锁，都可以 调用 notify_all, notify_one . // p780
                // 。。但是有锁的时候，调用 notify， 别人醒过来 拿不到锁啊。 等于白 notify 了。

}


//////////////////

void latch_6()  // 一次性的
{
    std::latch lat { 3 };       // arrive_and_wait (会减少计数值), wait(不会减少)

    std::jthread t1 {[&lat]() {
        std::cout<<"arrive and wait  latch...\n";
        lat.arrive_and_wait();
        std::cout<<"t111 end\n";
    }};
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);
    std::jthread t2 {[&lat]() {
        std::cout<<"wait, not decre latch...\n";
        lat.wait();
        std::cout<<"t2  end\n";
    }};

    std::this_thread::sleep_for(100ms);
    std::cout<<"down to 1111. latch...\n";
    lat.count_down();
    std::cout<<"down to zero latch...\n";
    lat.count_down();
    // lat.count_down();        // 多 down 的话，t1,t2会卡住。 不知道为什么
    // lat.count_down();        // 一旦计数器达到0,就会保持开通。。 所以 变成负数 会 卡住？
                        // 书上是 简单的信号量 (latch的值是1)， 让 一些线程等待， 直到某个线程 完成一件事情(初始化，或加载磁盘数据)。

    std::jthread t3 {[&lat](){
        std::cout<<"don't wait..\n";
        lat.wait();
        std::cout<<"t3 end...\n";
    }};

}

///////////////
void fun_when_barrier() noexcept
{
    std::cout<<"barrier complete\n";
}
void barrier_7()
{
    std::barrier bar {2, fun_when_barrier};

    std::vector<std::jthread> vt;
    for (int i = 0; i < 5; ++i)
    {
        std::jthread t2 {[&bar, i](std::stop_token st){
            std::cout<<"barrier "<<i<<" enter in ...\n";
            bar.arrive_and_wait();
        }};
        vt.push_back(std::move(t2));        // 必须move， jthread 没有 拷贝。
    }

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(10ms);

    vt.back().request_stop();       // 这个没用，因为 if (st.stop_requested()) {  需要主动检查， 卡在 bar 上了， 不可能 出来 检查。。

    std::this_thread::sleep_for(10ms);
    std::cout<<"bar++++++\n";
    bar.arrive_and_wait();      // ? 试 semaphore 的时候， 这里卡住过一次，看代码是 0,1 complet， 2,3,4 complete。。。 然后这里 在wait，就 卡住了。
                // 但是 再试了N次， 没有复现。。 不知道 那次是什么原因。



}


///////////////////////



void semaphore_8()
{
    // 轻量级同步原语，可以用作其他同步机制(mutex,latch,barrier)的构建块
    // std::couting_semaphore,  std::binary_semaphore
    // acquire, try_acquire, try_acquire_for, try_acquire_until. release

    std::counting_semaphore sp { 3 };
    std::atomic_int atoi;
    std::vector<std::jthread> vj;

    for (int i = 0; i < 10; ++i)
    {
        vj.push_back(std::jthread([&sp, &atoi](std::stop_token st){
            while (true) {
                sp.acquire();
                ++atoi;
                std::cout<<"got semaphore, and ++atoi : "<<atoi<<std::endl;
                using namespace std::literals::chrono_literals;
                std::this_thread::sleep_for(20ms);
                --atoi;
                sp.release();

                if (st.stop_requested()) {
                    std::cout<<"stop token ...\n";
                    return;
                }
            }
        }));
    }

    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);

    std::cout<<"goto kill all jthread semaphore\n";
    for (std::jthread& jt : vj)
        jt.request_stop();



}


////////////////////////

void workwork(std::promise<int> pro)
{
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(24ms);

    std::cout<<"set value promise\n";
    pro.set_value(115533);
}

void waitfuture(std::future<int> fut)
{
    for (int i = 0; i < 10; ++i)
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(7ms);
        // if (fut.wait_for(0ms)) {


        // https://en.cppreference.com/w/cpp/thread/future/wait_for  // do_while, switch
        std::future_status fs = fut.wait_for(0ms);
        if (fs == std::future_status::ready) {
            int t2 = fut.get();
            std::cout<<" got the result in future : "<<t2<<"  \n";
            return;         // 不加的话， 继续 wait_for，会 核心已转储： std::future_error: No associated state

        } else {
            std::cout<<" no ,not ready in future...\n";
        }
    }
}

int work_packaged_task(int a)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(50ms);
    return a + a;
}

void future_promise_packaged_task_9()
{
    // myfut.wait_for(0ms) 可以询问结果是否可用 (等待0ms)。   myfut.get()      get只能调用一次。
    // ... wait_for 返回的是 枚举。。

    // promise.set_value   set_exception  只能调用一次 set_xxx, 第二次 std::future_error 异常
    // 使用 set_exception，   future.get 会抛出这个异常。

    // future 的析构器 也会 等待 结果。

    // future<T> 要求 T 可move。  调用 get 时， 将 T move出来。 所以 只能调用一次 get

    // 如果要 多次 get， 或者 从 多个线程 多次 调用， 需要使用 std::shared_future<T>
                    // my_fut.share()  或 move future 给 shared_future 的构造器。
    // shared_future 可以用于 同时 唤醒 多个线程。


    {
        std::promise<int> prom;
        std::future<int> fut {prom.get_future()};

        std::thread t1 {workwork, std::move(prom)};
        std::thread t2 {waitfuture, std::move(fut)};        // must move

        t1.join();
        t2.join();

    }


//////////////////////////////
    std::packaged_task<int(int)> tsk {work_packaged_task};
    std::future fut {tsk.get_future()};
    std::thread t1 {std::move(tsk), 1234};
    int t2 = fut.get();
    std::cout<<". got from packaged_task: "<<t2<<std::endl;
    t1.join();

//////////////////////

    std::future ff { std::async(work_packaged_task, 3333) };
    int t22 = ff.get();
    std::cout<<"got from async's future "<<t22<<'\n';



}



int main()
{
    thread_2();

    thread_local_2();

    cancel_thread_2();

    throw_rethrow_exception_2();

    atomic_3();

    mutex_4();

    // C++20 std::basic_osyncstream  <syncstream> 在销毁的时候才显示日志

    cv_5();

    latch_6();  // 一次性的

    barrier_7();

    semaphore_8();      // // 轻量级同步原语，可以用作其他同步机制(mutex,latch,barrier)的构建块

    future_promise_packaged_task_9();

    // thread_pool

    // coroutine

}

