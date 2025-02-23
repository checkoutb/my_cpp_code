#include <unistd.h>
#include <stdlib.h>

int main()
{
  if ((write(1, "here is some data\n", 18)) != 18)
    write(2, "error occur", 12);    // 原文的string很长，估计就是46个字符。

// 用46 报错了。
// write_system_call.c:7:5: warning: ‘write’ reading 46 bytes from a region of size 12 [-Wstringop-overread]
//     7 |     write(2, "error occur", 46);    // 原文的string很长，估计就是46个字符。

  exit(0);
}
