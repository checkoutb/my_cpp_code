
export module main;

import grid;
import <iostream>;


/*

https://github.com/carlesmartin85/procpp5e/blob/main/code/c12_code/03_GridNonType/GridTest.cpp

有2个差别
1. format，我还是不行
$ g++ -std=c++20 -fmodules-ts grid.cc  main.cpp
In module /usr/include/c++/13/format, imported at grid.cc:12,
of module grid, imported at main.cpp:4:
/usr/include/c++/13/format: In instantiation of ‘class std::basic_format_arg<std::basic_format_context<std::__format::_Sink_iter<char>, char> >’:
/usr/include/c++/13/format:3617:8:   required from ‘constexpr void std::__format::_Formatting_scanner<_Out, _CharT>::_M_format_arg(std::size_t) [with _Out = std::__format::_Sink_iter<char>; _CharT = char; std::size_t = long unsigned int]’
/usr/include/c++/13/format:3614:7:   required from here
/usr/include/c++/13/format:2817:11: 编译器内部错误：段错误
 2817 |     class _Arg_store;
      |           ^~~~~~~~~~


2. github上，main方法所在文件没有 export module xxx;
$ g++ -std=c++20 -fmodules-ts grid.cc  main.cpp
‘
/usr/include/c++/13/optional:735：不能从先前的错误中恢复，退出
预处理后的源码储存于文件 /tmp/ccxk309a.out，请将其附在错误报告内。

我这里增加 export module main； 后才可以编译。

*/

consteval size_t getHeight()
{
    return 12;
}

int main()
{
    Grid<int, 10, 10> mg;

    // std::cout<<"222\n";
    std::cout<<mg.getHeight()<<'\n';

    Grid<int, getHeight(), getHeight()> mg2;
    std::cout<<mg2.getHeight()<<'\n';

    return 0;
}
