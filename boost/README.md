



# getting startd guide

https://www.boost.org/doc/libs/1_87_0/more/getting_started/index.html

---

下载


1. 下载压缩包， https://www.boost.org/users/download/  下面的命令是 tar.bz2 的
2. 解压，先进入 你想放置boost的目录，然后执行 `tar --bzip2 -xf /path/to/boost_1_82_0.tar.bz2`
  注意：有些发行版，提供了boost，但是它们会将boost拆分为多个包。 所以建议直接使用官网的。

---

boost文件的结构

```text
boost_1_82_0/ .................The “boost root directory”
   index.htm .........A copy of www.boost.org starts here
   boost/ .........................All Boost Header files
   `` ``
   libs/ ............Tests, .cpps, docs, etc., by library
     index.html ........Library documentation starts here
     algorithm/
     any/
     array/
                     …more libraries…
   status/ .........................Boost-wide test suite
   tools/ ...........Utilities, e.g. Boost.Build, quickbook, bcp
   more/ ..........................Policy documents, etc.
   doc/ ...............A subset of all Boost library docs
```

1. 文档中出现的 `$BOOST_ROOT` 是指 上面的 boost root directory (通常是 `/usr/local/boost_1_82_0`)
2. 要使用boost，必须在代码中include `boost/xxx`   。。不应该从 boost_1_82_0开始？ 还是把 boost-1-82-0 作为一个 `-I` ？是的 `-I` 下面的例子中就是这样的
3. 所有的 boost 头文件都是 `.hpp` 后缀。且在 boost/ 目录中，所以只需要 `#include <boost/whatever.hpp>` 或 `#include "boost/whatever.hpp"`
4. doc中是文档， 如果要看文档，那么建议从 index.html 开始，而不是直接看doc/


---

header-only lib

boost的所有内容都放在 头文件中，所以不需要 link。 直接编译即可。

必须单独build 的是
- Boost.Chrono
- Boost.Context
- Boost.Filesystem
- Boost.GraphParallel
- Boost.IOStreams
- Boost.Locale
- Boost.Log (see build documentation)
- Boost.MPI
- Boost.ProgramOptions
- Boost.Python (see the Boost.Python build documentation before building and installing it)
- Boost.Regex
- Boost.Serialization
- Boost.Thread
- Boost.Timer
- Boost.Wave

一些库有可选的单独编译的二进制文件
- Boost.Graph
- Boost.Math
- Boost.Random
- Boost.Test
- Boost.Exception
- Boost.System

。。这里有点搞不懂。。 分开build怎么build，只有头文件啊。


---

## 使用boost的一个简单例子

```C++
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " " );
}
```

上面的内容复制到 example.cpp

然后执行下面的命令
`c++ -I path/to/boost_1_82_0 example.cpp -o example`

`echo 1 2 3 | ./example`

---


## 使用 boost 二进制库前的准备

5.1 build and install
```shell
cd path/to/boost_1_82_0
./bootstrap.sh --help
```

选择你的配置选项，然后 再次调用 bootstrap.sh, 这次不带 --help。
如果你 没有 `/usr/local/` 的 写权限，那么使用下面的命令，安装到其他地方。
`./bootstrap.sh --prefix=path/to/installation/prefix`
也可以使用`--show-libraries` 和 `--with-libraries=lib-name-list` 来 防止 编译全部 导致 长时间等待。

最后 `./b2 install` ,会将 boost的 lib目录中的 二进制文件 放到 上面的 prefix中。 你也会在这个 prefix中 找到一个 include/ 目录，包含了所有头文件。

---

这里还有一个 5.2 是上面的 进阶版，Build Custom Binaries
很麻烦。

---

5.3 预期的 build output

在build过程中，你会看到一些信息：
- boost lib 的配置
- build tool 的日志
- build action 的信息
- 编译期警告

---

5.4 build error

你能看到的错误信息 应该只有 IOStreams 库的 一些关于 zip，bzip2 的  错误信息。 如果 需要这些功能，就安装 libz 和 libbz2

其他的错误信息 必须注意。

如果无法找到 编译期 或 linker，那么配置 user-config.jam 。   这里有 链接。

---

## 将你的程序 link到 boost 库

我们使用下面的程序来展示 link到boost lib。
下面的代码 从email中 提取 主题行
使用了Boost.Regex库，它有一个 单独编译的 二进制组件

```C++
#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string line;
    boost::regex pat( "^Subject: (Re: |Aw: )*(.*)" );

    while (std::cin)
    {
        std::getline(std::cin, line);
        boost::smatch matches;
        if (boost::regex_match(line, matches, pat))
            std::cout << matches[2] << std::endl;
    }
}
```

link有2个主要的挑战
1. 工具的配置
2. 识别二进制库

有2种方法来link

下面的~/boost/stage/lib/ 和 libboost_regex-gcc34-mt-d-1_36.a 是 在之前构建的。 。。但是这里给的连接是 到 使用boost的一个简单例子(这里不会生成库啊。)， 而不是 到 使用二进制库前的准备。  。。难道说 会自动生成？ 因为看名字 确实是 regex的。

1. 指定 每个库的 全路径
    ```shell
c++ -I path/to/boost_1_82_0 example.cpp -o example ~/boost/stage/lib/libboost_regex-gcc34-mt-d-1_36.a
    ```
2. 通过 -L 指定搜索的目录 和 -l指定库名
    ```shell
c++ -I path/to/boost_1_82_0 example.cpp -o example -L~/boost/stage/lib/ -lboost_regex-gcc34-mt-d-1_36
    ```
    这个方法比上面的简洁很多， 但是==注意==：如果 link的库 同时有 静态库.a 和 动态库.so，那么 系统会自动选择一个，除非你 显式 `-staic`

---

### 库的命名规则

以 `libboost_regex-vc71-mt-d-x86-1_34.lib` 为例， 可以拆分为
- lib， 前缀， 在win上，只有传统的 static库 有这个前缀， import lib 和 DLL 没有。 其他系统，都有 lib前缀。
- boost_regex， 库名， boost所有库名 都以 boost_ 开头
- -vc71， toolset tag， 识别构建该二进制文件的 tool set 和 版本
- -mt， threading tag， 表示 支持 `多线程`。 没有就是不支持 多线程
- -d， ABI tag， 可能有 s(static)，g(debug)，y(python)，d(debug)，p(STLPort)  。。碰到再说。()中是自己加的
- -x86， architecture and address model tag， 第一个字符是架构，x: x86-32,x86-64, a: ARM, i: IA-64, s: Sparc, m: MIPS/SGI, p: RS/6000 & PowerPC。后面是32位或64位
- -1_34， version tag： boost release number。
- .lib，后缀。取决于OS， Unix系统中是 `.a` 和 `.so`, win: `.dll`代表共享库， `.lib`代表 静态库 或 import库。 部分 Unix toolset 还会==增加 版本号==，比如 `.so.1.34` 和一个 ==符号链接== 链接到 二进制文件，且 符号链接的名字 ==没有 版本号==。


---

测试你的程序

复制下面的内容，并保存为 `jayne.txt`
```text
To: George Shmidlap
From: Rita Marlowe
Subject: Will Success Spoil Rock Hunter?
---
See subject.
```

如果你使用 共享库，那么需要做一些操作 来让 程序能找到并加载 共享库。
大部分系统 都有一个 环境变量。 你需要==往这个变量中 增加 包含 动态库的 目录==
许多OS 都是 ==LD_LIBRARY_PATH==。  MacOS 是 `DYLD_LIBRARY_PATH`。  Cygwin是 `PATH`

除了 csh 和 tcsh 外的 所有shell都可以如下。
```shell
VARIABLE_NAME=path/to/lib/directory:${VARIABLE_NAME}
export VARIABLE_NAME
```
csh 和 tcsh ： `setenv VARIABLE_NAME path/to/lib/directory:${VARIABLE_NAME}`

执行代码
`path/to/compiled/example < path/to/jayne.txt`

代码应该显示 `Will Success Spoil Rock Hunter?`

---

更多资源

- Boost.Build reference manual
  https://www.boost.org/doc/libs/1_87_0/tools/build/doc/html/index.html
- Boost Users' mailing list
  https://www.boost.org/more/mailing_lists.htm#users
- Index of all Boost library documentation
  https://www.boost.org/doc/libs/1_87_0/libs/index.html





## Asio tutorial

https://www.boost.org/doc/libs/1_87_0/doc/html/boost_asio/tutorial.html







---


之前(查看commit，是2024-4-7提交)，使用kDevelop，现在不用了。而且差不多一年前了。所以 这个 boost文件夹 重新使用。 因此将之前的文件要删除掉，所以复制了之前唯一一个cpp文件的内容

```C++
// ok
// #include <boost/lambda/lambda.hpp>
// #include <iostream>
// #include <iterator>
// #include <algorithm>
//
// int main()
// {
//     using namespace boost::lambda;
//     typedef std::istream_iterator<int> in;
//
//     std::for_each(in(std::cin), in(), std::cout<<(_1 * 3)<<" ");
// }
//
//
// // g++ -I /usr/local/boost_1_84_0 main.cpp
// // echo 1 2 3 | ./a.out


// build后，就不需要 -I 了， 我看 kdevelop 能找到 boost的库了。


#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string line;
    boost::regex pat ("^Subject: (Re: |Aw: )*(.*)");
    while (std::cin)
    {
        std::getline(std::cin, line);
        boost::smatch matches;
        if (boost::regex_match(line, matches, pat))
            std::cout<<matches[2]<<std::endl;
        else
            std::cout<<"###"<<std::endl;
    }
}

// g++ main.cpp
// 可以，不过我不知道怎么输入 才能让它走第一个分支。。所以一直是 ###。。。
// kdevelop识别到了库，在 /usr/local/include/boost/ 中。 这个 include 只有 boost 一个文件夹。
// C++20的 regex 我都没有看。
```

