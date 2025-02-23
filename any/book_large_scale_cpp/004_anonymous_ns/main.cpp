
#include <iostream>

/*
.

nm a.out

```text
0000000000401156 T _Z5funa2v
00000000004011ae T _Z6funa22v
0000000000404024 d _ZN12_GLOBAL__N_11aE
0000000000401178 t _ZN12_GLOBAL__N_15funa1Ev
000000000040116c t _ZN12_GLOBAL__N_15funa2Ev
```

匿名ns 和 全局 的 fun2 的标志不同， 但是 我不知道怎么才能调 匿名ns中的方法。
t: 局部非初始化数据段, local uninitialized data section
T: 全局非初始化数据段, gloal uninitialized data section

不管了， 反正匿名空间是内绑结的，二义性的话，直接修改 匿名空间中的方法名就可以了，不会影响 外部。

*/

void funa2() {  // funa2 可以在 匿名ns 之前，也可以之后， 效果一样，都是 和 匿名ns中的 funa2 冲突。

    std::cout<<"gggg\n";
}

namespace {
    void funa1();  // 不然 funa2 无法调用 funa1
}

namespace {
    int a = 3;

    void funa2() {
        funa1();
    }
}

namespace {

    void funa1() {
        std::cout<<" - - "<<a<<std::endl;
    }
}

void funa22() {
    std::cout<<" global\n";
}

int main() {

    // funa2();  // no global funa2, ok...   with global funa2, error, 二义性。
    // 似乎无法再调用 匿名空间中的 funa2 了。

    ::funa2();  // global funa2
    
    
    return 0;
}

