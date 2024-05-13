// #include <benchmark/benchmark.h>
//
// static void BM_StringCreation(benchmark::State& state) {
//   for (auto _ : state)
//     std::string empty_string;
// }
// // Register the function as a benchmark
// BENCHMARK(BM_StringCreation);
//
// // Define another benchmark
// static void BM_StringCopy(benchmark::State& state) {
//   std::string x = "hello";
//   for (auto _ : state)
//     std::string copy(x);
// }
// BENCHMARK(BM_StringCopy);
//
// BENCHMARK_MAIN();






// Microbenchmarks for string comparison using Google benchmark
#include <cstdlib>
#include <cstring>
#include <memory>

#include "benchmark/benchmark.h"

using std::unique_ptr;

bool compare_int(const char* s1, const char* s2) {
    char c1, c2;
    for (int i1 = 0, i2 = 0; ; ++i1, ++i2) {
        c1 = s1[i1]; c2 = s2[i2];
        if (c1 != c2) return c1 > c2;
    }
}

void BM_loop_int(benchmark::State& state) {
    const unsigned int N = state.range(0);
    unique_ptr<char[]> s(new char[2*N]);
    ::memset(s.get(), 'a', 2*N*sizeof(char));
    s[2*N-1] = 0;
    const char* s1 = s.get(), *s2 = s1 + N;
    for (auto _ : state) {
        benchmark::DoNotOptimize(compare_int(s1, s2));
    }
    state.SetItemsProcessed(N*state.iterations());
}

#define ARGS \
    ->Arg(1<<20)

BENCHMARK(BM_loop_int) ARGS;

BENCHMARK_MAIN();




/*

https://github.com/google/benchmark



`g++ 02_benckmark.cpp -std=c++17 -isystem benchmark-1.8.3/include/ -L benchmark-1.8.3/build/src/ -l benchmark -l pthread -o a.out`


`./a.out`


```text
2024-05-06T18:45:42+08:00
Running ./a.out
Run on (12 X 3600 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 512 KiB (x6)
  L3 Unified 16384 KiB (x2)
Load Average: 0.17, 0.16, 0.11
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
BM_StringCreation       4.65 ns         4.64 ns    150677740
BM_StringCopy           9.12 ns         9.10 ns     78584466
```


---
---



////////////////////////////////////////



`g++ 02_benckmark.cpp -std=c++20 -g -Wall -mavx2 -pedantic -isystem benchmark-1.8.3/include/ -L benchmark-1.8.3/build/src/ -l benchmark -l pthread -o a.out`
`g++ 02_benckmark.cpp -std=c++20 -g -O3 -Wall -mavx2 -pedantic -isystem benchmark-1.8.3/include/ -L benchmark-1.8.3/build/src/ -l benchmark -l pthread -o a.out`



`./a.out --benchmark_repetitions=5 --benchmark_report_aggregates_only=true`


O3 可以加速 8.5 倍

no -O3
```text
-------------------------------------------------------------------------------------
Benchmark                           Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------
BM_loop_int/1048576_mean      2210759 ns      2203938 ns            5 items_per_second=475.843M/s
BM_loop_int/1048576_median    2223801 ns      2216941 ns            5 items_per_second=472.983M/s
BM_loop_int/1048576_stddev      29546 ns        29431 ns            5 items_per_second=6.47021M/s
BM_loop_int/1048576_cv           1.34 %          1.34 %             5 items_per_second=1.36%
```

-O3
```text
-------------------------------------------------------------------------------------
Benchmark                           Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------
BM_loop_int/1048576_mean       259783 ns       258579 ns            5 items_per_second=4.05515G/s
BM_loop_int/1048576_median     259775 ns       258562 ns            5 items_per_second=4.05542G/s
BM_loop_int/1048576_stddev        238 ns          272 ns            5 items_per_second=4.27085M/s
BM_loop_int/1048576_cv           0.09 %          0.11 %             5 items_per_second=0.11%
```

`O3 + c++11` 和 `O3 + c++20` 一样的速度`

697616  c++11 + O3, a.out的大小
704448  c++11 + no O3, a.out的大小


CPU mean
C++20 + O1 : 516922 ns
C++20 + O2 : 260056 ns


不优化 : O1 = 4:1
O1 : O2 = 2:1
02 : 03 = 1/1


电脑支持 avx2 指令集的，但是 加不加 -mavx2 都一个速度
AVX2主要用于增强处理器在处理浮点和整数运算方面的性能，特别是在涉及大量数据时。它广泛应用于高性能计算任务、图形处理、科学计算和机器学习等领域。
。。好吧，这里的代码是 进行 c字符串的比较，不涉及 数值运算。

https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html



*/
