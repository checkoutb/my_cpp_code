
#include <iostream>

int main() 
{
    int i = 3;
    int j = 4;

    i += j;
    j = i * 2;

    // std::cout<<i<<", hi, "<<j<<std::endl;
    std::cout<<"hi"<<std::endl;
}


/*

编译器会优化代码
- 循环展开
- 编译期计算
- 重排序
- 消除死代码

导致我们无法看到执行的步骤。


下面的参数 让编译期不做任何优化，修改。 。。。应该是 产生 带调试信息的 可执行文件。
`g++ -g xxx.cpp`


## gdb命令


### 直接回车

运行上一条命令


### run (r)

```text
(gdb) run
Starting program: /my_github/my_cpp_code/any/gdb/zzz 

This GDB supports auto-downloading debuginfo from the following URLs:
  <https://debuginfod.fedoraproject.org/>
Enable debuginfod for this session? (y or [n]) n     /////////?????
Debuginfod has been disabled.
To make this setting permanent, add 'set debuginfod enabled off' to .gdbinit.
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".
hi
[Inferior 1 (process 16607) exited normally]
Missing separate debuginfos, use: dnf debuginfo-install glibc-2.38-7.fc39.x86_64 libgcc-13.2.1-4.fc39.x86_64 libstdc++-13.2.1-4.fc39.x86_64
```

`run arg1 arg2 < infile.in`
没太理解，估计是 参数 从文件里拿？



### break
breakpoint
`break 7`       第7行
`break main`   main方法

==打印的是 下一次将执行的 行==

```text
(gdb) break main
Breakpoint 1 at 0x40114e: file 01_basic.cpp, line 6.
(gdb) run
Starting program: /my_github/my_cpp_code/any/gdb/zzz 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib64/libthread_db.so.1".

Breakpoint 1, main () at 01_basic.cpp:6
warning: Source file is more recent than executable.
6	    int i = 3;
```

#### info breakpoints (i b)
显示所有 断点

info 有很多很多功能，直接 info 会显示。

```text
(gdb) info breakpoints
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000401281 in main() 
                                                   at 04_example.cpp:34
	breakpoint already hit 1 time               // -----------
3       hw watchpoint  keep y                      x
```

#### delete

```text
(gdb) info breakpoints
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000401281 in main() 
                                                   at 04_example.cpp:34
	breakpoint already hit 1 time
3       hw watchpoint  keep y                      x
(gdb) delete 3                          // ---------------------
(gdb) info breakpoints
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000401281 in main() 
                                                   at 04_example.cpp:34
	breakpoint already hit 1 time
```

```text
(gdb) delete
Delete all breakpoints? (y or n) n
(gdb) d
Delete all breakpoints? (y or n) n
```

。。感觉这的所有命令 都有缩写。。


#### next (简写n)
单步，不会进入方法

```text
6	    int i = 3;
(gdb) next
7	    int j = 4;
(gdb) 
```

#### step
单步，进入方法


#### continue (c)
执行到下一个断点


#### finish
执行，直到 退出本方法 时(销毁堆栈) 停止

```text
(gdb) finish        // ----------------
Run till exit from #0  0x00000000004011ab in factorial (n=@0x7fffffffdd1c: 1)
    at 03_recursive.cpp:8
0x00000000004011ab in factorial (n=@0x7fffffffdd5c: 2) at 03_recursive.cpp:8
8	        return n * factorial(n - 1);
Value returned is $2 = 1
(gdb) finish            // --------------------
Run till exit from #0  0x00000000004011ab in factorial (n=@0x7fffffffdd5c: 2)
    at 03_recursive.cpp:8
0x00000000004011ab in factorial (n=@0x7fffffffdd9c: 3) at 03_recursive.cpp:8
8	        return n * factorial(n - 1);
Value returned is $3 = 2
```


#### list (l)
显示当前行的 前后的代码，而不是只显示 一行

```text
(gdb) next
7	    int j = 4;
(gdb) list
2	#include <iostream>
3	
4	int main() 
5	{
6	    int i = 3;
7	    int j = 4;
8	
9	    i += j;
10	    j = i * 2;
```

##### list 25
展示 25行 上下的 代码。


#### print (p)

```text
(gdb) print i
$1 = 3
(gdb) print j           // 还没有执行 int j = 4;
$2 = -8424
(gdb) print a
No symbol "a" in current context.
```

```text
(gdb) p j
$3 = 4
(gdb) p j+i
$4 = 11
(gdb) p j+i+i+i
$5 = 25
```

##### print address
```text
(gdb) p &i
$6 = (int *) 0x7fffffffddec
(gdb) p &j
$7 = (int *) 0x7fffffffdde8
```


### quit (q)
离开 gdb

```text
(gdb) quit
A debugging session is active.

	Inferior 1 [process 16735] will be killed.

Quit anyway? (y or n) y
```




### up / down

调用栈的上一层 下一层

```text
Program received signal SIGSEGV, Segmentation fault.
0x0000000000401136 in crash (i=0x0) at 02_example.cpp:6
warning: Source file is more recent than executable.
6	    *i = 1;
Missing separate debuginfos, use: dnf debuginfo-install glibc-2.38-7.fc39.x86_64 libgcc-13.2.1-4.fc39.x86_64 libstdc++-13.2.1-4.fc39.x86_64
(gdb) up            ///////----------------
#1  0x000000000040117b in f (i=0x7fffffffdddc) at 02_example.cpp:15
15	    crash(j);
(gdb) down      ////////----------------
#0  0x0000000000401136 in crash (i=0x0) at 02_example.cpp:6
6	    *i = 1;
```

### backtrace (bt)
调用栈

```text
(gdb) backtrace
#0  0x0000000000401136 in crash (i=0x0) at 02_example.cpp:6
#1  0x000000000040117b in f (i=0x7fffffffdddc) at 02_example.cpp:15
#2  0x0000000000401192 in main () at 02_example.cpp:21
```



### display / undisplay

跟踪 j
undisplay 序号

```text
(gdb) p j
$2 = (int *) 0x7ffff7e423c0
(gdb) display j                 // --------
1: j = (int *) 0x7ffff7e423c0
(gdb) n
13	    sophisticated(j);
1: j = (int *) 0x7fffffffdddc
(gdb)                           // 直接 回车
14	    j = complicated(j);
1: j = (int *) 0x7fffffffdddc
(gdb) n
15	    crash(j);       // 将要执行的行
1: j = (int *) 0x0          // null pointer ， 监控的 变量
(gdb) undisplay i           // ---------
```



### watch
当变量的值 变化时，暂停

```text
(gdb) watch x       // -----------
Hardware watchpoint 2: x
(gdb) c         // ---
Continuing.

Hardware watchpoint 2: x

Old value = 10
New value = 30
bar (p=@0x7fffffffdddc: 30) at 04_example.cpp:14
14	    return unknown(p);
```


### whatis

```text
(gdb) whatis x
type = int
```




## 高级功能

### target record-full
让 gdn记录下所有， 以便 撤销/回滚/回退

可能无效。

#### reverse-next (rn)

#### reverse-step

#### reverse-continue



### set var

直接修改变量的值。

```text
(gdb) set var x=1234
(gdb) p x
$4 = 1234
```





*/



