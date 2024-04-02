
import hello;


// https://gcc.gnu.org/wiki/cxx-modules
//
// g++ -fmodules-ts hello.cc main.cpp

// .. 被书上坑了， 书上都是 .cppm 来声明 module。
// 我实际试了下， .cppm 不行的。 failed to read compiled module。
// .cc, .cpp 都可以。

int main()
{
    print("world");
    return 0;
}
