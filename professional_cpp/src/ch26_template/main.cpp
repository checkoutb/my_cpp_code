module;
#include <cstddef>

export module mymain;

import mygrid_v1;
import mygrid_v2;
import mygrid_v3;
import variadic;
import variadic_mix;
import meta_program;
import mytrait;

import <optional>;
import <vector>;
import <deque>;
import <iostream>;
import <string>;


namespace {
    int defaultint {11};        // < const T& DEFAULT> 必须有链接
    std::string defaultstr {"zzz"};
}

void work(int i) { std::cout<<"do work "<<i<<std::endl; }

int main()
{
    Grid<int, std::vector<std::optional<int>>> myvector;
    Grid<int, std::deque<std::optional<int>>> mydeque;

    myvector.at(3,4) = 5;
    std::cout<<myvector.at(3,4).value_or(11111)<<std::endl;

    mydeque.at(2,3) = 7;
    std::cout<<mydeque.at(2,3).value_or(11111)<<std::endl;

    // Grid<int, std::vector<std::optional<int>>> myv2 {myvector};
    Grid<int> myv2 {myvector};
    std::cout<<myv2.at(3,4).value_or(11111)<<std::endl;

    // Grid<int, double> t2;    // template constraint failure for ‘template<class T, class Container>  requires  ContainerType<Container> class Grid@mygrid_v1’
                //  the required expression ‘c.resize(1)’ is invalid
                //  the required type ‘typename Container::value_type’ is invalid


//////////////////////////////
    // template template
    mygrid_v2::Grid<int, std::vector> mgv2;
    mgv2.at(1,2) = 2;
    std::cout<<mgv2.at(1,2).value_or(11112)<<'\n';      // 2

    mygrid_v2::Grid<int, std::deque> mgdqv2;
    std::cout<<mgdqv2.at(1,1).value_or(231)<<'\n';      // 0

    mygrid_v2::Grid<int, std::vector, 567> mgv2d;
    std::cout<<mgv2d.at(1,1).value_or(11111)<<'\n';     // 567


    // mygrid_v2::Grid<int, std::vector, myv2> mgv2f;      // could not convert ‘myv2’ from ‘Grid@mygrid_v1<int, std::vector<std::optional<int> > >’ to ‘int’
                    // 第一个模板参数是 int，所以无法转换。

    mygrid_v2::Grid<double, std::vector, 2.2> mgv2g;
    std::cout<<mgv2g.at(1,1).value_or(1111.1)<<'\n';

    // < c++20， 非类型参数(就是v2的 第三个模板参数 DEFAULT) 不能是 对象，甚至不能是 浮点数。 只能是 整型，枚举，指针和引用
    // 20 允许 浮点数，甚至某种类类型，但这样的 类类型 有很多限制。本书不讨论。

    // const T& DEFAULT 可以使用任何类型实例化。 只要 传给DEFAULT的  具 有 链 接  (外链接 或 内链接)

    // mygrid_v2_ref::Grid<int, std::vector, defaultint> mgv2ref1;
    // mygrid_v2_ref::Grid<std::string, std::vector, defaultstr> mgv2ref2;
    // std::cout<<mgv2ref1.at(1,1).value_or(111111)<<'\n';
    // std::cout<<mgv2ref2.at(1,1).value_or("-----")<<'\n';

    // mygrid_v2_ref::Grid<int, defaultint> myv2ref3;
    // std::cout<<myv2ref3.at(1,1).value_or(11111)<<'\n';
    // mygrid_v2_ref::Grid<std::string, defaultstr> myv2ref4;
    // std::cout<<myv2ref4.at(1,1).value_or("----")<<'\n';


    // 不能有 Container。。
    mygrid_v2_ref::Grid<std::string, defaultstr> myv2ref5;
    std::cout<<myv2ref5.at(1,1).value_or("====")<<'\n';



//////////////////////////

    mygridv3::NGrid<int, 3> my3dgrid {4};
    my3dgrid[1][1][2] = 543;
    std::cout<<my3dgrid[1][1][2]<<'\n';


//////////////////////////

    MyVariadicTemplate<int, int, int, std::string, double, double> mvt1;
    MyVariadicTemplate<> mvt2;



//////////////////////

    processValues(1,2,3.456,"str..",1.4f);


/////////////////

    MyClass<Mixin1, Mixin2> mxa { Mixin1 {111}, Mixin2 {222} };
    mxa.mx1fun();
    mxa.mx2fun();

    MyClass<Mixin1> mxb { Mixin1 {333} };
    mxb.mx1fun();
    // mxb.mx2fun();   // 错误：‘class MyClass@variadic_mix<Mixin1@variadic_mix>’ has no member named ‘mx2fun’; did you mean ‘mx1fun’?

    MyClass<> mxc;

////////////////////////

    processValues2(123,234,"asd","zzz",123.123);

    printValues2("aaa","bbb",'c',1111,333.333);

    std::cout<<sumValues(1,2,3,111.1)<<"   ----\n";     // 117.1



////////////////////////

    std::cout<<"12! = "<<Factorial<12>::value<<std::endl;       // 12! = 479001600
    std::cout<<" also : "<<factorial(12)<<'\n';

    Loop<3>::run(work);         // 6




/////////////////////////
    checkType(1,32);
    checkType(1,2.2);


    Clz a;
    call(a);
    call(1);

    call2(a);
    call2(3.2);


    std::cout<<"end of all.....\n";
    return 0;
}







    // 1. g++ -std=c++20 -fmodules-ts -xc++-system-header iostream
    // 2. g++ -fmodules-ts -std=c++20 base.cc derived.cc base.cpp derived.cpp main.cpp
    // 3. ./a.out
