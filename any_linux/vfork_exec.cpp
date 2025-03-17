
#include <unistd.h>
#include <stdio.h>


/*


gcc -g xxx.cpp

cat /proc/sys/kernel/core_pattern  ..but useless
coredump's dir: /var/lib/systemd/coredump 。这个是复制上面的文件的内容，然后 再加上 会将核心转储文件存储到哪里， 问AI的。   实际上应该问 systemd-coredump(这个就是 core_pattern 中的) 默认将核心转储文件存放在哪里


..zst file...  sudo dnf install zstd


zstd a.zst     is compress.
zstd -d a.zst   uncompress
  15k -> 400k

file a
   core: ELF 64-bit LSB core file, x86-64, version 1 (SYSV), SVR4-style, from './a.out', real uid: 1000, effective uid: 1000, real gid: 1000, effective gid: 1000, execfn: './a.out', platform: 'x86_64'

file a.zst
   core.zst: Zstandard compressed data (v0.8+), Dictionary ID: None

enable debuginfod for this session, choose yes, then gdb download /lib64/libc.so.6
...Missing debuginfo, try: dnf debuginfo-install glibc-2.40-17.fc41.x86_64   ..no


Program terminated with signal SIGSEGV, Segmentation fault.
#0  0x0000000000000000 in ?? ()


。。看到说 vfork 父子进程共享内存，还觉得功能很不错，结果，共享是共享的，但是只要子进程修改了，那么 父进程就 SIGSEGV 。。。
。子进程必须 _exit(xx) 退出， 不能正常返回，因为你正常返回后 的值 就会 修改 父进程的数据。
。而且父进程会等待子进程的结束。 所以 感觉 vfork确实 毫无意义。


vfork 主要用于子进程几乎立刻就会调用 exec() 来替换自己的映像，或者子进程非常简单且很快就会退出的情况。由于其特殊的内存共享机制，不推荐用于复杂的子进程逻辑，以避免潜在的数据损坏风险。

由于fork的 COW，所以 fork 优于 vfork。




------------------


exec系列主要有
- execl, 使用路径名指定要执行的文件，并以列表形式传递参数（必须以NULL结尾）。
- execlp, 类似于execl，但是允许通过文件名而不是完整路径来指定可执行文件，它会在PATH环境变量所列出的目录中搜索该文件。
- execle, 与execl类似，但在调用时可以额外指定一个环境变量数组，用来覆盖默认的环境变量。
- execv, 接受一个完整的路径名作为第一个参数，并且第二个参数是一个指向字符串数组的指针，这个数组包含了传递给新程序的所有参数，最后一个元素必须是NULL。
- execvp, 类似于execv，但它可以根据PATH环境变量搜索可执行文件，而不是要求提供完整路径。
- execve, 与execv相似，但允许指定环境变量，就像execle那样。

如果你知道 程序的 确切路径，使用 execl execv
如果希望通过 PATH 变量查找 程序，使用 execlp execvp
如果需要 控制 传递给 新程序的 环境变量，使用 execle execve

返回值，表示出错，检查 errno


*/
int fun01() {
    int pid = vfork();

    // int a = 34;
    
    if (pid == 0) {

        sleep(5);
        //a = 112233;

        execlp("echo", "echo", "hello from child process", (char*)0);
        
        printf("child over\n");

        _exit(21);
        // return 52;
    } else {
        printf("parent will wait for child over or exec\n");

        // printf("parent's a is %d.\n", a);

        return 0;
    }
}


int main(int argc, char **argv) {

    int res = fun01();
    
    return res;
}
