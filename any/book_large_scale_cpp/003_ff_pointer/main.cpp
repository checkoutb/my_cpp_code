
#include <iostream>


int add(int a, int b) {
    std::cout<<"add. "<<(a+b)<<std::endl;
    return a+b;
}

int minus(int a, int b) {
    std::cout<<"minus. "<<(a-b)<<std::endl;
    return a-b;
}

class A {

    static int pri_fun(int a, int b) {
        std::cout<<a<<"  private...\n";
        return 246;
    }

public:
    int (*pub_fun(int z))(int a, int) {
        A a;
        std::cout<<z<<std::endl;
        return A::pri_fun;
    }
};


int (*ffpadd())(int, int) {
    return add;
}

void f() { }

int main() {

    int (*ffp)(int, int) = add;

    ffp(2,4);
    std::cout<<ffp<<std::endl; // 1
    std::cout<<reinterpret_cast<long long>(ffp)<<std::endl; // 等于下面的16进制的 10进制值
    std::cout<<reinterpret_cast<void*>(ffp)<<std::endl;

    ffp = &minus;
    ffp(5,2);
    std::cout<<reinterpret_cast<void*>(ffp)<<std::endl;

    ////////////////
    
    A a;
    int (*ffp2)(int, int) = a.pub_fun(555);
    std::cout<<ffp2(4, 1)<<std::endl;

    ////////////////

    int (*ffparr[])(int, int) = {add, minus};
    ffparr[0](1,2);
    ffparr[1](4,2);

    ////////////////

    ffp = ffpadd();
    ffp(22,33);

    //int (*ffpp)() = ffpadd;

    void (*ffp3)() = f;

    //int (*ffpp4)(int, int) = ffpadd;

    auto ffpp4 = ffpadd;

    int (* (*aaaa)() )(int, int) = ffpadd;  // 光标移动到上面的 auto，靠插件推导出 auto的值。然后写出来的。。只需要修改 ffp，
    
    ffpp4()(2,3);
    aaaa()(11,33333);
    
    return 0;
}


