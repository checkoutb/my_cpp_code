
#include <chrono>
#include <thread>
#include <format>
#include <string>
#include <numeric>
#include <iostream>

#include <vector>
#include <algorithm>        // std::for_each
#include <functional>       // std::function    mem_fn

#include <limits>   // numeric_limits
#include <type_traits>      // is_arithmetic_v
#include <concepts>

#include <memory> //unique_ptr      // #  include <bits/unique_ptr.h>

#include <source_location>

#include <bit>      // bit_cast, std::byte, bit_width, popcount


// import <iostream>;      // gg。会有问题。 放弃 -fmodule-ts 后，下面的 编译器内部错误 都ok了 (应该，没有把所有注释都放出来)， 顺便把 fun01的 t3-t2 的 invalid pointer 也修好了。  module 还需要很长一段时间啊。
            // 。。 还是说， 我执行的  `g++ -std=c++23 -fmodules-ts -xc++-system-header iostream`  有问题？

// import std;     // 我看书上是 import std； 但是 编译失败。 看微软的教程 cl 可以 import std;  cppreference 是 import <iostream>
// https://learn.microsoft.com/en-us/cpp/cpp/tutorial-import-stl-named-module?view=msvc-170
// https://en.cppreference.com/w/cpp/language/modules

// 最开始的设想是， import std;  cout<<xx   。。 就是不需要 std::cout。。 看起来不行。 没有引入namespace的话，必须 std::

// gcc 版本 13.2.1 20231011 (Red Hat 13.2.1-4) (GCC)

/*

g++ -std=c++23 -fmodules-ts -xc++-system-header iostream
g++ -std=c++23 -fmodules-ts ch16_chrono.cpp

。。最开始用了c++20, 也可以。 转c++23的时候，第一条命令也需要执行下。不然找不到 c++23的 iostream， 编译失败。



g++ -std=c++23 -fmodules-ts ch16_chrono.cpp && ./a.out
..this will fatal      "during RTL pass: expand"

*/





// g++ ch16_chrono.cpp -std=c++20     or c++23






void do_work(int64_t millis);


// p235 - p235, 时钟
void fun01()
{
    using namespace std::chrono;

    auto t2 = system_clock::now();

    do_work(1234);

    auto t3 = std::chrono::system_clock::now();

    static_assert(std::is_same<decltype(t2), decltype(t3)>::value);

    static_assert(std::is_same_v<decltype(t2), decltype(t3)>);

    std::cout<<duration_cast<milliseconds>(t3 - t2).count()<<"ms\n";
    // std::cout<<std::chrono::duration_cast(t3 - t2).count()<<"\n";        // <must set>
    std::cout<<std::chrono::duration_cast<nanoseconds>(t3 - t2).count()<<"ns\n";

    std::cout<< t3-t2 <<"\n";        // free(): invalid pointer  ..???

    std::cout<<typeid(decltype(t2)).name()<<std::endl;
    std::cout<<typeid(decltype(t3)).name()<<std::endl;      // NSt6chrono10time_pointINS_3_V212system_clockENS_8durationIlSt5ratioILl1ELl1000000000EEEEEE

}


// 这个方法由于 module 的问题，导致好乱，放弃了， 不需要看， 看 fun02_2
auto fun02()
{
    using namespace std::chrono;

    // https://en.cppreference.com/w/cpp/chrono/year_month_day
    auto aday = April/6/2023;       // NSt6chrono14year_month_dayE        确实有 year_month_day 类型

    std::cout<<(aday.ok() ? "a good day" : "a bad day")<<std::endl;     // good day

    // constexpr std::chrono::year_month_day ymd{std::chrono::July/1/2021};     // https://en.cppreference.com/w/cpp/chrono/year_month_day/accessors

    // 4 - 6
    std::cout<<static_cast<unsigned>(aday.month())<<" - "<<static_cast<unsigned>(aday.day())<<std::endl;

    const std::chrono::time_point now{std::chrono::system_clock::now()};
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};
    std::cout<<ymd<<'\n';            // 编译器内部错误
    // https://en.cppreference.com/w/cpp/chrono/year_month_day      // 应该是 OS的问题。 上面的代码是来自 cppreference的。
            // 下面的显示 参阅 bugzilla.redhat的， 说明是 fedora的报错，不是 gcc的。在执行的时候 报错了， 编译ok。
                    // 确实，编译，不执行，ok的。
/*
$ g++ -std=c++23 -fmodules-ts ch16_chrono.cpp && ./a.out
during RTL pass: expand
In file included from /usr/include/c++/13/string:43,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
of module /usr/include/c++/13/iostream, imported at ch16_chrono.cpp:4:
In member function ‘constexpr _Tp* std::allocator< <模板形参-1-1> >::allocate(std::size_t) [with _Tp = char]’,
    inlined from ‘static constexpr _Tp* std::allocator_traits<std::allocator<_CharT> >::allocate(allocator_type&, size_type) [with _Tp = char]’ at /usr/include/c++/13/bits/alloc_traits.h:482:28,
    inlined from ‘static constexpr std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_allocate(_Char_alloc_type&, size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>]’ at /usr/include/c++/13/bits/basic_string.h:126:39:
/usr/include/c++/13/bits/allocator.h:193:39: 编译器内部错误：在 make_decl_rtl 中，于 varasm.cc:1442
  193 |             if (__builtin_mul_overflow(__n, sizeof(_Tp), &__n))
      |                 ~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~
Please submit a full bug report, with preprocessed source.
参阅 <http://bugzilla.redhat.com/bugzilla> 以获取指示。
预处理后的源码储存于文件 /tmp/ccGXRaOe.out，请将其附在错误报告内。
*/

    // std::cout<<aday;        // 也是 编译器内部错误

    // std::cout<<weekday(aday);      // 导致 编译器内部错误。。。
    // std::cout<<aday<<std::endl;

    // https://en.cppreference.com/w/cpp/chrono/weekday
    auto t2 = weekday(aday);
    // std::cout<<t2<<std::endl;        // 也是 编译器内部错误。

    std::cout<<typeid(decltype(aday)).name()<<std::endl;
    std::cout<<typeid(decltype(t2)).name()<<std::endl;

    auto bad = April/0/2022;

    if (!bad.ok())
        std::cout<<"bad date\n";


    sys_days t = std::chrono::sys_days{February/23/2023};
    t += days{7};
    auto d = year_month_day{t};
    std::cout<<d<<std::endl;     // 编译器内部错误
    std::cout<<std::format("{:%B}/{}/{}\n", d.month(), d.day(), d.year());           // 编译器内部错误

    std::cout<<std::format("{}/{}/{}\n", static_cast<unsigned int>(d.month()), static_cast<unsigned int>(d.day()), int(d.year()));

    std::cout<<static_cast<unsigned int>(d.month())<<" - "<<static_cast<unsigned int>(d.day())<<" - "<<int(d.year())<<std::endl;

    // std::cout<<std::format("{}-{}-{}\n", 1,2,3);     // 编译器内部错误 ...

    // std::string s = std::format("{} {}!", "Hello", "world", "something");       // 编译器内部错误
    // std::cout<<s<<std::endl;
    std::string s = std::to_string(static_cast<unsigned>(d.month())) + std::string("-") + std::to_string(int(d.year()));             // 编译器内部错误
    std::cout<<s;

    // std::cout<<std::to_string(1)<<std::endl;        // 编译器内部错误 .. 问题有点大。。

    // std::string s = std::to_string(1);      // 编译器内部错误

    // std::string s = "asd";          // 编译器内部错误
    // s += "123";

    std::cout<<"111";           // ok

    // ch16_fedora_string.cpp 中可以复现，  g++ -std=c++23 -fmodules-ts ch16_fedora_string.cpp  直接编译报错。也是 编译器内部错误 。。这个文件里 都是 编译可以， 执行报错。
            // g++ -std=c++23 ch16_fedora_string.cpp 编译成功， ./a.out 执行也成功。  所以是 -fmodules-ts 的问题。 nnd。


}


// p236 - p237， 日历， 时区。
auto fun02_2()
{
    using namespace std::chrono;
    auto day = April/1/2023;    // April 是 std::chrono::Month 类型
    std::cout<<weekday(day)<<'\n';      // Sat
    std::cout<<std::format("{:%A}\n", weekday(day));    // Saturday ( %A 星期几的全名)

    auto day2 = 2024y/April/3;
    std::cout<<day2<<std::endl;     // 2023-04-03

    auto bad = January/0/2024;
    if (!bad.ok())
        std::cout<<"not a valid date"<<std::endl;


    // year_month_day 类型 和 time_point 类型互换
    sys_days t = sys_days{February/23/2024};   // 时间点，精确到天
    t += days{7};
    auto d = year_month_day(t);     // 时间点 转为 日历
    std::cout<<d<<'\n'; // 2024-03-01
    std::cout<<std::format("{:%B}/{}/{}\n", d.month(), d.day(), d.year());  // March/01/2024  (%B 月份的全名)

    static_assert(weekday(April/7/2018) == Saturday);


    // ========= 时区 ===========
    auto tp = system_clock::now();      // tp 是 time_point 类型
    std::cout<<tp<<std::endl;    // 2024-01-07 08:46:34.368632090


    zoned_time ztp{current_zone(), tp};
    std::cout<<ztp<<'\n';       // 2024-01-07 16:46:34.368632090 CST


    // est 的类型 是 string 就可以了。。。。
    // 不行，无法声明 time_zone.  no matching function for call to ‘std::chrono::time_zone::time_zone
    // const time_zone est {"Europe/Copenhagen"};       // 哥本哈根
    // std::cout<<zoned_time{&est, tp}<<std::endl;
    // const time_zone zones {"Asia/Shanghai"};
    // std::cout<<zoned_time(&zones, tp)<<std::endl;


    // https://learn.microsoft.com/en-us/cpp/standard-library/time-zone-class?view=msvc-170
    const auto& tzdb = get_tzdb();
    const auto& cuZon = tzdb.current_zone();
    std::cout<<cuZon<<std::endl;


    // https://en.cppreference.com/w/cpp/chrono/zoned_time
    const std::chrono::zoned_time zt{"Europe/Copenhagen", std::chrono::system_clock::now()};
    std::cout<<std::setw(20)<<zt<<std::endl;    // 2024-01-07 10:11:37.701196226 CET


    const auto est {"Europe/Copenhagen"};        // ok   typeid.name == PKc
    // const std::string est = "Europe/Copenhagen";        // ok   typeid.name == NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
    std::cout<<zoned_time{est, system_clock::now()}<<std::endl;     // 2024-01-07 10:13:18.764537583 CET
    std::cout<<typeid(decltype(est)).name()<<std::endl;     // PKc    wtf?


    std::cout<<current_zone()<<std::endl;   // 0x113b7b8
    std::cout<<current_zone()->name()<<std::endl;   // Asia/Shanghai  .. current_zone() return pointer

}

class Shape;
// class Triangle;
// class Circle;
auto create_vs() -> std::vector<Shape*>;
auto invoke_draw() -> std::function<void(Shape*)>;


class Shape {
    int id;
    std::string type;
public:
    virtual void draw();
    virtual ~Shape() = default;
    Shape(int id, std::string type): id(id), type{type} {}      // () or {} in ctor-initializer
};

// p237 - p238 函数适配
auto fun03() -> void
{
    std::vector<Shape*> vs = create_vs();
    // vs.emplace_back(new Shape(1,"asd"));
    // vs.emplace_back(new Triangle(222, 222));
    // vs.emplace_back(new Circle(333, 333));
    auto lmd = invoke_draw();
    // auto lmd = [](Shape* p){p->draw();};

    std::cout<<typeid(decltype(lmd)).name()<<std::endl;     // Z5fun03vEUlP5ShapeE_

    std::for_each(vs.begin(), vs.end(), lmd);       // p238


        // C++11 引入 lambda 之前，是 将 面向对象调用风格 转为 函数式调用风格的 主要方式。    。。就是已经不用了？？
    std::for_each(vs.begin(), vs.end(), std::mem_fn(&Shape::draw));     // 有多态。
}


class Triangle: public Shape {
    int len;
public:
    Triangle(int id, int len): Shape(id, "triangle"), len(len) {}
    void draw() override;
};

class Circle: public Shape {
    int r;
public:
    Circle(int id, int r): Shape(id, "circle"), r{r} {}
};

void Shape::draw() {
    std::cout<<std::format("this shape is {}, id is {}.\n", type, id);
}

void Triangle::draw() {
    std::cout<<"this is triangle's fun. len is "<<len<<" , ";
    Shape::draw();
}

auto create_vs() -> std::vector<Shape*>{
    std::vector<Shape*> vs;
    vs.emplace_back(new Shape(111, "shape"));
    vs.emplace_back(new Triangle(222, 223));
    vs.emplace_back(new Circle(333, 321));
    return vs;
}

// lambda return type
auto invoke_draw() -> std::function<void(Shape*)> {
    return [](Shape* p){ p->draw(); };
}


// ---------------
// p239

template<typename A>
class Complex {
    A re, im;
    std::string v = "111";
public:
    static_assert(std::is_arithmetic_v<A>, "sorry, it must be number type");
};

// template<typename A>
// class Complex {          // re define... how to SFINAE ?
//     A re, im;
//     std::string v = "222";
// }

// // template<std::Arithmetic A>      // <concepts> not contain Arithmetic
// template<std::integral A>
// class Complex2 {
//     A re, im;
// };

// 无法 SFNIAE， 下面和上面 不能同时存在。。
template<std::floating_point A>
class Complex2 {
    A re, im;
};

template<typename T>
class Clz {
    T a;
public:
    T& operator*() const;
    T* operator->() const requires std::is_class_v<T>;             // 定义和声明都需要 requires xxxx
};

template<typename T>
T& Clz<T>::operator*() const
{
    std::cout<<std::format("this is operator*.\n");
    return const_cast<T&>(a);           // ... a 的const 是从哪里来的？
}

template<typename T>
T* Clz<T>::operator->() const requires std::is_class_v<T>       // 定义和声明都需要 requires xxxx
{
    std::cout<<std::format("this operator->..\n");

    return const_cast<T*>(&this->a);
}


void fun04() {
    constexpr float minf = std::numeric_limits<float>::min();
    constexpr int szi = sizeof(int);
    constexpr int sz2 = sizeof(Shape);
    constexpr int sz3 = sizeof(Triangle);
    constexpr int sz4 = sizeof(Circle);
    std::cout<<minf<<"\n";
    std::cout<<std::format("int {}, shape {}, triangle {}, circle {}\n", szi, sz2, sz3, sz4);

    bool aaa = std::is_arithmetic_v<int>;
    bool bbb = std::is_pointer_v<Shape*>;
    std::cout<<std::format("aaaa {}, bbbb {}\n", std::is_arithmetic_v<int>, std::is_pointer_v<Shape*>);
    std::cout<<std::format("{}, {}\n", aaa, bbb);
    std::cout<<std::format("{} \n", std::is_pointer_v<int>);

    using Res = std::invoke_result_t<decltype(create_vs)>;
    std::cout<<std::format("{}\n", std::is_void_v<Res>);
    using Res2 = std::invoke_result_t<decltype(fun03)>;
    std::cout<<std::format("   {}\n", std::is_void<Res2>::value);
    using Res3 = std::invoke_result_t<decltype(fun02_2)>;
    std::cout<<std::format("  {}, {}, {}\n", std::is_same_v<Res2, Res3>,
                           std::is_base_of_v<Shape, Circle>,
                           std::has_virtual_destructor_v<Circle>);

    // Complex<std::string> cs;        // compile error. static assert failed.
    Complex<int> ci;
    // Complex2<int> c2;
    Complex2<double> c3;        // template constraint failure

    Clz<int> clz1;
    Clz<std::string> clz2;

    std::cout<<clz2->empty()<<std::endl;
    std::cout<<(*clz2).empty()<<std::endl;

    // std::cout<<clz1.operator->()<<std::endl;
    std::cout<<(*clz1)<<std::endl;

}



// --------------
// 244

// const source_location sl = source_location::c
void mylog(const std::string& msg = "default log", const std::source_location sl = std::source_location::current())
{
    std::cout<<std::format("fname: {}, line: {}, column: {}, fun_name: {}, message: {}\n", sl.file_name(),
                           sl.line(), sl.column(), sl.function_name(), msg);
}
void testlog()
{
    mylog("hi world");      // sl.line number is this line's number (invoke line) , not mylog's declare line (it is fixed)
}

void fun05()
{
    mylog();
    testlog();

    std::source_location sl = std::source_location::current();

    mylog("a test", sl);
    mylog("1 line below");

    std::cout<<__FILE__<<", "<<__LINE__<<"\n";              // < c++20, use macro
    std::cout<<typeid(decltype(__FILE__)).name()<<std::endl;  // A16_c  // don't know
    std::cout<<typeid(decltype(__LINE__)).name()<<std::endl;            // int
    std::string asd = "asd";
    std::cout<<typeid(decltype(asd)).name()<<std::endl;
    int a = 123;
    std::cout<<typeid(decltype(a)).name()<<std::endl;
}



void do_work(int64_t millis)
{
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::this_thread::sleep_for(std::chrono::milliseconds(millis));

    // using namespace std::chrono_literals;
    using namespace std::literals::chrono_literals;     // both ok

    std::this_thread::sleep_for(100ms + 500us);
}


int main()
{
    std::cout<<"start..."<<std::endl;

    fun01();

    // fun02();
    fun02_2();

    fun03();

    fun04();

    fun05();

    return 0;
}

