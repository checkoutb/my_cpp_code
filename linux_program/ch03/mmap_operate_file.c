
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct {
    int integer;
    char string[24];
} RECORD;

#define NRECORDS (10)

/*
这里出了大问题：在执行 43改为143 的时候，发生了 Segmentation fault (core dumped)。
然后发现可以通过 gdb 查看 core文件。
但是我没有在 目录下发现 core文件。

`/proc/sys/kernel` 目录下有一个 core_pattern 
[qwer@192 kernel]$ cat core_pattern
|/usr/lib/systemd/systemd-coredump %P %u %g %s %t %c %h

这个位置是 core dump 的地址。 。。 这个不是，这个是 软件。。艹。

最开始的时候是认为没有生成core，所以查看了 ulimit -a， 无限的。
并且生成 core文件 需要 gcc 的时候 -g 。
。。但是第一次的时候没有 -g 也报出了这个错误。
主要是 后来才发现 core dumped， 就是说明 core 已经 dump 了。 所以第一次的时候应该也有 dump 文件。


https://www.epubit.com/articleDetails?id=Nb5fe85e1-3a32-4809-9ae4-84bb29c979a4
fedoraproject.coredumpctl

fedora 24以后， 默认 不生成 core文件。
使用 coredumpctl 这个工具，更加强。

`coredumpctl gdb`  调试最近一次 coredump，并进入 gdb 调试模式。

其他程序的调试，可以通过 `coredumpctl gdb pid` 来进行调试。

要生成core dump 的文件的话：
1. sysctl kernel.core_pattern="|/usr/libexec/abrt-hook-ccpp %s %c %p %u %g %t %P %I” 
2. vi /etc/abrt/plugins/CCpp.conf 修改MakeCompatCore = yes 
3. systemctl stop abrt-journal-core 
4. systemctl start abrt-ccpp
。。上面的4步， 我没有执行。


coredumpctl gdb
后进入了 gdb的界面，可以看到是 fseek 的问题。
并且看到了 目录： /var/lib/systemd/coredump 。里面有7,8个dump 了。。都是今天的。

gdb 的命令： ru， bt 。。 不知道干什么的。 2个都执行了下， 都是显示 fseek 的问题。
。。但是 fseek 为什么报错？？？？

报的错是 SIGSEGV ， 内存问题。

printf 了 fp， 是 0。。。
艹， records.dat,  record.dat 。。

并且试了下， gcc 不带 -g , 也可以 coredumpctl 的。

把 /var/lib/systemd/coredump 中的文件全部 rm 后，  
coredumpctl gcc 可以启动，会报错，xxxxxxxx 文件不可读， no such file.

*/


int main()
{
    RECORD record, *mapped;
    int i, f;
    FILE *fp;


    // 初始化 record.dat 。。。不过里面怎么有乱码。。应该是空位
    // fp = fopen("records.dat", "w+");
    // for (i = 0; i < NRECORDS; ++i)
    // {
    //     record.integer = i;
    //     sprintf(record.string, "record-%d", i);
    //     fwrite(&record, sizeof(record), 1, fp);
    // }
    // fclose(fp);




    // // 将第43条记录中的整数值 由43改为143。并写入到第43条记录中。
    // // 这里缩水了， 把4改成143。  ok的。
    // fp = fopen("records.dat", "r+");
    // printf("fpppppp : %d \n", fp);
    // fseek(fp, 4 * sizeof(record), SEEK_SET);
    // fread(&record, sizeof(record), 1, fp);          // 这个可以 序列化？
    // record.integer = 143;
    // sprintf(record.string, "RECORD-%d", record.integer);
    // fseek(fp, 4 * sizeof(record), SEEK_SET);
    // fwrite(&record, sizeof(record), 1, fp);
    // fclose(fp);




    // 把records 映射到 内存中，然后访问 并修改。
    f = open("records.dat", O_RDWR);
    mapped = (RECORD *) mmap(0, NRECORDS * sizeof(record), PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
    mapped[3].integer = 243;
    sprintf(mapped[3].string, "RECORD-%d", mapped[3].integer);
    
    msync((void *) mapped, NRECORDS * sizeof(record), MS_ASYNC);
    munmap((void *) mapped, NRECORDS * sizeof(record));
    close(f);

    exit(0);
}
