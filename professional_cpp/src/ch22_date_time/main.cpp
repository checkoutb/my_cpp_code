
#include <ratio>   // 有理数
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>


void ratio_1()
{
    using r1 = std::ratio<3, 60>;
    int num { r1::num };
    int den { r1::den };
    std::cout<<num<<", "<<den<<'\n';        // 1, 20

    const int t2 = 2;
    using r2 = std::ratio<t2, 40>;      // compile calculate, so t2 must const

    // ratio_  not_equal, less, less_equal, greater_equal
    std::cout<<std::ratio_equal<r1, r2>::value<<std::endl;          // 1

    std::cout<<std::atto::den<<'\n';        // 预定义的比例, 很多， (1/10)^24 到 10^24 都有

    using r3 = std::ratio<30>;
    std::cout<<r3::num<<" - "<<r3::den<<'\n';       // 30 - 1
}

void duration_2()
{

    std::chrono::duration<long> d1;     // 默认 ratio<1> 即 1s

    std::chrono::duration<long, std::ratio<60>> d2; // 60s

    std::chrono::duration<long, std::milli> d3;     // 1ms

    std::chrono::duration<long, std::ratio<60>> d4 {123};

    std::cout<<d1.count()<<" - "<<d4.count()<<'\n';     // 随即巨大值 - 123

    std::chrono::duration<int, std::ratio<1>> d5 {123};

    std::cout<<(d5 < d4)<<'\n';     // 1

    std::chrono::duration<int, std::ratio<1>> d6 {d5 + d4};
    std::cout<<d6.count()<<'\n';    // 7503 = 123 * 60 + 123

    std::chrono::duration<double, std::ratio<60>> d7 {d5};
    std::cout<<d7.count()<<'\n';        // 2.05 = 123/60

    // nanoseconds, microseconds, milliseconds, seconds, minutes, hours, days, weeks, years, months
    std::chrono::weeks w1 {1};
    std::chrono::days day2 {w1};
    std::cout<<"one week = "<<day2.count()<<" days\n";

    std::chrono::seconds ss{60};
    // std::chrono::minutes mm{ss};     // 没有提供这个构造器，因为可能出现 小数，所以就没有提供。
    std::chrono::milliseconds ms{ss};   // 必然是整数
    std::cout<<ms.count()<<'\n';        // 60000

    // h,min,s,ms,us,ns 字面量，在 std, std::literals, std::chrono_literals, std::literals::chrono_literals 命名空间中，这几个都可以。

    using namespace std::chrono_literals;
    auto d11 { 40min };
    std::cout<<d11.count()<<'\n';

}

void clock_3()
{
    // system_clock, steady_clock, high_resolution_clock
    // 系统实时时钟，  绝不递减的钟，  滴答周期最小
    // 都有 now() 静态方法

    // c++20 增加了 utc/tai/gps/file_clock

    std::chrono::system_clock::time_point tp { std::chrono::system_clock::now() };
    time_t tt { std::chrono::system_clock::to_time_t(tp) };     // 还有个 from_time_t
    tm* t { localtime(&tt) };
    std::cout<<std::put_time(t, "%H,%M,%S")<<'\n';      // HH,MM,SS



    /////////////////////////////

    auto start {std::chrono::high_resolution_clock::now()};
    for (int i = 0; i < 100; ++i)
        ;
    auto end {std::chrono::high_resolution_clock::now()};
    auto diff {end - start};
    std::cout<<std::chrono::duration<double, std::milli> {diff}.count() <<" ms\n";      // 0.00022 ms
    // 定时器的更新频率不高，可能导致数值不准确。


}

void time_point_4()
{
    // time_point 是相对于 纪元 的duration。 linux 的纪元是 1970.1.1,  window的纪元是 1601.1.1
}

void date_5()
{
    // C++20 增加的
    std::chrono::year y1 {2022};
    using namespace std::literals;
    std::chrono::year y2 {2222y};

    std::chrono::month m1 {6};
    using namespace std::chrono;
    std::chrono::month m2 {June};

    auto d2 {22d};

    year_month_day fulldate { 2222y, June, 22d };
    auto fd2 { 2222y / June / 22d };
    auto fd3 {22d/June/2222y};

    // 2022年6月的 第三个 周一
    year_month_day fd4 { Monday[3] / June / 2022 };

    auto june22 {June / 22d};
    auto june2222_6_22 {2222y/june22};

    auto june_lastday { June / last };
    auto june_last_2022 {2222y / june_lastday };

    auto last_monday_2022_6 { 2022y / June / Monday[last] };


    ///////////////////////
    auto today = {floor<days>(std::chrono::system_clock::now())};
    // std::cout<<today<<'\n';          // no

    std::chrono::system_clock::time_point t1 {std::chrono::sys_days { 2022y / June / 22d }};

    year_month_day ymd {floor<days>(t1)};
    year_month_day ymd2 {floor<days>(std::chrono::system_clock::now())};

    auto t2 {std::chrono::sys_days {2022y/June/22d} + 9h + 22min + 11s};

    auto t3 {t2 + years{1} + days{5}
        + 1s
    };

    std::cout<<ymd<<std::endl;      // 2022-06-22
    std::cout<<t2<<std::endl;       // 2022-06-22 09:22:11
    std::cout<<t3<<std::endl;       // 2023-06-27 15:11:23   这个 时分秒 是怎么回事？

}

void timezone_6()
{
    // ok
    // const auto& db {std::chrono::get_tzdb()};
    // for (const auto& tz : db.zones) {
    //     std::cout<<tz.name()<<'\n';
    // }

    auto* hhh { std::chrono::locate_zone("Pacific/Kosrae") };
    auto* gmt { std::chrono::locate_zone("GMT")};
    auto* curr {std::chrono::current_zone()};

    using namespace std::literals;
    using namespace std::chrono;
    auto t {std::chrono::sys_days {2022y/June/22d} + 12h + 12min + 12s};
    std::cout<<hhh->to_local(t)<<"\n";      // 2022-06-22 23:12:12

    // zoned_time

}

int main()
{
    ratio_1();

    duration_2();

    clock_3();

    time_point_4();

    date_5();

    timezone_6();

}
