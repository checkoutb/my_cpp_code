
#include <iostream>

// 0. 进入同级的build目录
// 1. cmake ../
// 2. cmake --build .
// 3. ./add 111 333

// 刚开始的时候，想使用 /src 同级的 /build 目录，但是会报错。

// add_executable(add main.cpp) 中的 main.cpp 必须提供一个 main 方法作为入口。

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout<<"error too less arguments"<<std::endl;
    }
    std::cout<<argv[0]<<", "<<argv[1]<<", "<<argv[2]<<std::endl;
    int a = atof(argv[1]);
    int b = atof(argv[2]);
    std::cout<<"a: "<<a<<", b: "<<b<<", a+b: "<<(a + b)<<std::endl;
    return 0;
}
