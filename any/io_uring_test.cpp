


// #include <stdio.h>
#include <iostream>
// #include <stdlib.h>
#include <cstdlib>

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
#include <atomic>
#include <linux/io_uring.h>

#define QUEUE_DEPTH 8
#define BLOCK_SZ    1024
int ring_fd;
unsigned *sring_tail, *sring_mask, *sring_array,
            *cring_head, *cring_tail, *cring_mask;
struct io_uring_sqe *sqes;
struct io_uring_cqe *cqes;
char buff[BLOCK_SZ];
off_t offset;




// #define __test



/*
// 最后有个问题， 这个 sring_tail 为什么这样写？
// 初始化不管，
sring_tail 的 使用的地方 只有 submit_to_sq 中，并且是
tail = *sring_tail，
tail++，
*sring_tail = tail 。

并且 第一行 和 第二行 中间 隔了 好多操作， 但是 这3行 放一起 是可以的啊。 不需要隔那么远。
并发的话，肯定会出错，因为这里的 读取，保存， 不是原子的。 所以 可能2个线程 读到 同一个 sring_tail。

还有， 这里是 io_uring_enter 触发 让内核读， 但是 内核怎么知道 读多少？ 就是 sq中 tail 是在那里？
。。啊，我知道了。  所以 sring_tail 就是 让内核 知道 尾巴，  所以 它必须是 最后一步操作， 并且 需要 内存序 来防止 和 内核 乱序。

所以3行不能 和一起。

所以 2根线程 一起 submit_to_sq 是会有问题的啊。 这里并没有处理，毕竟只是一个demo。

要么加锁， 要么 类似 one loop per thread，  把 thread 和 io_uring 绑定到一起。 保证单线程。
。没有问题， myserver中是 thread 和 eventloop 绑定在一起，  eventloop 有一个 epoll。 不需要 大改。
。。不对。  确实是有问题的。 如果用 io_uring的话， 一个线程一个 io_uring， 线程池，轮询，
。不会，没问题，eventLoop 有 m_callPendingFunctors ， ，而且 单线程 也没办法并发。



*/


/* Macros for barriers needed by io_uring */
// #define io_uring_smp_store_release(p, v)            \
//     atomic_store_explicit((_Atomic typeof(*(p)) *)(p), (v), \
//                   memory_order_release)
void io_uring_smp_store_release(unsigned *p, unsigned v)
{
    // atomic_store_explicit(p, v, memory_order_release);
    // 直接把 p 变成 atomic ？
    // *p = v;     // 暂时不管。 zzzz

    // std::atomic<unsigned> pp(*p);      // 这个应该是 直接把 外面的 p 改成 atomi_uint 还是 这里改，但是这里的话。 是这样吗？ 它读的是 指针 还是 指针的内容？ 我看cppreference 上说， 从C++20 开始 可以支持 指针了。 所以，应该不会有问题吧？
    //
    //
    // pp.store(v, std::memory_order_release);

    // *p = v;

    // std::atomic_ref<unsigned> pp(*p);           // 应该对了。  用 atomic 的话， 外面的 sring_tail 数值不会变化。  用atomic_ref 就可以发生变动了，并且 变动 和 .c 的一样。         // c++20
    // pp.store(v, std::memory_order_release);


    // std::atomic<unsigned*> pp(p);
    // pp.store(&v, std::memory_order_release);
    // std::cout<<pp.load()<<", "<<*p<<", "<<v<<std::endl;     // *p 没有变化。  所以 error

    std::atomic<unsigned*> pp(p);
    *p = v;
    pp.store(p, std::memory_order_release);
    // std::cout<<pp.load()<<", "<<*p<<", "<<v<<std::endl;     // ok.  可以运行， 但是 应该 还是错的。 因为这里的语义 是 对 *p 进行 原子赋值， 但是 我这里的 赋值 是 *p=v， 并不是 原子的。   不是，这里是 时序。   原子性 也没有保证， 这里 *p =v 无法保证 原子性。  不过 32bit，应该可以 原子性。
    // release 是 在前面的读 都在 前面执行完， 在后面的 写 不能 乱序到 前面。  这样的话 前面的 写 可以乱序到 后面去  *p=v 可以被乱序到 pp.store 后面啊。 不行的， 因为 pp.store 依赖于 p的值， 所以 不能乱序的。  乱序是 保证 结果正确的情况下， 修改 指令顺序。。 有依赖关系，就不可能 乱序。
    // 实在不行 就 改成 memory_order_seq_cst  最严格的， 应该就可以了。

    // 还有 就是 atomic_uint 来 声明 sring_tail .


    // 单线程 指令重拍 不会导致问题。
    // 多线程可能， 主要就是  线程A 判断m，执行n，  线程B判断n，执行m， 就是 2个变量 在 各自的线程中 毫无关系， 但是 依赖于 其他线程的 对 该变量的 操作。  所以 线程A 中 m和n 没有任何 关系，所以 可以乱序。 一旦A乱序 就导致 线程 b的判断n 有问题。
    // 例子： A{a.init(); ready=true;}  B{if(ready) {a.exe();}}  ,  a和ready 在 A 中没有关联关系， 所以 a和ready 可以乱序，  一旦乱序， B 就会在 a没有init 的时候 进行 exe 了。
    // 但是 内存序 说的是  ... 有点绕。


    // 还有， 内存序 会扩散吗？ 就是这里是在方法中进行的内存序，  外面的话， 会不会把 这个 方法的调用 乱序掉。  感觉会啊。 所以 c 是 #define，直接替换的，不是 方法调用。

    // 还是 直接 atomic_uint  最合适。

    // https://en.cppreference.com/w/cpp/atomic/memory_order


    // 应该还是得用 atomic_uint，

}

// #define io_uring_smp_load_acquire(p)                \
//     atomic_load_explicit((_Atomic typeof(*(p)) *)(p),   \
//                  memory_order_acquire)
unsigned io_uring_smp_load_acquire(unsigned *p)
{
    // return *p;      // zzz
    // std::atomic<unsigned> pp(*p);      // c++20             // 估计。也有问题的
    // return std::atomic_load_explicit(&pp, std::memory_order_acquire);

    std::atomic<unsigned*> pp(p);
    return *pp.load();
}




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

    // 我加的，看下是不是等于 QUEUE_DEPTH , 等于
#ifdef __test
    printf("@app_setup_uring, p.sq_entries: %u\n", p.sq_entries);
#endif

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
    //   传 MAP_ANONYMOUS 的话， 不能传 fd
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

    // zzzz。。 c++中 void * 可以指向 任意数据类型， 所以 它不知道 类型的大小，所以无法 直接 增加。   c怎么可以？ 而且 这里加的 是 什么？ 类型？ 1byte？ 。。。

    // C语言： 但值得注意的是，GNU 则不这么认为，它指定“void*”的算法操作与“char*”一致。因此下列语句在 GNU 编译器中都是正确的：

    // 。。 所以 我转成 char * ，然后再 +， 然后 再转为 unsigned * ？
    //  转 unsigned * 再加的话， 会 4倍吧 ？
    // 干脆 除以 sizeof 算了。
#ifdef __test
    std::cout<<" ... p.sq_off.tail = "<<p.sq_off.tail<<", "<<p.sq_off.ring_mask<<", "<<p.sq_off.array<<", "<<sq_ptr<<std::endl;
#endif

    sring_tail = static_cast<unsigned *>(sq_ptr) + p.sq_off.tail / sizeof(unsigned);
    sring_mask = static_cast<unsigned *>(sq_ptr) + p.sq_off.ring_mask / sizeof(unsigned);
    sring_array = static_cast<unsigned *>(sq_ptr) + p.sq_off.array / sizeof(unsigned);

#ifdef __test
    std::cout<<" sring. tail, mask, array "<<sring_tail<<", "<<sring_mask<<", "<<sring_array<<std::endl;
#endif

// ... p.sq_off.tail = 64, 256, 576, 0x7f88217a8000
// sring. tail, mask, array 0x7f88217a8040, 0x7f88217a8100, 0x7f88217a8240
// ok




    // sring_tail = sq_ptr + p.sq_off.tail;
    // sring_mask = sq_ptr + p.sq_off.ring_mask;
    // sring_array = sq_ptr + p.sq_off.array;

    sqes = static_cast<io_uring_sqe*>(mmap(0, p.sq_entries * sizeof(struct io_uring_sqe), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQES));

    if (sqes == MAP_FAILED)
    {
        perror("mmap_sqes");
        return 1;
    }

#ifdef __test
    std::cout<<"p.cq_off: "<<p.cq_off.head<<", "<<p.cq_off.tail<<", "<<p.cq_off.ring_mask<<std::endl;
#endif

    cring_head = static_cast<unsigned *>(cq_ptr) + p.cq_off.head / sizeof(unsigned);
    cring_tail = static_cast<unsigned *>(cq_ptr) + p.cq_off.tail / sizeof(unsigned);
    cring_mask = static_cast<unsigned *>(cq_ptr) + p.cq_off.ring_mask / sizeof(unsigned);
    cqes = static_cast<io_uring_cqe *>(cq_ptr) + p.cq_off.cqes / sizeof(io_uring_cqe);

    // cring_head = cq_ptr + p.cq_off.head;
    // cring_tail = cq_ptr + p.cq_off.tail;
    // cring_mask = cq_ptr + p.cq_off.ring_mask;
    // cqes = cq_ptr + p.cq_off.cqes;

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


#ifdef __test
    std::cout<<"before store sring_tail: "<<sring_tail<<", "<<(*sring_tail)<<", "<<tail<<std::endl;
#endif

    io_uring_smp_store_release(sring_tail, tail);

#ifdef __test
    std::cout<<"after store sring_tail: "<<sring_tail<<", "<<(*sring_tail)<<", "<<tail<<std::endl;
#endif

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

    // int i = 2;
    // while (i-- > 0)
    while (1)
    {
        res = submit_to_sq(STDIN_FILENO, IORING_OP_READ);            // fd
#ifdef __test
        std::cout<<"res submit to sq: "<<res<<std::endl;    // 0 .. error
#endif
        res = read_from_cq();
#ifdef __test
        std::cout<<"res read from cq: "<<res<<std::endl;        // -1...
#endif
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
