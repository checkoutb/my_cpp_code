

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

#include <vector>

class MyIOUring
{
public:


private:

//     #define QUEUE_DEPTH 8
// #define BLOCK_SZ    1024
// int ring_fd;
// unsigned *sring_tail, *sring_mask, *sring_array,
//             *cring_head, *cring_tail, *cring_mask;
// struct io_uring_sqe *sqes;
// struct io_uring_cqe *cqes;
// char buff[BLOCK_SZ];
// off_t offset;



    static const kQueueSize = 8;
    static const kBufSize = 1024;

    std::vector<char> buf;
    int ring_fd;


};


