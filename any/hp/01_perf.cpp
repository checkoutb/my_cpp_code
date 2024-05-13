
#include <iostream>
#include <vector>

#include <time.h>


// https://perf.wiki.kernel.org/index.php/Tutorial


void funa1(std::vector<int>& vi, int i)
{
    struct timespec ts;
    ts.tv_nsec = 5;
    ts.tv_sec = 0;
    // std::cout<<i<<", "<<ts.tv_sec<<std::endl;       // 是个随机long long。。
    // nanosleep(&ts, 0);       // 没用，不计入 perf report
    for (int a = 1; a < 1000; ++a)  // 计入
        ;
    vi.push_back(i % (i + 5));
}

int main()
{
    // std::vector<int> vi(1000, 0);
    std::vector<int> vi;

    for (int i = 0; i < 1000; ++i)
    {
        // if (i % 100 == 0)
        // {
        //     vi[i] = -123;
        // }
        // else
        {
            // vi[i] = 123;
        }
        funa1(vi, i);
    }

    return 0;
}


/*

`perf list`
所有的指标


`perf stat a.out`


`perf stat -e cache-references,cache-misses,branch-misses,branches,cpu-clock ./a.out`
不带 branched，不会告诉你 branch-misses 的百分比。


`perf stat -e cache-references,cache-misses,branch-misses,branches,cpu-clock,cycles,instructions ./a.out`
cycles, instructions, 一个CPU周期 执行多少条指令。


`perf record ./a.out`
`perf report`


`perf record -c 100 ./a.out`        // 每100个CPU周期 记录一次数据。 每秒多少样本 是 -F




`g++ -g -Wall -pedantic 01_perf.cpp`
g 产生调试信息
Wall 启用所有警告
pedantic 关闭g++的扩展(一些不符合ISO C++的东西)


*/

/*


`perf record -a --call-graph dwarf -p `ps aux | grep "xxx" | grep -v grep | cut -c 9-15` -d 1 -b`





















*/


