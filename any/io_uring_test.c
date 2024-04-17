



// 最开始是 .cpp ，然后用 g++, gcc 都是编译失败。  改成 .c 用 g++ 就可以了。 。我还以为 gcc 就是 把文件当作 c语言来编译， 也是看 后缀的。。



// https://man.archlinux.org/man/io_uring.7


// fedora 39 。。man 没有 io_uring 啊。
// man io_uring
// 没有 io_uring 的手册页条目


/*

https://man.archlinux.org/man/io_uring.7

sq: submission queue
cq: completion queue
sqe: sq entry
cqe: cq entry

1. io_uring_setup; mmap映射 SQ 和 CQ 到 用户空间 shared buffer

2. 当你要执行IO (读文件，写文件，socket操作) 时， 创建一个 SQE 来描述 IO操作， 把这个SQE 添加到 sq tail 。

3. 添加完后，调用 io_uring_enter 来告知 kernel 出队 SQE 并执行

4. 对于你提交的 每个SQE，一旦执行完成，kervel 会在 CQ tail 添加 CQE 。
    每个 SQE 都有 有且仅有一个 CQE
    你收到 CQE 后，你 需要检查 CQE 的 res 字段， 它保存了 系统调用的 返回值。你可以直接使用它， 不需要再 通过 io_uring
    举例来说： 通过 io_uring 进行 读操作 时， 以 IORING_OP_READ 操作 开始，等同于 read 系统调用。 实际上，如果有 偏移量的话，它 混合了 pread, preadv 的语义， -1的 偏移量值 代表 当前文件位置。

    io_uring 是 异步接口，所以 errno 没有用来 传递 错误信息。  使用 CQE 的 res 来传递， 如果成功，res是 对应系统调用的返回， 如果失败，res 是 -errno 。
        例如， 如果 普通的read 系统调用 返回 -1, 并设置 errno 为 EINVAL，那么 res 会是 -EINVAL。 如果 read成功，返回了 读到的字节数为 1024, 那么 res 是 1024。

5. 可选的，io_uring_enter 也可以 等待 指定数量的 request 被 kernel 处理完成后 再返回。 kernel 会至少放置 你指定的 数量 个 CQE 。你可以在 io_uring_enter 之后，直接 读。

6. 要记住，提交给 kernel 的 IO请求 可以 以任意顺序 完成。 你不能要求 kernel 在完成某个请求后 才能处理 另一个请求。
    当你 出队 CQE 的时候，你应该 总是 检查 这个CQE 对应的 请求。
    最常用的方法是 利用 SQE的 user_data 字段， 这个字段 会被 赋值到 CQE 上。


对于SQ，CQ
- 你在 SQ tail 上添加 SQE， kernel 在 头上读取
- kernel 在 CQ tail 上 添加 CQE，你在 头上读取



io_uring 支持 polling 模式， 这样 你就不需要调用  io_uring_enter 了。
通过 SQ Polling， io_uring 启动一根 内核线程 ，它会 poll 所有的IO请求。  启动SQ POlling 后，你就不需要 调用 io_uring_enter 了。 这样 减少了 系统调用的 开销。


还有一些 代码及注释，  使用io_uring的 代码片段,  枚举值的 说明

还有一个 超大的 EXAMPLE

...看代码的时候，才发现，还有 内存序。。怪不得 都说难用，然后 推荐 liburing..
编译期和CPU会 重排序，所以 在 SMP 系统上 要特别注意，因为 io_uring 在 内核 与 用户空间 间 共享了 buffer。 这些buffer 对于 内核 和 用户 都是 可见的，而且 都可以修改。 内核改头，用户改尾，或反过来。 这些修改 需要 在另一端 是 连贯地可见。 我们需要使用 内存屏障 来 强制这种 连贯性。


。现在 ASMP 不是主流。  SMP 主流，更快。


*/



// https://man.archlinux.org/man/io_uring.7
// The following example uses io_uring to copy stdin to stdout.



#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdatomic.h>
#include <linux/io_uring.h>

#define QUEUE_DEPTH 8
#define BLOCK_SZ    1024
/* Macros for barriers needed by io_uring */
#define io_uring_smp_store_release(p, v)            \
    atomic_store_explicit((_Atomic typeof(*(p)) *)(p), (v), \
                  memory_order_release)
#define io_uring_smp_load_acquire(p)                \
    atomic_load_explicit((_Atomic typeof(*(p)) *)(p),   \
                 memory_order_acquire)
int ring_fd;
unsigned *sring_tail, *sring_mask, *sring_array,
            *cring_head, *cring_tail, *cring_mask;
struct io_uring_sqe *sqes;
struct io_uring_cqe *cqes;
char buff[BLOCK_SZ];
off_t offset;


/*
 * System call wrappers provided since glibc does not yet
 * provide wrappers for io_uring system calls.
* */
int io_uring_setup(unsigned entries, struct io_uring_params *p)             // ? 这个不是 。。。 我以为 是 已经有的。。。
                                            // 这里一直说的 io_uring_setup 估计是 system call, 在 C 库 中没有提供，所以要自己写。
{
    return (int) syscall(__NR_io_uring_setup, entries, p);
}
int io_uring_enter(int ring_fd, unsigned int to_submit,
                   unsigned int min_complete, unsigned int flags)
{
    return (int) syscall(__NR_io_uring_enter, ring_fd, to_submit,
    			 min_complete, flags, NULL, 0);
}


int app_setup_uring(void) {
    struct io_uring_params p;
    void *sq_ptr, *cq_ptr;
    memset(&p, 0, sizeof(p));   // 查看 io_uring_setup(2) 的 io_uring_params ，来看 你可以设置的 flag。
                    // https://man.archlinux.org/man/io_uring_setup.2.en
                    // 确实有，The flags, sq_thread_cpu, and sq_thread_idle fields are used to configure the io_uring instance. flags is a bit mask of 0 or more of the following values ORed together:
    ring_fd = io_uring_setup(QUEUE_DEPTH, &p);      // QUEUE_DEPTH 文章中是1, 我改成8, 不知道有什么后果。
    if (ring_fd < 0)
    {
        perror("io_uring_setup");   // 用于显示与上一个系统调用相关联的错误描述。它是用于处理和显示来自errno变量的错误信息的
        return 1;
    }

    // io_uring 通过 2个 kernel-user 的 ring buffer 来通信， 这2个 buffer 可以 连在一起， 通过一个 mmap 来映射

    // 我加的，看下是不是等于 QUEUE_DEPTH
    printf("@app_setup_uring, p.sq_entries: %u\n", p.sq_entries);

    int sring_sz = p.sq_off.array + p.sq_entries * sizeof(unsigned);
    int cring_sz = p.cq_off.cqes + p.cq_entries * sizeof(struct io_uring_cqe);

    // check feature field, if IORING_FEAT_SINGLE_MMAP, 我们可以 只使用一个 mmap
    if (p.features & IORING_FEAT_SINGLE_MMAP)   // single map
    {
        if (cring_sz > sring_sz)
            sring_sz = cring_sz;
        cring_sz = sring_sz;
    }

    // . MAP_ANONYMOUS 就 不会和 文件关联。  现在和文件关联的话， 是和 /tmp 中 的 某个 新建的 文件吗？，不是 是和 ring_fd
    //   传 MAP_ANONYMOUS 的话， 不传 fd
    sq_ptr = mmap(0, sring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQ_RING);

    if (sq_ptr == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    if (p.features & IORING_FEAT_SINGLE_MMAP)
    {
        cq_ptr = sq_ptr;            // ..
    }
    else
    {
        cq_ptr = mmap(0, cring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_CQ_RING);
        if (cq_ptr == MAP_FAILED)
        {
            perror("mmap");
            return 1;
        }
    }


// p.sq_off.val: 64, 256, 576, 0x7f9f85828000
// sring. tail, mask, array, 0x7f9f85828040, 0x7f9f85828100, 0x7f9f85828240
// 0x40 = 64

    fprintf(stdout, "p.sq_off.val: %u, %u, %u, %p\n", p.sq_off.tail, p.sq_off.ring_mask, p.sq_off.array, sq_ptr);

    //
    sring_tail = sq_ptr + p.sq_off.tail;
    sring_mask = sq_ptr + p.sq_off.ring_mask;
    sring_array = sq_ptr + p.sq_off.array;

    fprintf(stdout, "sring. tail, mask, array, %p, %p, %p\n", sring_tail, sring_mask, sring_array);


    sqes = mmap(0, p.sq_entries * sizeof(struct io_uring_sqe), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQES);

    if (sqes == MAP_FAILED)
    {
        perror("mmap_sqes");
        return 1;
    }

    cring_head = cq_ptr + p.cq_off.head;
    cring_tail = cq_ptr + p.cq_off.tail;
    cring_mask = cq_ptr + p.cq_off.ring_mask;
    cqes = cq_ptr + p.cq_off.cqes;

    return 0;
}


// submit a read or write to SQ
int submit_to_sq(int fd, int op)
{
    unsigned index, tail;

    tail = *sring_tail;
    index = tail & *sring_mask;

    struct io_uring_sqe *sqe = &sqes[index];
    sqe->opcode = op;
    sqe->fd = fd;
    sqe->addr = (unsigned long) buff;

    if (op == IORING_OP_READ) {
        memset(buff, 0, sizeof(buff));
        sqe->len = BLOCK_SZ;
    }
    else
    {
        sqe->len = strlen(buff);
    }
    sqe->off = offset;
    sring_array[index] = index;
    tail++;

    fprintf(stdout, "submit, sring_tail: %p, val: %u, tail: %u\n", sring_tail, *sring_tail, tail);

    io_uring_smp_store_release(sring_tail, tail);

    fprintf(stdout, "after , sring_tail: %p, val: %u, tail: %u\n", sring_tail, *sring_tail, tail);

    int ret = io_uring_enter(ring_fd, 1, 1, IORING_ENTER_GETEVENTS);

    if (ret < 0)
    {
        perror("io_uring_enter");
        return -1;
    }
    return ret;
}

int read_from_cq()
{
    struct io_uring_cqe *cqe;
    unsigned head;
    head = io_uring_smp_load_acquire(cring_head);
    if (head == *cring_tail)
        return -1;  // buffer is empty
    cqe = &cqes[head & (*cring_mask)];
    if (cqe->res < 0)
    {
        fprintf(stderr, "ERROR: %s\n", strerror(abs(cqe->res)));
    }
    head++;
    io_uring_smp_store_release(cring_head, head);
    return cqe->res;
}



int main()
{

    int res;

    if (app_setup_uring())
    {
        fprintf(stderr, "unable to setup uring\n");
        return 1;
    }

    while (1)
    {
        res = submit_to_sq(STDIN_FILENO, IORING_OP_READ);            // fd

        fprintf(stdout, "res submit to sq: %d\n", res);

        res = read_from_cq();

        fprintf(stdout, "res read from cq: %d\n", res);

        if (res > 0)
        {
            // read success, write to stdout
            submit_to_sq(STDOUT_FILENO, IORING_OP_WRITE);       // fd
            read_from_cq();
        }
        else if (res == 0)
        {
            // eof
            break;
        }
        else if (res < 0)
        {
            // error
            fprintf(stderr, "ERROR: %s\n", strerror(abs(res)));
        }
        offset += res;
    }

    return 0;
}


