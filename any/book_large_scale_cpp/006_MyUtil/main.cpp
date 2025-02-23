
#include "myutil_ns_normal.h"
#include "myutil_struct_good.h"
#include <iostream>


/*
.

使用 struct， 因为 只有 myutil_struct_good.h 中可以修改。
而命名空间的话，main中也可以修改。导致 定义分散， 无法在一个地方找到所有的定义/声明


`g++ -c myutil_ns_normal.cpp`
`g++ main.cpp myutil_ns_normal.o`


`g++ -c myutil_struct_good.cpp`
`g++ main.cpp myutil_ns_normal.o myutil_struct_good.o`



*/


namespace MyUtil {
    int a = 123456; // 非常不好
}


int main() {

    std::cout<<MyUtil::PI<<std::endl;
    std::cout<<MyUtil::g()<<std::endl;
    std::cout<<MyUtil::count<<std::endl;

    std::cout<<MyUtil2::PI<<std::endl;
    std::cout<<MyUtil2::g()<<std::endl;
    std::cout<<MyUtil2::count<<std::endl;

    std::cout<<MyUtil::a<<std::endl;
    
}
