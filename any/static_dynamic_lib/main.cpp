
#include <iostream>

#include "header/api.h"

using std::cout;
using std::endl;
int main()
{
    std::cout<<"1+2="<<plus(1, 2)<<std::endl;
    cout<<"1-2="<<minus(1, 2)<<endl;

    return 0;
}


/*

静态: linux: .a, windows: .lib
动态: linux: .so, windows: .dll


*/

/*
静态库

---

OpenGL/learn

g++ glad.c -c -I../include    // compile
ar rcs libmyglad.a glad.o     // create .so

g++ -c my_shader.cpp -I/xxxx/include -lmyglad -L./    // compile
ar rs libmyglad.a my_shader.o                         // (r) insert into .so  and  (s) flush symbol table

---



生成

my_cpp_code/any/static_dynamic_lib$ g++ -c plus.cpp
my_cpp_code/any/static_dynamic_lib$ ls
api.h  main.cpp  minus.cpp  plus.cpp  plus.o

my_cpp_code/any/static_dynamic_lib$ g++ -c minus.cpp -o minus.o
        // header文件被移动后 要 `g++ -c minus.cpp -I header/`

my_cpp_code/any/static_dynamic_lib$ ls
api.h  main.cpp  minus.cpp  minus.o  plus.cpp  plus.o
my_cpp_code/any/static_dynamic_lib$ ar rcs libapi.a minus.o plus.o
my_cpp_code/any/static_dynamic_lib$ ls
api.h  libapi.a  main.cpp  minus.cpp  minus.o  plus.cpp  plus.o

。。ac rcs:
- c  create archive
- r  insert file into archive
- s  write object-file index into archive

`ar vt slib/libapi.a`
rwxrwxrwx 0/0   1256 Apr 30 10:31 2024 minus.o
rwxrwxrwx 0/0   1256 Apr 30 10:30 2024 plus.o
查看内容

`ar -x libapi.a`
解压



-----

$ g++ -static main.cpp -o s_main -L./ -L/usr/lib64/ -lapi -I./
/usr/bin/ld: 找不到 -lstdc++: 没有那个文件或目录
/usr/bin/ld: 找不到 -lm: 没有那个文件或目录
/usr/bin/ld: 找不到 -lc: 没有那个文件或目录

---

。。不理解了，按照网上的说法， -static 用于链接静态库，
但是 加上后，会报上面的错误。  无法解决。

把 -static去掉后， 可以编译，但是 执行会失败，说 找不到 libapi.so， 所以 确实 走了 动态链接库。
我把 libapi.so 删除后 (之前是 libapi.so 和 libapi.a 都在一个 main.cpp 的目录下)， 不带 -static 是可以的。 所以 后续 调整了下 目录结构。

所以 我没有带上 -static， 但是依然可以用 .a 编译成功，运行，  所以现在是 静态 还是 动态 链接？
。应该是静态的， 静态动态的区别，应该就是 代码是否包含在 可执行文件中， 我把 静态库的 名字 改了， 依然 可以运行。

。我看视频上， 动态链接的 可执行文件  和 静态链接的可执行文件 的 文件大小 相差非常大 (100倍的差距，13k vs 2mb)。 但是 我这里，只差了 几十个byte。。 不过 视频是 vs code 的，可能 这个环境会打包很多额外的东西

这里就是 静态链接 ，但是不知道为什么 带上 -static 就报错， 不带就可以。。
视频上，执行 动态链接的 可执行文件 报错了， 我的 动态链接的可执行文件 也是 同样的错，就是 找不到 那个 共享/动态 库


唯一的问题就是 那个 -static 了。 无法理解。

https://zhuanlan.zhihu.com/p/614380458  这里确实没有用 -static


https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html#index-static

-static
On systems that support dynamic linking, this overrides -pie and prevents linking with the shared libraries. On other systems, this option has no effect.


-pie
Produce a dynamically linked position independent executable on targets that support it. For predictable results, you must also specify the same set of options used for compilation (-fpie, -fPIE, or model suboptions) when you specify this linker option.


需要static
https://blog.csdn.net/weixin_44713496/article/details/132769186


不需要static
https://blog.csdn.net/chongbin007/article/details/123962714
https://www.geeksforgeeks.org/how-do-i-create-a-library-in-cpp/
https://www.geeksforgeeks.org/how-to-create-static-library-in-cpp/
https://iq.opengenus.org/create-archive-library-in-cpp/

---


my_cpp_code/any/static_dynamic_lib1$ g++ main.cpp -o smain -I ./header -L ./slib -l api
my_cpp_code/any/static_dynamic_lib$ ./smain
1+2=3
1-2=-1
my_cpp_code/any/static_dynamic_lib$ ls
header  main.cpp  minus.cpp  minus.o  plus.cpp  plus.o  slib  smain
my_cpp_code/any/static_dynamic_lib$ tree
.
├── header
│   └── api.h
├── main.cpp
├── minus.cpp
├── minus.o
├── plus.cpp
├── plus.o
├── slib
│   └── libapi.a
└── smain

3 directories, 8 files



*/


/*

共享/动态库

生成
`-fPIC`: 生成地址无关的代码，装载时定位

my_cpp_code/any/static_dynamic_lib$ g++ -shared -fPIC -o libdapi.so plus.o minus.o
。。这个可以 从 .cpp 文件开始，而不是 从.o， .cpp 更好
如果有全局变量的话，直接用 .o 会导致 报错： relocation xx  。。在 外面的 load_lib_at_runtime 中遇到了这个问题。 从 cpp开始就没有问题。
..但是这里，我在 plus.cpp 中 增加 全局变量， 没有问题啊。 可以 从 .o 生成。
。复现了，确实 不行。 有全局变量，且 在方法中 使用了 这个全局变量， 生成 .o 后 这里就会失败。
.. 也给出提示了 `recompile with -fPIC`  在生成 .o 的命令中 带上 -fPIC, 确实可以。  -fPIC ..知道了。



my_cpp_code/any/static_dynamic_lib$ g++ main.cpp -o dmain -L dlib/ -l dapi -I header/

my_cpp_code/any/static_dynamic_lib$ echo $LD_LIBRARY_PATH
// 确实空的
my_cpp_code/any/static_dynamic_lib$ export LD_LIBRARY_PATH=my_github/my_cpp_code/any/static_dynamic_lib/dlib/
my_cpp_code/any/static_dynamic_lib$ ./dmain
1+2=3
1-2=-1
my_cpp_code/any/static_dynamic_lib$ echo $LD_LIBRARY_PATH
my_github/my_cpp_code/any/static_dynamic_lib/dlib/


.
├── dlib
│   └── libdapi.so
├── dmain
├── header
│   └── api.h
├── main.cpp
├── minus.cpp
├── minus.o
├── plus.cpp
├── plus.o
├── slib
│   └── libapi.a.2          // 最后改回去了
└── smain


drwxrwxrwx. 1 root root   152  4月30日 11:51 dlib
-rwxrwxrwx. 1 root root 24200  4月30日 11:53 dmain            // 动态链接的可执行文件
drwxrwxrwx. 1 root root   144  4月30日 11:42 header
-rwxrwxrwx. 1 root root  4761  4月30日 12:06 main.cpp
-rwxrwxrwx. 1 root root    65  4月30日 10:26 minus.cpp
-rwxrwxrwx. 1 root root  1256  4月30日 10:31 minus.o
-rwxrwxrwx. 1 root root    64  4月30日 10:26 plus.cpp
-rwxrwxrwx. 1 root root  1256  4月30日 10:30 plus.o
drwxrwxrwx. 1 root root   152  4月30日 11:54 slib
-rwxrwxrwx. 1 root root 24248  4月30日 11:42 smain            // 静态链接的可执行文件


$ ll dlib/ slib/
dlib/:
总计 16
-rwxrwxrwx. 1 root root 15752  4月30日 11:50 libdapi.so

slib/:
总计 4
-rwxrwxrwx. 1 root root 2734  4月30日 10:32 libapi.a.2


这里动态库很大。  确实， 动态库 需要在 可执行文件启动时 加载，初始化一些东西(估计就是地址之类的)， 所以需要 额外的 代码。

静态库 就是 inline， 只需要 我的代码 就可以了， 直接添加到 可执行文件中， 不需要 额外的 代码。



*/



