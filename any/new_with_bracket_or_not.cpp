
#include <iostream>


// https://blog.csdn.net/m0_49715839/article/details/129418404
// 总结：对于有用户提供构造函数的类，不论有没有括号，都用构造函数进行初始化；如果没有构造函数，则不加括号的new只分配内存空间，不进行内存的初始化，而加了括号的new会在分配内存的同时初始化为0。



// 测试代码取自
// https://www.cnblogs.com/lvvou/p/14974734.html
// 总结： 在编写类时，只有在不给该类显式提供构造函数并在 new 对象加上括号()时, 其类内部数据成员才会被定义，其余情况都是未定义行为



// g++ (GCC) 13.2.1 20231011 (Red Hat 13.2.1-4)

// 我试下来
// 全局变量，全都初始化为0，即使 B(){} 中什么也没有做， 就是 应该默认变成了： B(): value{0} {} 。 不，这个 value 的值为0 是 new 作的，因为 `B i;` 的构造器打印了 随机值，所以 不可能 构造器初始化列表中有 value{0} 。 只可能是 new 的时候 全部置0了。
//   但是 全局变量 也是0， 所以 全局变量 也会 置0，即使没有 new

// 局部变量
//  带上 new, 就是 0值(带不带括号都可以), 否则是 随机值 , a有点奇怪,但不管了.



class A {
public:
    int value;
};

class B {
public:
    B(){
        std::cout<<" >>> "<<value<<" <<< "<<std::endl;
    }
    int value;
};

A g_a;      // 0
A *g_ap = new A; // 0
A *g_ap2 = new A(); // 0

B g_b; // 0
B *g_bp = new B; // 0
B *g_bp2 = new B(); // 0

int main()
{
    std::cout<<g_a.value<<", "<<g_ap<<", "<<g_ap->value<<", "<<g_ap2<<", "<<g_ap2->value<<std::endl;
    std::cout<<g_b.value<<", "<<g_bp<<", "<<g_bp->value<<", "<<g_bp2<<", "<<g_bp2->value<<std::endl;


    int a;
    // std::cout<<&a<<",a "<<a<<std::endl;        // g++：random value in runtime.  网上的应该是MSVC，会报错：使用了未初始化的局部变量a
    std::cout<<a<<std::endl;    // 很奇怪， 有全局变量 且 不打印 &a 的话， 会是 0
                // 没有全局变量 或 打印 &a, 会是 随机值.
    // std::cout<<&a<<",a "<<a<<std::endl;
            // 感觉不打印 &a 会被编译器优化掉了。 但不知道为什么 没有 全局变量的时候 没有 优化掉。

    // int b();     // 函数声明， 编译失败
    // std::cout<<b<<std::endl;

    int *c = new int;
    std::cout<<c<<",c "<<*c<<std::endl;      // 0    // 网上：随机值

    int *d = new int();
    std::cout<<d<<",d "<<*d<<std::endl;  // 0

    A e;    // 200510288。。。这可不随机啊。
    std::cout<<e.value<<std::endl;      // random in runtime

    // A f();  // function declaration

    A *g = new A;
    std::cout<<g->value<<std::endl; // 0    // 网上: 随机值

    A *h = new A();
    std::cout<<h->value<<std::endl; // 0

    B i;
    std::cout<<i.value<<std::endl;      // random

    B *j = new B;
    std::cout<<j->value<<std::endl; // 0    // 网上：random

    B *k = new B{};     // .. {} 也可以,和()差不多
    std::cout<<k->value<<std::endl; // 0    // 网上：random


    int x {};
    std::cout<<x<<std::endl;    // 0

    return 0;
}


