
#include "../header/myheader.h"

#include <functional>
#include <thread>
#include <condition_variable>
#include <semaphore>


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



class LT1115
{
public:



};




// Runtime
// 11ms
// Beats41.16%of users with C++
// Memory
// 10.12MB
// Beats41.92%of users with C++
class FooBar {
private:
    int n;
    std::counting_semaphore<1> semf {1};        // must <N>
    std::counting_semaphore<1> semb {0};
public:
    FooBar(int n) {
        this->n = n;
    }

    void foo(function<void()> printFoo) {

        // std::counting_semaphore asd {3};     // 不需要 <N>
        // asd.acquire();

        for (int i = 0; i < n; i++) {
            semf.acquire();
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            semb.release();
        }
    }

    void bar(function<void()> printBar) {

        for (int i = 0; i < n; i++) {
            semb.acquire();
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            semf.release();
        }
    }
};



// Runtime
// 0ms
// Beats100.00%of users with C++
// Memory
// 9.82MB
// Beats93.23%of users with C++
class FooBar_2 {
private:
    int n;
    std::condition_variable cv;
    std::mutex mtx;
    int cnt = 0;
public:
    FooBar_2(int n) {
        this->n = n;
    }

    void foo(function<void()> printFoo) {

        for (int i = 0; i < n; i++) {
            // {        // not need
                // cout<<"foo、"<<endl;
                std::unique_lock<std::mutex> lck {mtx};
                cv.wait(lck, [&]{ return cnt % 2 == 0; });
                // printFoo() outputs "foo". Do not change or remove this line.
                printFoo();
                ++cnt;
            // }
            cv.notify_one();
        }
    }

    void bar(function<void()> printBar) {

        for (int i = 0; i < n; i++) {
            // {
                // cout<<"bbb、"<<endl;
                std::unique_lock<std::mutex> lck {mtx};
                cv.wait(lck, [&]{return cnt % 2 == 1; });
                // printBar() outputs "bar". Do not change or remove this line.
                printBar();
                ++cnt;
            // }
            cv.notify_one();
        }
    }
};


// Runtime
// 66ms
// Beats5.02%of users with C++
// Memory
// 10.24MB
// Beats23.25%of users with C++

class FooBar_1 {
private:
    int n;
    std::atomic_int ati;
public:
    FooBar_1(int n) {
        this->n = n;
        this->ati = 0;
    }

    void foo(function<void()> printFoo) {

        for (int i = 0; i < n; i++) {
            while (ati.load() % 2 == 1)
                ;
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            ati.fetch_add(1);
        }
    }

    void bar(function<void()> printBar) {

        for (int i = 0; i < n; i++) {
            while (ati.load() % 2 == 0)
                ;
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            ati += 1;
        }
    }
};


void foo() {
    cout<<"foo,";
}
void bar() {
    cout<<"bar,";
}

int main()
{

    LT1115 lt;

    FooBar_2 fb(10);

    std::jthread th1(&FooBar_2::foo, &fb, &foo);
    std::jthread th2(&FooBar_2::bar, &fb, &bar);


    return 0;
}
