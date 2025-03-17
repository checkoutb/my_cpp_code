


select, poll, epoll


借助 fork 往 fd中传数据，传数据得用 socket啊。  en, socket成功时 返回 fd。


所以 先建立一个socket， 然后 fork， 子进程 往 socket里写？ 不对啊， 应该是 子进程 也建立一个socket 和 之前的socket 建立连接，然后 往自己的socket里 发送数据？


fd， 既然 everything is file， 那么 我打开一个文件， 然后 fork， 这个也可以把？  话说 这也是 IPC啊。





碰到的问题：
- 怎么通过 FILE* 找到 fd.
  - man FILE 有东西，但是没有我想要的。   我估计 fd 是 FILE里一个字段，但是不知道是哪个。
    - 问了AI，告诉我 FILE中没有直接暴露 fd， 要用`fileno()`。
- 新进程 是直接使用 fp 来写入，还是 再fopen一下？  试下 fopen。  。。子进程中 fopen的 fd2 和 之前 的fd 是相同的。  不过 子进程是 w 打开。
- 2各进程 是不是 都要 flose fp？ 应该要吧， 子进程 复制了所有的fd啊。
- 我应该用 read/write 直接读写 fd ？ 而不是 fwrite
- 搞不懂，select 立刻返回了。 文件空的时候 read 了0个字符。。  估计是文件的问题， 在 select之前 read一次，然后 再select, 并且timeout设置为NULL，还是 秒回。
  - AI: Unix-like 系统中，普通文件总被认为 可读可写，所以 select不会阻塞等待。
- accept 和 select 需要分开 2个线程处理。  但是 server 怎么把 ac的 fd 发给 select的线程？   不， ac 和 select 可以一起。只不过要分辨下。
- server 怎么感知 client关闭呢？ 应该也是一个 关闭请求， 怎么区别 关闭连接请求 和 普通的数据传输请求？    读操作 返回0  就是 对方已关闭。
- 怎么保存select所需要的 fd？ 似乎没有好办法。
- 我没有处理 recv返回0 的情况，导致 返回0的 fd依然会 select， select会立刻返回。循环一下子消耗光了。
  - 还是不对， recv返回值为0，将fd从 arr中剔除，但是 这个 fd没有复用啊。
    - 没有close(fd)。。所以无法复用。 不过不清楚 server需要 shutdown吗？ 应该不需要吧？
- poll 时， 怎么把 fd 失效？ 就是 client关闭后，怎么把 fd失效掉？ 修改指针数组有点烦， 直接fd设置为-1 会发生什么？  不过我这里试不到。 0把，标准输入，稍微好点。
  - 0，会一直循环。 poll无法停止。
  - 还是得自己追踪。



其他
- setsockopt, 没有使用
- tv.tv_sec 需要放循环里面，因为select 会修改这个值。


。。以前写过。。 `../socket/`  .这个应该是复制的。



第二天，epoll 还是挺顺利的， 当然，很多还是得抄，但是知道这里缺什么。


还缺很多 精细的控制， 方法接受很多 控制参数， 我基本没有使用。  阻塞非阻塞等(fnctl)。

