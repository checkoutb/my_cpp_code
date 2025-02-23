
#include <iostream>
#include "liba.h"
// #include "liba.cpp"


/*
.

#include "liba.cpp"
`g++ main.cpp`
compile error: conflict declare  (int vs. double)

---

include "liba.h"

`g++ -c liba.cpp`
`g++ main.cpp liba.o`
`./a.out`  输出: 858993459.  应该是以int来解释double的bit？

使用了 1.0, 1.5, 1.125, 1.0/16, 都是输出0 。 根据double格式， 指数 + 位数，应该能输出一些东西啊。
改成1.3，就变成 负数： -858993459
1.4是 1717986918， 不理解 1.2 和 1.3 正好是相反数。


*/

int b = 333;  // g++会前向搜索， 在 liba.cpp 中使用了这个 b， `g++ main.cpp liba.o` liba.o 中的符号会从前面 的main.cpp 中搜索出定义。



int main() {
    std::cout<<"start...\n";

    extern int a;
    std::cout<<a<<std::endl;

    fun1(); // liba.h 中必须有声明， 引入符号
    
    std::cout<<"end...\n";
    return 0;
}

