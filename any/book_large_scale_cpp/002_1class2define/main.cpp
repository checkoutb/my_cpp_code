

#include <iostream>
#include "clzAdefine2.h"  // <> 无法搜索到文件。


/*
.

`g++ main.cpp`  undefined A::say()

---

`g++ -c clzAdefine2.cpp`
`g++ main.cpp claAdefine2.o`
`./a.out`  输出 define 22222

`g++ -c clzAdefine1.cpp`
`g++ main.cpp clzAdefine1.o`   // ok
`./a.out`  输出 define 111111   // 6， 2.h的名字， 1.cpp的实现。.h只是引入一个符号，内容是在link时确定。

`g++ main.cpp clzAdefine1.o clzAdefine2.o` compile error: multiple define A::say()
// 2个都是 强定义符号，所以 冲突了。


define.cpp只导入2个头文件，没有 A::say的定义， 可以 -c 生成 .o。 但是编译main时，无法找到 A::say



*/

int main() {

    A a;

    a.say();
    
}

