#include <unistd.h>
#include <stdlib.h>

int main()
{
  char buffer[128];
  int nread;

  nread = read(0, buffer, 12);  // 可以的，多余的变成了终端的命令。命令找不到
  if (nread == -1)
    write(2, "error occur", 12);
  if ((write(1, buffer, nread)) != nread)
    write(2, "errir", 6);
  exit(0);
}
