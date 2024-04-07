
#include <stdlib.h>

#include "../include/poller.h"
#include "../include/epoll_poller.h"


// class Poller2 : public Poller        // 还是抽象类， 因为没有实现 updateChannel， removeChannel
// {
//     Poller2(EventLoop* loop) : Poller(loop)
//     {}
// };


Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    if (::getenv("MUDUO_USE_POLL"))
        // return new Poller(loop);     // Poller is abstract class ...
        return nullptr; // 生成poll实例
        // return Poller2(loop);
    else
        return new EpollPoller(loop);
}
