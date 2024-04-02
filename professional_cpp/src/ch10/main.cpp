
export module main;

import derived;
import base;

import <iostream>;

class Test {
    virtual int getId() { return 11; }
public:     // 。。。没写public，导致： Test2::Test2()’ is implicitly deleted because the default definition would be ill-formed:     // 百思不得其解。。
    virtual ~Test() = default;
    virtual base::Base* fun1() {
        std::cout<<"return base\n";
        return nullptr;
    }
    virtual base::Base* fun1(int i) {
        return nullptr;
    }
    Test() = default;
    Test(int a) { }

    void commonAlgo() {
        std::cout<<"test.commonAlgo. "<<getId()<<"\n";
    }

    virtual void defaultArg(int a = 11) {
        std::cout<<"test.default.arg."<<a<<"\n";
    }
};
class Test2 : public Test {
    int getId() override { return 22; }
public:
    // virtual ~Test2() = default;
    derived::Derived* fun1() override {
    // Test* fun1() override {         // cannot convert ‘Test*’ to ‘base::Base@base*’ in assignment
        std::cout<<"return derived\n";
        return nullptr;
    }
    // p304 有个多重继承的例子，当2个基类都有 相同的形参时，子类2个using，会导致 二异性，必须 子类自己定义 相同形参的方法 来隐藏 2个父类中的构造器
    // 310, 可以修改继承的方法的 访问级别。 但是可以通过 父类& 来跳过这个限制。

    void defaultArg(
        int a = 22      // 这里没有 = 22 的话，下面的 方法必须提供 参数，而不是从父类 继承 11
    ) override {
        std::cout<<"test2.default.arg "<<a<<"\n";
    }

    using Test::Test;
    Test2() = default;
    Test2(base::Base b) : Test(1) { }

    using Test::fun1;

    Test2& operator=(const Test2& rhs)
    {
        // 一般使用 Test2 tmp {rhs}; swap(tmp, *this); return *this;

        if (&rhs == this)
            return *this;

        Test::operator=(rhs);
        return *this;
    }
};


class Animal
{
public:
    virtual void eat() = 0;
    virtual void sleep() { std::cout<<"zzzzz...\n"; }
};
class Dog : public virtual Animal       // Animal is virtual base class, Both(Dog,Bird) need virtual
{
public:
    void eat() override { std::cout<<"dog eat\n"; }
    // void sleep() override { std::cout<<"ddd...\n"; }         // if Both(Dog,Bird) are override sleep(), compile error
};
class Bird : public virtual Animal
{
public:
    void eat() override { std::cout<<"bird eat\n"; }
    void sleep() override { std::cout<<"bbbb...\n"; }       // DogBird use this sleep(), if delete this fun, DogBird use Animal.sleep()
};
class DogBird: public Dog, public Bird
{
public:
    void eat() override { Dog::eat(); }
};


int main()
{

    // 1. g++ -std=c++20 -fmodules-ts -xc++-system-header iostream
    // 2. g++ -fmodules-ts -std=c++20 base.cc derived.cc base.cpp derived.cpp main.cpp
    // 3. ./a.out

    // 好像挺好用的，唯一的问题时，kDevelop对C++20的支持不够。反正全是报错。
    // 不过 .h 应该也很快。毕竟简单demo

    derived::Derived d;
    d.someMethod();
    d.someOtherMethod();
    d.noVgo();  // d
    d.noVgo(1);      // no match function       // base. // in Derived class: using base::Base::noVgo;

    // d.multiBase();       // 对成员‘multiBase’的请求有歧义
    dynamic_cast<base::Base2&>(d).multiBase();      // base2    // need base::
                    // after add multiBase to Derived, it will invoke base.multiBase. not base2
                        // after using Base::multiBase (no multiBase fun);  base2
                // using + multiBase,  base
    d.Base2::multiBase();        // base2     // don't need base::
            // after using Base::multiBase;  base2
                    // after Derived.multiBase, it still base2
            // using + multiBase, base2

    d.multiBase();

    base::Base* b { new derived::Derived{} };
    b->someMethod();        // derived
    b->noVgo();     // base

    base::Base b2 { d };
    b2.someMethod();    // base

    // Test2 t3;
    // Test* t = &t3;
    Test* t {new Test2{}};
    Test2* t2 = dynamic_cast<Test2*>(t);        // cannot ‘dynamic_cast’ ‘t’ (of type ‘class Test*’) to type ‘class Test2*’ (源类型不是多态的)
                // 基类 加一个虚函数就可以了。 子类加不加虚函数无所谓。

    auto a = t->fun1();  // derived
    a = t2->fun1();     // derived
    a = Test().fun1();  // base

    auto a2 = Test(2);
    auto a3 = Test2(2);      // no match fun:  Test2::Test2(int)’       // using Test::Test;

    a2.fun1(1);
    a3.fun1(2);     // no match fun: Test2::fun1(int)    // 可以使用 using Test::fun1; 来显式声明
    Test& a4 = a3;
    a4.fun1(2);     // no using, ok

    a2.commonAlgo();        // 11
    a3.commonAlgo();        // 22
    a4.commonAlgo();        // 22   // 多态

    a3.defaultArg();    // 22
    a4.defaultArg();    // 11       // 根据 编译期类型 绑定默认参数。

    std::cout<<typeid(a3).name()<<" - "<<typeid(a4).name()<<"\n";
    std::cout<<(typeid(a3) == typeid(a4))<<"\n";        // 1,true

    DogBird db;
    db.sleep();
    db.eat();

    auto ss = d.getName();      // 没有导入 <string>    .. 照书上的说法是， auto 可以编译， string 不可以编译，因为没有导入 string。
    std::string s2 = d.getName();           // 但是现在看起来 string 被导入了， 不知道被谁导入的。
    std::cout<<ss<<s2<<"\n";             // 而且，我把 Derived的 import <string> 删除后，这里没有问题。
    std::string aff = "qqasd";      // ...尴尬了， 我不知道怎么会把 <string> 导入进来的。。 确实执行过 -xc++-system-header string。
    std::cout<<aff<<"\n";               // 但是，我把main.cpp 中的 import <iostream> 删除后，编译就失败了。 说明不是默认导入的。(也不可能默认导入)
                // 新建了一个cpp，试了下，找不到 string。
                // 感觉可能是 cache。。。没用， 删除整个gcm.cache， 只system-header iostream， main.cpp可以。 test_string.cpp 不能。
                // rm -rf gcm.cache/
                // g++ -std=c++20 -fmodules-ts -xc++-system-header iostream
                // g++ -fmodules-ts -std=c++20 base.cc derived.cc base.cpp derived.cpp test_string.cpp      // 这个报错，找不到string
                // 把上面的 test_string.cpp 换成 main.cpp， 编译ok，执行ok。
        // ??????????
        // 不知道能不能稳定复现， 估计不行。 我看到上面的 typeid().name()， 这个应该是 string。
        // ...test_string 需要导入 <typeinfo>， 我这里没有导入啊，也可以用 typeid。。。
        //   test_string 中导入 typeinfo后， 还是报错 string 找不到。
        // ...test_string 可以了， 导入 <iostream> 就有了 typeinfo， string。。。  import 和 include 都可以。
        // 看了下 iostream， 没有明显的痕迹。。 估计是 #include <bits/c++config.h> 里的。 里面有用到 inline namespace string_literals { }


    return 0;
}

