

// 7.6.2 不需要看。静态库 动态库 看 外面的 static_dynamic_lib
// #include <iostream>
// #include "vecop.h"

// int x[2] = {1, 2};
// int y[2] = {3, 4};
// int z[2];

// int main() {
//     addvec(x, y, z, 2);
//     std::cout<<z[0]<<", "<<z[1]<<std::endl;
//     return 0;
// }



// 7.11 load lib at runtime

#include <cstdlib>
#include <iostream>
#include <dlfcn.h>
//#include "vecop.h"

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2];

int main() {
    void *handle;
    void (*addvecp)(int *, int *, int *,int );
    char *error;

    handle = dlopen("./libopvec.so", RTLD_LAZY);
    if (!handle) {
        std::cerr<<dlerror()<<std::endl;
        exit(1);  // stdlib
    }

    // 1. 需要 reinterpret_cast。
    // 2. 方法名不对，运行时没有这个符号， 就是 c 和 cpp 的 符号规则不同， 至少 c 不会带上 参数。 cpp 必然带参数。
    // ..直接 `nm libopvec.so` 人 工 找到符号。 应该没有更好的方法了， 我们没有办法 影响 g++ 生成的 符号的名字吧。 但是这个名字(命名规则) 应该 在 gcc 的不同版本 中是同样的。
    //    -C 不行，这个是 把 符号 翻译回 方法名了，所以 可以看到 `00000000000010f9 T addvec(int*, int*, int*, int`
    //    -D,-g 都是 `00000000000010f9 T _Z6addvecPiS_S_i`, 而且比 不带参数的 nm，少了很多 项。
    addvecp = reinterpret_cast<decltype(addvecp)>(dlsym(handle, "_Z6addvecPiS_S_i"));
    if ((error = dlerror()) != NULL) {
        std::cerr<<error<<std::endl;
        exit(1);
    }

    addvecp(x, y, z, 2);
    std::cout<<z[0]<<", "<<z[1]<<std::endl;

    if (dlclose(handle) < 0) {
        std::cerr<<dlerror()<<std::endl;
        exit(1);
    }
    return 0;
}







/*

csapp

原文是 c 的，这里直接文件名改成 cpp 了，内容完全复制的。

7.6.2
addvec.cpp, multvec.cpp, main.cpp

`gcc -c addvec.cpp multvec.cpp`  -> addvec.o, multvec.o
`ar rcs libopvec.a addvec.o multvec.o`

`g++ -c main.cpp` 生成 main.o
`g++ main.o ./libopvec.a`   书上是 c 的： `gcc -static -main.o ./libopvec.a` ,
g++ 带上 -static 不行， ld 找不到东西，  没有试 gcc。

`./a.out`  ->   4,6

顺序很重要
`g++ ./libopvec.a main.o` 会失败，因为在 main.o 后面找不到 addvec


`objdump -dx main.o`


---

7.11  load lib at runtime

`g++ -fpic -o libopvec.so addvec.c `


从 .o 文件链接会有错误，因为2个 addcnt   。。 因为 生成 .o 的时候 没有
-fPIC，下面也有提示 recompile with -fPIC

`g++ -shared -fPIC -o libopvec.so addvec.o multvec.o`

/usr/bin/ld: addvec.o: warning: relocation against `addcnt' in read-only section
`.text' /usr/bin/ld: addvec.o: relocation R_X86_64_PC32 against symbol `addcnt'
can not be used when making a shared object; recompile with -fPIC
/usr/bin/ld:最后的链结失败: bad value
collect2: 错误：ld 返回 1


`g++ -shared -fPIC -o libopvec.so addvec.cpp multvec.cpp`

`g++ -rdynamic main.cpp  -ldl`

`./a.out`




*/

