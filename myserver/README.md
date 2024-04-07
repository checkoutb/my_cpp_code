





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




Reactor
Proactor






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

