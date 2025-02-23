
#include <template1.h>   // `g++ main.cpp -I.`
#include <iostream>
#include <typeinfo>

/*
.

template1.h 中只有声明，没有定义的话：编译报错，
因为编译时需要对模板进行实例化，但是 找不到 模板的定义，所以无法实例化。

模板特化 和普通函数一样，可以 声明，定义 分离。  模板无法分离 声明，定义

---

加入 double特化的定义 到 .h 后，
`g++ main.cpp -I. template1.o`  是可以的。

但是 `g++ -c template1.cpp` 后，上面的 编译 就会失败。
因为 现在 template1.o 中有 double特化的定义了，main.cpp又导入了double特化的定义， 2者冲突了。


特化和普通函数 是一样的， 不能定义2次。 所以 要么 inline， 要么特化的声明和定义分离

inline后，可以直接编译main，不需要重新编译 template1.o


>>>> 实例化模板 优先于 隐式转换。

模板实例化，生成的是 弱符号(W)

00000000004014d8 W _Z7compareIdEiRKT_S2_
0000000000401511 W _Z7compareIiEiRKT_S2_
0000000000401619 W _Z7compareIPKiEiRKT_S4_
0000000000401565 W _Z7compareIsEiRKT_S2_
00000000004015bf W _Z7compareIxEiRKT_S2_
但是看不出是什么类型的。

可以代码中 typeid(xx).name()， 可以看到简化的后类型。和上面的 模板的类型中出现的 一样。
比如下面的 pa 是 PKi， a 是 i。
PKi 估计就是 pointer const int

不知道怎么让 g++ 不要缩写。 -g ? 不行。

模板里也可以用 typeid(xx).name() 来获得类型，不过是 缩写后的类型。


*/

int main() {

    int a = 2, b = 5;

    std::cout<<compare(a, b)<<std::endl;  // normal
    
    std::cout<<compare<int>(a, b)<<std::endl; // template

    char ch1 = 'a', ch2 = 'd';

    std::cout<<compare(ch1, ch2)<<std::endl;  // specialization   .. not compare(int, int)
    std::cout<<compare<char>(ch1, ch2)<<std::endl; // specialization

    double da = 1.222, db = 123123.233;
    std::cout<<compare(da, db)<<std::endl; // specialization


    long long lla = compare<int>(a, b);
    std::cout<<"  "<<lla<<std::endl;
    short sa = compare<int>(a, b);
    std::cout<<" ss "<<sa<<std::endl;

    sa = a;
    std::cout<<sa<<"\n\n";
    short sb = 3;
    std::cout<<compare(sa, sb)<<std::endl; // template  实例化模板 优先于 隐式转换
    std::cout<<compare(a, sb)<<std::endl; // normal

    std::cout<<compare<int>(a, sb)<<std::endl; // template

    std::cout<<compare(lla, sa)<<std::endl;  // normal
    std::cout<<compare(lla, 1LL * sa)<<std::endl; // template


    const int * const pa = &a, * const pb = &b;
    std::cout<<compare(pa, pb)<<std::endl; // template
    std::cout<<pa<<" - "<<pb<<std::endl;


    std::cout<<typeid(pa).name()<<std::endl;
    std::cout<<typeid(a).name()<<std::endl;
    
    return 0;
}
