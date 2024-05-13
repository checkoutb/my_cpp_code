

在build目录下
`cmake ..`
`cmake --build .` or `make`
`./myserver`

delete /build, mkdir build, cd build
`cmake .. -DCMAKE_TOOLCHAIN_FILE={your_path_______}/vcpkg/scripts/buildsystems/vcpkg.cmake`
`make`




还没有想好抄谁的。先收集下


- https://github.com/qinguoyi/TinyWebServer
  14.8k
  线程池，非阻塞socket，epoll，事件处理(Reactor,模拟Proactor)
  状态机解析http请求报文，直接解析 get post
  同步/异步日志系统
  webbench 实现 上万并发
  有文章

测试环境用到 mysql。。 和mysql有什么关系。是模拟业务数据之类的？ 。看到了， 注册和登录。
有文章来解析代码，不过稍微看了下， 估 计 和代码的联系 不算太紧密。

有个 CPP11 实现 (https://github.com/markparticle/WebServer) 。这个工程写了 C++14  3.1k
。。所以这个工程 是用什么实现的？

确实没有指明 版本号。


- https://github.com/linyacool/WebServer
  7.3k
  c++11
  get,head请求，处理静态资源，支持HTTP长连接，支持管线化请求，异步日志
。。管线化，，pipeline，我在其他书上看到，这个功能是http1的 (或1.1)，很坑, 各种支持不太好。
  Reactor，非阻塞IO，线程池。  线程均衡是轮询



- https://github.com/chenshuo/muduo
  14.2k



- https://github.com/xyygudu/mymuduo
  十几个
  C++11
  有文章， zhihu，但和代码不是很紧密。原理性
  稍微看了下，代码量 感觉很少
。。不过，都不大。


- https://github.com/Shangyizhou/A-Tiny-Network-Library
  几百个
  有文章

- https://github.com/S1mpleBug/muduo_cpp11
  一百多
  muduo，c++11, 移除Boost依赖
  有B站视频



# 准备抄
https://github.com/Shangyizhou/A-Tiny-Network-Library









# after copy

// 编译前，我感觉 LOG_INFO 完全没有用啊，根本没有指定文件名。
// 而且，我少了一些文件，感觉没有用到，所以就没有抄。
// 先试下
// 。。编译也难，之前的用不了，我不知道为什么 sh脚本中把 include，lib 的东西放到 /usr/local/下
// 而且 lib 里的 libtiny_network.so 有什么用？ 我抄代码的时候 并没有碰到 任何的 共享库的东西啊。
// 而且 我也不知道里面有什么。

// 先自己试下。cmake
// ..改 CMakeLists.txt 的时候，才发现，IDE 自动编译的。我靠，怪不得kDevelop一直右下角 闪绿条。

---
include_directories
add_library
add_subdirectory

target_include_directories() 的功能完全可以使用 include_directories() 实现。但是我还是建议使用 target_include_directories()。为什么？保持清晰！
include_directories(header-dir) 是一个全局包含，向下传递。什么意思呢？就是说如果某个目录的 CMakeLists.txt 中使用了该指令，其下所有的子目录默认也包含了header-dir 目录。

。。我的joplin的CMake 中只有  target_include_directories 。。

---
。# add_subdirectory(common)      // does not contain a CMakeLists.txt file. ...尴尬
。# add_subdirectory(http)

aux_source_directory 在顶层增加

add_executable(myserver main/main.cpp) 把这行提到 target_include_directories 之上 就可以 消除:`cannot specify include directories for target xxx  which is not built by this project`

---
。。。 一直没有执行 cmake --build .    一直在找 为什么不生成。
`cmake --build .` 就是调用 make 了 , 在 build 文件夹下 `make -j1`(1就是单线程) 或者直接 make

---

# compile error

编译错误 开始了。

错误真的是千奇百怪
- #define LOG_FATAL 最后加了一个 ;
- aux_source_directory 的位置，名字
- 应该 `==` 的用了 `=`
- 名字写错了 ts 用了 time (kDevelop 还可以点，点进去是 time 方法) 。我还以为是 & 的问题。
- 名字写错了，应该是 m_writeCompleteCallback ，写成了 WriteCompleteCallback
- 漏了个 ;

实际上 kDevelop 有提示错误， 但是我不知道 哪里可以看到 集中显示的， 现在都是 make 报错后，找到文件出错的行， 可以看到 kDevelop 也提示了错误。
但是哪里可以集中显示呢？
找到了，左下角【问题】..

- unknow sokelen_t .. socklen_t
- unque_lock ... unique_lock
- unknow pragma igonred ..  漏了 once

还有几个 unuse，看了下，还真发现个问题， TimerQueue.cpp 的 resetTimerfd 最后 没有使用 形参，而是使用了 m_timerfd。。。 因为 原文的形参名是 timerfd_  ， 在我看来 后缀下划线 就是 类成员，所以 抄的时候 直接用 m_timerfd 了。。


- newDefaultPoller 。。。链接失败， 少抄了类。 。。 不知道其他的没有抄的类 会不会也导致错误。。这里是少了 DefaultPoller.cc, 但是同文件，还有 EPollPoller， 代码中怎么指定 使用 默认还是 epoll ？
  。。。就在DefaultPoller中。。 等于是一个 工厂了。里面根据 环境参数 决定使用 default 还是 epoll。 非epoll的话 是个 nullptr。。 只能走 epoll。。


- HttpServer构造器中调用 TcpServer的 setConnectionCallback 方法， 确实 只有定义，没有实现。。
  ...? 原文是在 h中就定义了， 当时 我没有抄 定义。。what happen?


```text
[  4%] Building CXX object CMakeFiles/myserver.dir/main/main.cpp.o
[  8%] Building CXX object CMakeFiles/myserver.dir/net/tcp_server.cpp.o
[ 12%] Building CXX object CMakeFiles/myserver.dir/common/epoll_poller.cpp.o
[ 16%] Building CXX object CMakeFiles/myserver.dir/http/http_server.cpp.o
[ 20%] Linking CXX executable myserver
[100%] Built target myserver
```


# debug

```text
d=19 - tcp_connection.cpp : 36
2024/04/07 17:40:072960INFO new Connection arrived - http_server.cpp : 35
2024/04/07 17:40:073061INFO parse request success - http_server.cpp : 62
headers GET, /hello
Sec-Fetch-Site ：none
Sec-Fetch-User ：?1
Sec-Fetch-Mode ：navigate
Connection ：keep-alive
Upgrade-Insecure-Requests ：1
Accept-Encoding ：gzip, deflate, br
Accept-Language ：zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2
Accept ：text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
User-Agent ：Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0
Sec-Fetch-Dest ：document
Host ：localhost:8080
2024/04/07 17:40:073232INFO Connection closed - http_server.cpp : 39
2024/04/07 17:40:073278INFO TcpServer::removeConnectionInLoop [http_server] - Connection is http_server-0.0.0.0:8080#3 - tcp_server.cpp : 93
2024/04/07 17:40:073326INFO TcpConnection::dtor[http_server-0.0.0.0:8080#3] at fd=19 state=0 - tcp_connection.cpp : 43
```


使用 浏览器访问， server可以收到信息， 但是 浏览器不显示 response
应该还是有问题，主要是 `2024/04/07 17:40:073232INFO Connection closed - http_server.cpp : 39`
main.cpp中 只有最后一个 else (没有任何匹配) 才会 resp->setCloseConnection(true);
不，这2个 属性 不一样。

有点搞不清了。。
准备上 http client， 不用 浏览器了。 浏览器没有任何相应， 可能是无法解析？

之前学go 的 gin 的时候 试过了 postman，insomnia，。最后使用的是 vscode 的插件 rest client

`Parse Error: Invalid response status`
也没有告诉我，到底收到了什么。。难道要上 curl ？


curl http://localhost:8080/
curl: (1) Unsupported HTTP/1 subversion in response

curl -I http://www.baidu.com
只返回头，就是 head 请求


。。。调用过程
1. main中启动server，
。。太麻烦了。。

关键是 http_server.cpp 的 onRequest 方法， 里面 new 了 HttpResponse，并且调用了 conn->send 来发送 response的数据。   最关键的是， HttpResponse 是局部变量，没有return 出来。

那就 ++ cout 了。

```text
2024/04/07 18:48:404462DEBUGonRequest HTTP/1.1 200OK
Connection: close
Server: Muduo_zzz
Content-Type: text/html

<html><head><title>This is title</title></head><body><h1>Hello</h1>Now is 2024/04/07 18:48:37</body></html> - http_server.cpp : 90
```


`curl -I http://www.baidu.com`
```text
HTTP/1.1 200 OK
Connection: keep-alive
Content-Length: 405537
```

确实有问题， baidu 的 返回 200 和 OK 是分开的。
但是 也不至于识别出 HTTP/1 吧？

。。。
`"HTTP/1.1 %d "`
`"HTTP/1.1 %d"`  的区别。。。

# 好了

```text
curl -I localhost:8080/

HTTP/1.1 200 OK
Connection: close
Server: Muduo_zzz
Content-Type: text/html
```


http://localhost:8080
```text
HTTP/1.1 200 OK
Connection: close
Server: Muduo_zzz
Content-Type: text/html

<html><head><title>This is title</title></head><body><h1>Hello</h1>Now is 2024/04/07 18:57:21</body></html>
```

http://localhost:8080/hello
```text
HTTP/1.1 200 OK
Connection: close
Server: Muduo_zzz
Content-Type: text/plain

hello world
```

http://localhost:8080/asdf
```text
HTTP/1.1 404 Not Found
Connection: close
```

# 流程

一些已知的前提：
- one loop per thread





## 启动

1. 万物始于main
  1. main中创建了 EventLoop loop
    调用的是EventLoop 的无参构造器，但是这个构造器里作了很多事情，在event_loop.cpp中，第30行到第50行
      - atomic_bool m_loop 设置为false，但是只有构造的时候初始化为 false，loop()方法中开头设置为true，结尾设置为false，并没有 判断这个 m_loop 的地方。 不知道这个属性的作用。
      - atomic_bool m_quit 设置为false，loop() 方法中 while (!quit) 循环，在 quit() 方法中设置为 true， quit() 是在 EventLoopThread 析构器中调用的。
      - atomic_bool m_callPendingFunctors，设置为false，doPendingFunctors() 中开头设置为true，结尾设置为false，queueInLoop() 方法中会进行判断。 queueInLoop() 原文就有注释 说明为什么需要判断 m_callPendingFunctors
      - const pid_t m_tid，保存当前线程的ID
      - m_timerQueue，需要用到 this (EventLoop)
      - m_wakeupFd, ::eventfd() Return file descriptor for generic event channel，事件等待/通知机制
      - m_wakeupChannel, 需要用到 EventLoop，m_wakeupFd
      - m_currentActiveChannel 设置为 nullptr
      方法中：
      - 将this指针 放到 thread_local 的 t_loop 中，只在 EventLoop的析构器中 将 t_loop 重置为 nullptr， 还有就是 EventLoop 的构造器中，判断 t_loop 是否为nullptr，因为 one loop per thread，所以需要用到 t_loop 来判断
      - m_wakeupChannel，上设置 ReadCallback，回调函数是 EventLoop::handleRead，里面没有作什么处理啊，只是从 wakeupFd 上读取
      - m_wakeupChannel 是新建状态，没有==注册到 epoll中==，所以需要注册。Channel的构造器中，m_index 是 -1,epoll_poller.cpp 中 kNew 也是-1, 代表还没有添加到poller， EpollPoller::updateChannel 中 判断了 channel的 index，然后执行 EPOLL_CTL_ADD 或 EPOLL_CTL_DEL 或 EPOLL_CTL_MOD
  2. 使用上一步创建的 loop 及 IP端口(InetAddress的ip默认127.0.0.1) 及 服务器名字 创建 HttpServer server
    HttpServer的构造器中：
    - 将 HttpServer 的 构造参数 全部转发给 TcpServer 的构造器 来构造TcpServer
    - 构造TcpServer 的时候 构造了 acceptor，threadpool.并且在 acceptor上注册了 newConnectionCallback，TcpServer::newConnection 会将 TcpConnection 注册到 ioLoop 中，让 ioLoop 执行 TcpConnection::connectEstablished，这个方法中 使用了 m_channel -> enableRead() ，==注册到 epoll里去了。==。 这个 m_channel 是在 TcpConnection 构造器中 设置了 各种 回调函数。
      - 这个回调真的。在 HttpServer 的构造器中，会设置 TcpServer 的 messageCallback，(此时 TcpServer 的构造器已经调用结束 )
      - 在 TcpServer 的构造器中，将 TcpServer::newConnection 注册到 acceptor上，这样，有 新连接请求，就会 调用 newChannel， acceptor 底层也是用了 Channel 这个东西来 进行 事件触发 。。acceptor的底层暂时不管。
      - TcpServer::newConnection 中 创建 TcpConnection，并将 TcpServer 的 回调函数 设置到 TcpConnection 上，由于 新连接 必然是在 server 启动后的， 所以在 server启动前 ( 在 HttpServer 的 构造器中 设置的 TcpServer 的回调函数，在这里是 可见的)， 所以 TcpConnection 上的 回调函数 实际上是 HttpServer 的构造器中 设置的
      - 在 TcpConnection 的构造器中，设置了 TcpConnection的 m_channel 的属性，设置了 回调函数 为 TcpConnection::handleRead / handleWrite 等。 TcpConnection::handleRead中调用了 TcpConnection 的 msgCallback。
      - **==所以，当 TcpChannel 的 Channel 处理 read事件的时候，会调用 TcpConnection的 handleRead，然后调用 TcpConnection的 m_msgCallback，这个函数对象 是 在 TcpServer::newConnection时，从 TcpServer的 m_msgCallback 上获取，TcpServer的 m_msgCallback 是 在 HttpServer的构造器中 设置的 ，设置的是 HttpServer::onMessage。==**
    - TcpServer 的实例 是 HttpServer 的 一个属性
    - 在 tcpServer 实例上 注册 connectionCallback, MessageCallback, ==线程数量==
      - connectionCallback 是 HttpServer::onConnection， 没有什么处理。
      - messageCallback 是 HttpServer::onMessage，这里比较重要，
        - 这里将 收到的 报文 转换/格式化为 HttpRequest。(是通过 HttpContext::parseRequest 来格式化的) 。
        - 转换/格式化 完成 会调用 HttpServer 的 onRequest  ( httpContext格式化 报文是 按阶段的，第一阶段格式化 request line (应该就是 报文的第一行)，第一阶段成功后，进行 第二阶段格式化 header，由于这里只支持 get，所以 第二阶段后 成功后，就认为 转换完成 。没有第三阶段的 body ) 
        - HttpServer::onRequest 方法的 第二个形参就是 HttpRequest，并且 内部生成了一个 HttpResponse， 所以这里就是 ==生成 Response 的地方==
          - HttpResponse 的构造器没有什么太多的东西，主要是 参数 close 一直都是 true，说明 会关闭 连接。这个close的 设置有点 问题。 不考虑 Keep-Alive 之类的东西，直接 true
          - 调用了 httpServer 的 m_httpCallback， 这个回调是在 HttpServer的构造器 的 初始化列表中 设置，设置为 http_server.cpp 的 defaultHttpCallback，里面是设置一些 httpResponse 的 属性的 默认值 (状态码404,关闭连接 等) 。。。 这里不是使用的 defaultHttpCallback， 在 main 方法中 创建 HttpServer 后，调用了 server.setHttpCallback， 设置了 自己/业务 的 回调函数，设置的是 main.cpp 的 onRequest 方法，这里就是 ==真正的 业务逻辑==。
          - 调用 HttpResponse::appendToBuffer。 这里是将 httpResponse 转换为 底层的数据包。
          - 在TCP 连接上 发送 数据包。
  3. 在 server 上注册 回调函数 onRequest
  4. server 启动，loop 启动

2. 回调机制

回调/信号 的起源 必然是 select/poll/epoll。kernel怎么知道的，我们不管。
这里使用了 epoll。
在上面的main的逻辑中，创建 EventLoop 的最后 一步就是 注册到 epoll (就是 m_wakeupChannel->enableRead() )

epoll 比 select/poll 好的地方 就是 它直接在 内核中注册 需要监听 哪些fd， 而不是 每次 传入 一个 fd 的数组。
那么 内核中保存了 监听的fd 的集合， 在 用户空间中 通过 m_epollfd 来 指向 这个集合，并且可以操作 这个集合。 这个是 EpollPoller 的 构造器的 初始化列表中 作的 ( 通过 epoll_create进行 system call )。

在注册的时候，最后调用了 EpollPoller::update，注意 epoll_ctl 的最后一个 参数，把 channel的 events (感兴趣的事件)， channel 和 fd 放一起 形成了 底层需要的 epoll_event。 
epoll_wait 返回的 epoll_event 中可以找到 channel。


0. 谁调用了 EpollPoller::poll
  下面第三点的 loop 中调用的
  。。不止main 中，还有 EventLoopThread::threadFunc 中也调用了 loop.loop()。 这个函数是 注册到 线程 上的，所以 线程中 的线程 执行的就是 EventLoopThread::threadFunc 这个方法。

1. EpollPoller::poll 中调用了 epoll_wait，所以是 起源
  epoll_wait 的注释： The "events" parameter is a buffer that will contain triggered events. The "maxevents" is the maximum number of events to be returned 
2. 获得 有事件发生的 fd 集合后， 调用 EpollPoller::fillActiveChannels
  - 遍历 epoll_wait 返回的数组，获得 Channel*  和 该fd 发生的 事件， 调用 channel->setRevents 将发生的事件 设置到 channel 上。
  - 将channel push到 activeChannels 中，这个activeChannels 是 输出参数
3. channel 是 EventLoop 的属性，在 EventLoop::loop() 中 (这个方法是 main 的最后一步) 
  - 循环，调用 第一点的 EpollPoller::poll 方法，获得 activeChannels 。然后 对这些 channel 进行处理，就是 根据 channel 的 revent 来知道 是 读/写/错误 事件， 调用 对应的 回调函数




loop
至少有2个loop
一个是 main 中 创建的 EventLoop ，可以认为是 acceptor
另外一个是 TcpServer 的 m_threadPool，类型是 EventLoopThreadPool 。
在 TcpSever::newConnection 中， 第一行就是 获得一个 EventLoop，这里用的是 轮询的策略 ( 如果没有线程池，那么就使用 main中创建的 EventLoop，这种情况不管)。  这个 EventLoop 是 TcpConnection 构造器的 参数，在 构造 TcpConnection 的 channel 的时候 也传入了 这个EventLoop， 所以 TcpConnection 的 channel  就和 这个 EventLoop 绑定了。
在 TcpServer::newConnection 的最后，使用 EventLoop 来执行 TcpConnection::connectEstablished ，这个方法 调用了 TcpConnection的 m_channel 的 enableRead()， 而 channel的enableRead 最后会调用 channel 的 update，update 中就是 m_eLoop->updateChannel(this); (使用 Channel中 的 m_eloop 来注册 到 EpollPoller 的 epoll)


one loop per thread 是什么？
。。扩展到 多线程服务器模型。。 我只知道一个 1:M:N， 但是好像没有找到信息啊
。还有 Reactor，Proactor 









EventLoop的内部逻辑
wakeup, eventfd

TimerQueue
。。好像没有用到。
是的，只在 event_loop.h 中被导入了， 有一个 m_timerQueue 属性， 但是 那些方法(runAt, runAfter, runEvery) 没有被调用。

Channel

线程池



# 扩展



---
https://zhuanlan.zhihu.com/p/392712498

## 多线程常用编程模型

- 每个请求创建一个线程，使用阻塞式IO操作(伸缩性不佳)
- 使用线程池，同样使用阻塞式IO操作
- 使用非阻塞IO + IO多了复用
- Leader/Follower


> one loop per thread

程序里的每个 IO 线程有一个 event loop (用作 IO 多路复用)，用于处理读写和定时时间。
event loop 代表了线程的主循环，需要让哪个线程干活，就把 timer 或 IO channel 注册到哪个线程的 loop 里即可。

> Leader/Follower

此模型会创建一个线程池，每个线程有三种状态：leading, following, processing。
Leader 线程负责监听请求，其他线程作为 follower 处于等待状态，当 leader 收到请求后，首先通知一个follower线程将其提拔为新的 leader，然后自己去处理这个请求，处理完毕后加入 follower 线程等待队列，等待下次成为 leader。
Leader/Follower 模式避免了线程动态创建和销毁的额外开销，将线程放在池中，无需交换数据，将上下文切换、同步、数据移动和动态内存管理的开销都降到了最低。


推荐的多线程编程模式：one loop per thread + 线程池。
event loop 用作 IO 多路复用，配合非阻塞 IO 和定时器；线程池用作计算，可以是任务队列或生产者消费者队列。

---
https://blog.csdn.net/woaiwojia6699/article/details/112731409

客户端：socket、connnect、send、recv
服务端：socket、bind、listen、accept、recv、send、close
阻塞IO和非阻塞IO：没有数据的时候是不是立即返回

优化方案1：为每一条连接，创建一个线程。

优化方案2：reactor网络模型
1. 组成：非阻塞IO+IO多路复用(复用的网络线程)
  linux下的IO多路复用模型
  select：需要轮询检查每一个IO是否有事件。
  poll：将读、写、错误分成三种类型去检测
  epoll：将读、写、操作事件集中到一个阻塞中去完成，并且返回有事件的IO。
2. 特征：事件循环+事件驱动/回调
3. 缺点：容易分裂业务逻辑
4. 解决办法：协程：同步非阻塞

epoll_create, epoll_ctl, epoll_wait


one loop per thread 模型
每个线程都有一个循环
重点：
1. accept 一个线程
2. 读，写，错误 另起线程

监听线程 和 处理线程分开，每个线程创建一个 epoll，loop循环就是这个epoll

变种：one loop per thread + 队列 + 线程池 ， 会有一定的延时



---
https://blog.51cto.com/u_15346415/5172283

使用最广泛的是 non-blocking IO + IO multiplexing, 即Reactor
程序的基本结构是 一个 事件循环 (event loop)，以 事件驱动 和 事件回调的方式 实现业务逻辑

Reactor的优点
- 编程容易，效率也不错
- 不仅可以用于 读写 socket， 建立连接 (connect/accept)，甚至DNS解析 都可以用 非阻塞的方式进行，以提高并发度 和 吞吐量， 对IO密集型APP 也是不错的选择
- DNS解析也可以用 非阻塞方式进行，以提高 并发度 和 吞吐量

lighttpd 就是这样，它内部的fdevent 非常精妙，值得学习

基于事件驱动的编程模型的 缺点
- 要求事件回调函数必须是 非阻塞的
- 对于涉及网络IO的请求响应式协议，它容易割裂业务逻辑，使其分散在 多个回调函数中，不容易理解和维护
- 现代的编程语言有一些应对的方法(coroutine)

多线程服务器的编程模型
- 一个请求一个线程，使用阻塞式IO，伸缩性不佳
- 使用线程池，同样使用阻塞式IO操作
- 使用non-blocking IO + IO multiplexing，即java NIO 的方式
- Leader/Follower

默认情况下，一般使用第三种，即 non-blocking IO + one loop per thread。

one loop per thread
- 这个模型下，程序里的每个IO线程 有一个 event loop (或者叫 Reactor)， 用于处理 读写 和 定时事件

好处
- 线程数目基本固定，可以在程序启动的时候设置，不会频繁创建和销毁
- 方便地在线程间调配负载
- IO事件发生的线程是固定的，同一个TCP链接不需要考虑事件并发

EventLoop代表了 线程的 主循环。需要让哪个线程干活，就把 timer 或 IO channel (如TCP连接)  注册到 那个线程的 loop中即可
- 对实时性有要求的 连接 可以单独用一个 线程
- 数据量大的 connection 可以独占一个线程，并把 数据处理任务分摊到 另外几个 计算线程中 (用线程池)
- 其他次要的辅助性 连接 可以共享一个线程

对于 non-trivial 的服务端程序，一般会采用 non-blocking IO + IO multiplexing， 每个 connection/acceptor 都会注册到 某个 event loop 上，程序里有 多个 eventloop, 每个线程最多一个  event loop

多线程程序 对 event loop 提出了更高的要求，那就是 "线程安全"
- 要允许一个线程往 别的线程的 loop 里 赛东西 ( 比如 主IO线程 收到一个 新建的连接，分配给某个子 IO线程处理)，这个loop 必须是 线程安全的

对于没有IO，只有计算任务的线程，使用 event loop 有点浪费，有一种补充方案，即 blocking queue
muduo 的 `BlockingQueue<T>` 。。 这个不对的， muduo 是C++的， `BlockingQueue<T>` 的 命名和 范型 是 java的。

muduo 推荐的C++多线程服务端 编程模式为 one (event) loop per thread + thread pool
- event loop (也叫 IO loop) 用作 IO multiplexing，配合 non-blocking IO 和 定时器
- thread pool 用来作计算，具体可以是 任务队列 或 生产者消费者队列。
- 具体使用 几个 loop，线程池的大小，应该根据app来设定，基本原则是 "阻抗匹配"，使得 CPU 和 IO 都能高效运作
- app中还有 个别特殊任务的线程，比如 logging，在分配资源时 需要算进去。

。。看这里，似乎 one loop per thread 就是 1:N:M 啊， 一个线程监听端口，有事件后，发给 N 个 event loop 的线程， event loop 线程通过 任务队列 / 消息队列 发送给 包含M根线程的 thread pool 进行 业务计算 ？



---
https://blog.csdn.net/m0_53485135/article/details/134007819

高性能的网络程序中，使用最广泛的是 `non-blocking IO + IO multiplexing`，即Reactor
- lighttpd
- libevent,libev
- ACE, Poco C++ libraries
- Java NIO, Apache Mina, Netty
- POE (Perl)
- Twisted (Python)

Boost.Asio, Windows IO Completion Ports 实现了 Proactor，应用面 窄一些。
ACE 也实现了 Proactor 

在 non-blocking IO + IO multiplexing 这种模型中，程序的 基本结构是 一个 事件循环 (event loop), 以事件驱动 (event-driven) 和 事件回调的方式 实现 业务逻辑。

。。这篇 和上一篇 一样的。这篇是 2023, 上一篇是 2022,  但是，但是，但是， 这篇 说了 "以事件驱动" ，上一篇 只有 "以事件驱" ， 少一个动了，所以上一篇 ，我只抄了 "事件回调的方式"
。。感觉还有一篇 应该在更早

。。https://www.cnblogs.com/ljygoodgoodstudydaydayup/p/5749880.html
。。16年的。。但是这里内容 很少很少。 我百度的是 "程序的基本结构是一个事件循环"
。。这篇也少东西。。 Reactor的优点的 第二条，很明显少了。
。。这些文字应该是 《Linux多线程服务端编程》 书上的。

。。和上一篇 补一起了。



---

https://www.jianshu.com/p/46d1c2830ff3

## 多线程服务器的常见编程模型

### IO 模型
IO模型分为 同步IO，异步IO
同步IO分为 Blocking IO, Non-Blocking IO, IO multiplexing

Blocking IO, 发送端send发送缓冲区满 或 接收端recv 接收缓冲区满 会阻塞当前线程
Non-Blocking IO，send, recv 直接返回错误 (需要轮询)
==IO Multiplexing，通过 select/poll/epoll 等 系统调用 监控多个连接上的 读写事件==

### 线程模型
Blocking IO + one thread per connection
acceptor 创建新线程

Blocking IO + thread pool
acceptor 转发给 线程池

IO multiplexing + thread pool
acceptor -> event queue -> pooler(select/poll/epoll) -> worker(one of thread pool)
。。这个才是 1:M:N 吧？

IO multiplexing + one thread one loop
线程数目基本固定
高并发


one thread one loop思想
一个线程 一个 事件循环流程

```C++
void CReactor::Run()
{
    //线程退出标志
    m_bShouldRun = true;
    while(m_bShouldRun)
    {
        //处理其他的任务
        handle_other_task();
        //利用select/pool/epoll等I/O多路复用监听各个连接（fd)的读写事件
        select_or_epoll_function();
        //处理各个连接(fd)的读写事件
        handle_io_event();
        //处理定时器
        check_timer();
        //处理同步或者异步事件
        dispatch_event();
    }
}
```

线程分为 AcceptReactor, TresultReactor, FrontReactor
AcceptReactor
- 监听是否有新的客户端连接 (listenfd 上是否有 读事件 发生)
- 有新的连接请求，则生成 新连接的 socket，并将 该socket 传递给 frontReactor

TresultReactor
- 订阅 tserver 发布的流水，接收到流水中的 XTP报文，保存到 TradeResult 中
。。？这个是什么？ 和 现在的 似乎没有任何关系。

FrontReactor
- 通过 epoll 或 select 监听 该线程 所负责的 socket 上的 读写事件
- 收包 (收网络数据，放缓冲区，解包并处理)，发包
- 从 TradeResult 中读取 tserver 发布的 XTP 报文，并处理





http://kegel.com/c10k.html





·



## pid_t is int

```C
#include <sys/types.h>  // typedef __pid_t pid_t;
#include <bits/types.h> // # define __STD_TYPE		typedef
                        // __STD_TYPE __PID_T_TYPE __pid_t;
#include <bits/typesizes.h> // #define __PID_T_TYPE		__S32_TYPE
#include <bits/types.h> // #define	__S32_TYPE		int
```




# cmake

设置参数
https://blog.csdn.net/zhiyuan2021/article/details/128882636


# vcpkg

https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md#%E5%AE%89%E8%A3%85-linux-developer-tools


哈哈，想偷懒 没偷到。
```text
$ vcpkg
bash: vcpkg: 未找到命令...
安装软件包“vcpkg”以提供命令“vcpkg”？ [N/y] y

 * 正在队列中等待...
 * 正在载入软件包列表。... 安装软件包失败: Multiple matches of vcpkg;2023.06.22-3.fc39;x86_64;fedora
```

sudo 后
`dnf install vcpkg`
这里没有说 多个匹配。

只有上面一条，没有 那种 dnf update 的语句( 不知道有没有这种语句)

```text
已安装:
  aria2-1.37.0-3.fc39.x86_64           vcpkg-2024.03.14-1.fc39.x86_64
```

---

```text
$ vcpkg install glog
current_path("/home/xxxx/.local/share/vcpkg"): No such file or directory
```
。。本来不想这里的，但是这里应该更好吧， 而且我也不知道怎么改。估计改 property

```text
$ vcpkg install glog
error: Invalid triplet: x64-linux
Built-in Triplets:
Community Triplets:
```
。。在 share文件夹下 建立 vcpkg 后， invalid triplet


`vcpkg help triplet`

```text
$ vcpkg help triplet
Built-in Triplets:
Community Triplets:
```
有种 完蛋了的感觉，但不应该啊。

```text
$ vcpkg search glog
The result may be outdated. Run `git pull` to get the latest results.
If your port is not listed, please open an issue at and/or consider making a pull request.  -  https://github.com/Microsoft/vcpkg/issues
```

。。找不到 可以执行git的 文件夹啊， whereis 只能找到一个 文件，找不到 文件夹


```text
$ vcpkg integrate install
Applied user-wide integration for this vcpkg root.
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=/home/xxxx/.local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

看了下，目前没有这个文件。


```text
已移除:
  aria2-1.37.0-3.fc39.x86_64           vcpkg-2024.03.14-1.fc39.x86_64
```


这网络是真的无语啊
看来 上面的 git pull 应该就是指 这个 仓库吧，我看很多文件啊，

```text
接收对象中: 100% (230376/230376), 68.30 MiB | 9.77 MiB/s, 完成.
处理 delta 中: 100% (152789/152789), 完成.
正在更新文件: 100% (11314/11314), 完成.
```

但是看文件系统中 只有 `13,857 项，共 19.8 MB`
文件的 ports 10万个文件，看到了 glog， 就是 第三方库的 索引也在这个仓库了，

所以 dnf install vcpkg ,安装了个 什么东西。。。


```text
$ ./bootstrap-vcpkg.sh
Downloading vcpkg-glibc...
vcpkg package management program version 2024-04-23-d6945642ee5c3076addd1a42c331bbf4cfc97457
```


```text
$ ./vcpkg install glog
Computing installation plan...
A suitable version of cmake was not found (required v3.29.2) Downloading portable cmake 3.29.2...
Downloading cmake...
```
这版本怎么这么高。。 最新的就是 3.29.2


```text
sudo dnf remove cmake

 软件包                     架构    版本                        仓库       大小
================================================================================
移除:
 cmake                      x86_64  3.27.7-1.fc39               @fedora    31 M
移除依赖的软件包:
 kdevelop                   x86_64  9:23.08.1-1.fc39            @fedora    16 M
清除未被使用的依赖关系:
 aspell                     x86_64  12:0.60.8-12.fc39           @fedora   3.2 M
 aspell-en                  x86_64  50:2020.12.07-8.fc39
......

事务概要
================================================================================
移除  116 软件包

将会释放空间：954 M
确定吗？[y/N]：
```

wocao, 我的 kDevelop ...

`sudo dnf update cmake`  dnf上是最新的了。 `cmake version 3.27.7`

那没办法了，看 vcpkg 的表演了。


41.4

```
whereis cmake
cmake: /usr/bin/cmake /usr/lib64/cmake /usr/local/lib/cmake /usr/share/cmake /usr/share/man/man1/cmake.1.gz
```

CMake 3.29.2 available for download April 11, 2024


vcpkg 这么激进吗, 今天4-27。  不过 git版本回退应该就好了

```text
$ ./vcpkg install glog
Computing installation plan...
A suitable version of cmake was not found (required v3.29.2) Downloading portable cmake 3.29.2...
Downloading cmake...
https://github.com/Kitware/CMake/releases/download/v3.29.2/cmake-3.29.2-linux-x86_64.tar.gz->/run/media/vcpkg/downloads/cmake-3.29.2-linux-x86_64.tar.gz
```

版本回退下， 最近的tag 是 2024-3-25 的

退到 2024.03.25 可以。 没有要求 更新 cmake

刚才，想多退一些，所以退到了 2024.01.12 上， 然后 下 glog，发现 是 0.6.0 的。  然后 网络 git 连不上,，所以 ctrl+c, 版本 还原到 2024.03.25 了， 2024.03.25 的是 0.7.0 的，

看了下
glog， 2022-4-5, 0.6.0 ，  2024-1-10,0.7.0-rc,  2024-02-18,0.7.0
spqlog 半年左右 (4-8个月 一版)， 前2天 刚发了一版。。


```text
CMake Error at scripts/cmake/vcpkg_download_distfile.cmake:32 (message):

      Failed to download file with error: 1
```



```text
service fastgithub start

export https_proxy=http://127.0.0.1:38457
export http_proxy=http://127.0.0.1:38457      // 是的 都是 http://

./vcpkg install glog

```

ok
```text
Total install time: 1.3 min
glog provides CMake targets:

  # this is heuristically generated, and may not be correct
  find_package(glog CONFIG REQUIRED)
  target_link_libraries(main PRIVATE glog::glog)
```

`"-DCMAKE_TOOLCHAIN_FILE=/run/media/vcpkg/scripts/buildsystems/vcpkg.cmake"`



`cmake .. -DCMAKE_TOOLCHAIN_FILE=/run/media/vcpkg/scripts/buildsystems/vcpkg.cmake`


试了半天，cmake的缓存'建功'了， 删了 build/ 就可以了。

看到的错误就是，cmake直接说 glog不存在， 没有任何 vcpkg 的信息。 就是因为 之前build了，cmake 已经缓存了之前的东西 吧？
就是下面的错误，下面的错误是 `cmake .. -DCMAKE_TOOLCHAIN_FILE=xxxx/vcpkg.cmake` 命令的 返回
里面和 vcpkg 毫无关系。 就是 走的 缓存
```text
CMake Error at CMakeLists.txt:23 (find_package):
  By not providing "Findglog.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "glog", but
  CMake did not find one.

  Could not find a package configuration file provided by "glog" (requested
  version 0.7.0) with any of the following names:

    glogConfig.cmake
    glog-config.cmake

  Add the installation prefix of "glog" to CMAKE_PREFIX_PATH or set
  "glog_DIR" to a directory containing one of the above files.  If "glog"
  provides a separate development package or SDK, be sure it has been
  installed.
```


`vcpkg.json`  这个没有的话， vcpkg 会报错， 报错很明显
```json
{
  "dependencies": [
    "glog"
  ]
}
```


# glog

main/main.cpp 的 main方法中
```C++
    // https://github.com/google/glog/blob/master/src/glog/flags.h
    FLAGS_log_dir = "log33";  // 自定义日志文件位置，这个文件夹需要手动创建好，这行要在 InitGoogleLogging 之前。
                // `./myserver` 运行程序的，所以这个 文件夹是 {project_root}/build/log33
                // 默认位置： /tmp/myserver.192.xxxxxxxxxx
    google::InitGoogleLogging(argv[0]);
    int a = 123;
    LOG(INFO) << "FIND " << a << " in glog";

```

## 日志等级
。。没有debug，有点意外。

```C++
// https://github.com/google/glog/blob/master/src/glog/log_severity.h

enum LogSeverity {
  GLOG_INFO = 0,
  GLOG_WARNING = 1,
  GLOG_ERROR = 2,
  GLOG_FATAL = 3,
#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#  ifdef ERROR
#  error ERROR macro is defined. Define GLOG_NO_ABBREVIATED_SEVERITIES before including logging.h. See the document for detail.
#  endif
  INFO = GLOG_INFO,
  WARNING = GLOG_WARNING,
  ERROR = GLOG_ERROR,
  FATAL = GLOG_FATAL
#endif
};
```



## config

https://github.com/google/glog/blob/master/src/glog/flags.h

DECLARE_类型

但是不知道默认值是多少，但是应该够用，不至于很离谱。就是开箱即用。
默认值： https://github.com/google/glog/blob/master/src/flags.cc   应该看这个文件。 .h 只有声明，这里有 声明+默认值+描述


- DECLARE_int32(logemaillevel);
  email发送 大于等于这个级别的日志
  默认 999 。。。

- DECLARE_int32(logcleansecs);
  每隔多久清除逾期(overdue)日志
  默认 60*5 sec，  即5分钟。
  。。所以有下面的 linux 可以立刻驱逐。

- DECLARE_bool(drop_log_memory);
  linux专用
  大意是：一旦写入到磁盘，就立刻从内存驱逐出去，因为我们不可能再去读取它，并且日志增长可能很快，所以要立刻驱逐
  默认true

- DECLARE_string(alsologtoemail);
  发送到 这些email地址 + logfile
  默认：""

- DECLARE_string(log_backtrace_at);
  Emit a backtrace when logging at file:linenum.
  默认 ""

- DECLARE_bool(timestamp_in_logfile_name);
  文件名是否带时间戳
  。。不带时间戳，同名文件，append
  默认：BoolFromEnv("GOOGLE_TIMESTAMP_IN_LOGFILE_NAME", true)

- DECLARE_bool(logtostdout);
  日志输出到stdout，而不是logfile
  BoolFromEnv("GOOGLE_LOGTOSTDOUT", false)

- DECLARE_bool(colorlogtostdout);
  输出到stdout的日志着色
  false

- DECLARE_bool(logtostderr);
  日志输出到stderr，而不是logfile
  BoolFromEnv("GOOGLE_LOGTOSTDERR", false)

- DECLARE_bool(alsologtostderr);
  日志输出到stderr 和 logfile
  BoolFromEnv("GOOGLE_ALSOLOGTOSTDERR", false)

- DECLARE_bool(colorlogtostderr);
  输出到stderr时着色
  false

- DECLARE_int32(stderrthreshold);
  **int32**
  日志级别>=该设置的，发送到stderr 和 logfile
  默认：google::GLOG_ERROR
  。。所以Error级别的会 stderr + logfile

- DECLARE_bool(log_file_header);
  创建文件时是否增加log file header
  。。应该就是指下面的(在日志文件的头几行)
  ```text
  Log file created at: 2024/04/28 09:44:07
  Running on machine: 192.168.1.5
  Running duration (h:mm:ss): 0:00:00
  Log line format: [IWEF]yyyymmdd hh:mm:ss.uuuuuu threadid file:line] msg
  ```
  默认 true

- DECLARE_bool(log_prefix);
  每行输出是否带上日志前缀(指 时间，线程，文件名，文件行)
  true

- DECLARE_bool(log_year_in_prefix);
  日志前缀是否包含年
  true

- DECLARE_int32(logbuflevel);
  日志级别小于等于这个的buffer，大于这个的立刻flush
  默认 0 ; (-1:不buffer，0 是info，所以buffer了info)

- DECLARE_int32(logbufsecs);
  可以buffer多久，就是buffer中有日志后，多久flush一次
  默认 30 sec

- DECLARE_int32(minloglevel);
  低于这个等级的日志不会输出
  默认 0

- DECLARE_string(log_dir);
  日志目录
  执行 DefaultLogDir() 方法，里面从 env中搜索 "GOOGLE_LOG_DIR", "TEST_TMPDIR"。没有的话，怎么使用/tmp 的不清楚。

- DECLARE_int32(logfile_mode);
  设置日志文件模式/权限
  默认: 0664

- DECLARE_string(log_link);
// Sets the path of the directory into which to put additional links
// to the log files.
  应该就是指 `myserver.INFO`，默认是和logfile一起的，我修改了 log_dir 后，myserver.INFO也自动变更了。它是一个link，指向了最后的日志文件。
  默认 ""

- DECLARE_int32(v);  // in vlog_is_on.cc
  Show all VLOG(m) messages for m <= this. Overridable by --vmodule.
  默认 0

- DECLARE_string(vmodule);  // also in vlog_is_on.cc
  ```text
  "per-module verbose level."
  " Argument is a comma-separated list of <module name>=<log level>."
  " <module name> is a glob pattern, matched against the filename base"
  " (that is, name ignoring .cc/.h./-inl.h)."
  " <log level> overrides any value given by --v.")
  ```
  默认 ""

- DECLARE_uint32(max_log_size);
  日志文件最大大小 MB
  默认 1800,  所以 1.8g

- DECLARE_bool(stop_logging_if_full_disk);
  磁盘满了，是否停止向磁盘输出日志
  默认 false

- DECLARE_bool(log_utc_time);
  是否使用UTC时间
  。。glog怎么获得当前时区的？
  默认 false

- DECLARE_string(logmailer);
  mailer
  应该是指邮件服务器
  默认 ""

- DECLARE_bool(symbolize_stacktrace);
  应该是：栈展开时，打印符号(就是方法名)，不然只有方法地址，没有方法名
  默认 true





















