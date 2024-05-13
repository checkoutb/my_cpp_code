
#include "../header/myheader.h"

class LT1195
{
public:



};



// Runtime
// 0ms
// Beats100.00%of users with C++
// Memory
// 8.27MB
// Beats93.46%of users with C++
class FizzBuzz {
private:
    int n;
    std::mutex mtx;
    int idx = 1;
    std::condition_variable cv;
public:
    FizzBuzz(int n) {
        this->n = n;
    }
// fizzbuzz, by 15
// fizz, /3 not /5
// buzz  /5 not /3
// i  not /3 not /5

    // printFizz() outputs "fizz".
    void fizz(function<void()> printFizz) {
        while (true)
        {
            std::unique_lock lck {mtx};
            cv.wait(lck, [&]{ return idx > n || (idx % 3 == 0 && idx % 5 != 0); });
            if (idx > n)
            {
                cv.notify_one();
                return;
            }
            printFizz();
            idx++;
            cv.notify_all();
        }
    }

    // printBuzz() outputs "buzz".
    void buzz(function<void()> printBuzz) {
        while (true)
        {
            std::unique_lock lck {mtx};
            cv.wait(lck, [&]{ return idx > n || (idx % 3 != 0 && idx % 5 == 0); });
            if (idx > n)
            {
                cv.notify_one();
                return;
            }
            printBuzz();
            idx++;
            cv.notify_all();
        }
    }

    // printFizzBuzz() outputs "fizzbuzz".
	void fizzbuzz(function<void()> printFizzBuzz) {
        while (true)
        {
            std::unique_lock lck {mtx};
            cv.wait(lck, [&]{ return idx > n || (idx % 3 == 0 && idx % 5 == 0); });
            if (idx > n)
            {
                cv.notify_one();
                return;
            }
            printFizzBuzz();
            idx++;
            cv.notify_all();
        }
    }

    // printNumber(x) outputs "x", where x is an integer.
    void number(function<void(int)> printNumber) {
        while (true)
        {
            std::unique_lock lck {mtx};
            cv.wait(lck, [&]{ return idx > n || (idx % 3 != 0 && idx % 5 != 0); });
            if (idx > n)
            {
                cv.notify_one();
                return;
            }
            printNumber(idx);
            idx++;
            cv.notify_all();
        }
    }
};



        // atomic, 必须t2,因为 2次load 之间可能发生了改变。
// Runtime
// 3ms
// Beats46.13%of users with C++
// Memory
// 8.30MB
// Beats93.46%of users with C++

// fizzbuzz, by 15
// fizz, /3 not /5
// buzz  /5 not /3
// i  not /3 not /5

// atomic, cv
class FizzBuzz_ {
private:
    int n;

    std::atomic_int ati = 1;

public:
    FizzBuzz_(int n) {
        this->n = n;
    }

    // printFizz() outputs "fizz".
    void fizz(function<void()> printFizz) {
        while (true)
        {
            int t2 = ati.load();
            if (t2 > n)
                return;

            if (t2 % 3 == 0 && t2 % 5 != 0)
            {
                printFizz();
                ati += 1;
            }
        }
    }

    // printBuzz() outputs "buzz".
    void buzz(function<void()> printBuzz) {
        while (true)
        {
            int t2 = ati.load();
            if (t2 > n)
                return;

            if (t2 % 3 != 0 && t2 % 5 == 0)
            {
                printBuzz();
                ati += 1;
            }
        }
    }

    // printFizzBuzz() outputs "fizzbuzz".
	void fizzbuzz(function<void()> printFizzBuzz) {
        while (true)
        {
            int t2 = ati.load();
            if (t2 > n)
                return;

            if (t2 % 15 == 0)
            {
                printFizzBuzz();
                ati += 1;
            }
        }
    }

    // printNumber(x) outputs "x", where x is an integer.
    void number(function<void(int)> printNumber) {
        while (true)
        {
            int t2 = ati.load();
            if (t2 > n)
                return;

            if (t2 % 3 != 0 && t2 % 5 != 0)
            {
                printNumber(t2);
                ati += 1;
            }
        }
    }
};

int main()
{

    LT1195 lt;


    return 0;
}
