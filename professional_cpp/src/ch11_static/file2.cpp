
import <iostream>;      // 不在module 中 也可以


// namespace {      // 匿名命名空间，也会导致  f() 是内部链接， 导致 file1 无法找到
                // 需要内部链接， 建议使用 匿名命名空间


// static
void f();    // static后， file1 无法找到 f() 。。  无法找到的场景： static声明+定义无所谓， 无声明+static定义
                        // 不带static的声明 + static定义，会报错：void f()’先被声明为‘extern’后又被声明为‘static’

// static
void f()
{
    std::cout<<"ffff\n";
}

// }

// int main()
// {
//     f();     // 匿名空间，ok
// }


// ---------
// const 和 typedef 默认内部链接， 所以 extern 可能有用。

// extern int x;       // 只是声明，没有分配内存
// int x { 3 };     // 分配内存

// extern int x {33};       // 声明+定义， 会有 编译器 警告。

int x { 333 };      // 也可以，因为 默认是 extern 的

// 不建议使用全局变量，令人困惑 且容易出错。

// ----------





