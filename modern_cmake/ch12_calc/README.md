
modern cmake for c++

第一版，第12章。
https://cloud.tencent.com/developer/article/2421610



工程根目录：`cmake -B build/ -S .`  得用root用户，从 /目录 到 build/_deps/ftxtui-src 全是 root的， 但是我用自己帐号mkdir的啊。


致命错误：无法访问 'https://github.com/ArthurSonzogni/FTXUI.git/'：OpenSSL SSL_read: SSL_ERROR_SYSCALL, errno 0

`sudo cmake -B build/ -S .`
拼写错误满天飞。： configure -> config,  ftxui -> ftxio, bootstrap -> boostrap, static -> statis

`cd build`
`cmake --build .`

`/ch12_calc/build/bin/calc_console` 目录下 `./calc_console`
能用(指有终端界面)，
有bug(逻辑完全不知道是什么)
试了下：只有数字 和 backspace 是可以的，其他没有反应。
数字会 out_of_range 错误， 英文也会报错。

界面和 网页上的有一点点不同。
```text
[asdf@192 calc_console]$ ./calc_console 
╭──────────────────────────╮
│CalcConcole 1.0.0         │
│Built: 2024-10-29T11:00:16│
│SHA: '3a78d5e'            │
├──────────────────────────┤
│Sum: 102                  │
╰──────────────────────────╯
```
少了 input的框，而且 数组的数字 直接在 sum中显示了。
按照页面的图片，逻辑应该是，修改 input框里的内容， sum会自动变。


画的框都不对，估计是 ui 的代码抄错了。

。。少抄了 3行
```C++
       input_a->Render(),
       input_b->Render(),
       separator(),
```

ok了
```text
╭──────────────────────────╮
│CalcConcole 1.0.0         │
│Built: 2024-10-29T11:13:25│
│SHA: '3a78d5e'            │
├──────────────────────────┤
│23                        │
│100                       │
├──────────────────────────┤
│Sum: 123                  │
╰──────────────────────────╯
```
修改 上面的， sum自动改。 不过不能全删完，删完就 无法转为 int 了。

而且 命令行的界面好看很多。 ..没有截图工具。。
..有的， printscreen 全屏， ctrl+printscreen 自定义区域 。 自动保存到 Pictures中的，没有任何提示，不知道是 什么工具， 不是 gnome-screen，scrot。
还有个问题， 图片是 imv打开的，怎么关。。
..得kill 。。 `ps aux | grep imv`  `kill 1234`

---

第一部分(app能启动)完结


---

```
cmake --build <build-tree> -t coverage-calc_test
cmake --build <build-tree> -t coverage-calc_console_test
```



```
cmake --build <build-tree> -t memcheck-calc_test
cmake --build <build-tree> -t memcheck-calc_console_test
```


---

https://clang.llvm.org/docs/ClangFormat.html
可以直接装在 emacs里


`sudo dnf install clang-tools-extra` 里面有 clang-format
`clang-format -version`
`sudo dnf install lcov`
`sudo dnf install cppcheck`
`sudo dnf install valgrind`



之前根目录下 CMakeLists.txt 中 最后2行的 add test，install 被我注释了。
导致 -t 没有规则。
找了半天。






geninfo: ERROR: no .gcda files found in .




用了 作者的代码，也是 no .gcda ，估计是 工具版本的差异？

要用的话，直接复制 作者的代码吧。 照着写，不行，也不会。
https://github.com/PacktPublishing/Modern-CMake-for-Cpp
第12章的 01


g


