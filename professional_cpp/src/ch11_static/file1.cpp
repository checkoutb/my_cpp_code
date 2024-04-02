
import <iostream>;

// 单个源文件中，按照 出现顺序初始化，
// 源文件之间的 初始化顺序 未定义。

void f();
void static_fun();
extern int x;       // file2 的x ，3。 想要使用其他文件的 x ，必须 extern。

// g++ -fmodules-ts -std=c++20 -xc++-system-header iostream
// g++ -fmodules-ts -std=c++20 file1.cpp file2.cpp
int main()
{
    f();

    std::cout<<x<<"\n";

    for (int i = 0; i < 10; ++i)
        static_fun();       // 1,2,3,...10
}


void static_fun()
{
    static int cnt = 0;     // 容易让人迷惑，通常有更好的方法。 有时十分有用，比如 Meyer 的单例设计模式，33章。
    ++cnt;
    std::cout<<"stati_fun: "<<cnt<<"\n";
}
