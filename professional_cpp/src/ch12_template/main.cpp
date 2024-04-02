
export module main;

import grid;
import mytest;
import <iostream>;
import <memory>;
// import <cstddef>;

/*

应该有大问题。写的真的痛苦啊。
问题1,老问题，std::format 一用就编译失败，日志 ./q1.txt
这个还好，我可以不用！！！


问题2, 日志18k ./q2.txt
本文件的 import <iostream> 注释掉， 编译，会发现 Grid的构造函数中的 vector的 resize() 不能用。 new不能用，导致无法 resize。
反正看不懂。无法理解。我照着Grid，又写了一个 Grid2, 这里没有导入iostream的情况下，也是 new 崩了，resize 崩了
真的无法理解 这里导入 iostream 和 grid.cc 的 vector的 resize 有 什么关系。

..看了下gcc的版本， c++20的modules，11开始就完全支持了。 13/14开始支持c++23的东西了。

*/

// 为Grid<int>生成全部的模板方法。 默认生成 所有虚方法 和 用 到 的 非虚方法。
template class Grid<std::string>;


// g++ -std=c++20 -fmodules-ts template_define.cc template_impl.cpp template_main.cc grid.cc main.cpp
// 不行，现在分开了 模板的定义和实现，但是在 实现中不能使用 形参。 不然 编译器内部错误。


int main()
{
    Grid<int> g(5,5);

    // std::cout<<"11\n";

    // Grid2<int> g2;

    g.at(1,1) = 123;
    int x { g.at(1,1).value_or(0) };
    std::cout<<x<<". \n";
    std::cout<<g.at(2,2).value_or(333)<<"\n";

    using GridInt = Grid<int>;
    GridInt g3;

    auto gridHeap { std::make_unique<GridInt>(2,2) };
    gridHeap->at(1,1) = 555;
    std::cout<<gridHeap->at(1,1).value()<<"\n";

    // template class Grid<int>;    // error

    MyGrid<int> mg(5);
    mg.setVal(2, 1234);
    std::cout<<mg.getVal(2).value_or(5555)<<'\n';

    return 0;
}

