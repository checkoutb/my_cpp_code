


使用了podman，以 debian 为镜像
- 官方教程里说了：there is no easy way to uninstall gRPC after you’ve installed it globally.
- podman用全新的 debian 12.5，自己把quick start中的流程走了一边，用了5个g。

不过按照文档里使用 MY_INSTALL_DIR的话，问题应该也不大。
```text
root@b490f11da401:~/.local# du -sh $MY_INSTALL_DIR
1.1G	/root/.local
root@b490f11da401:~/.local# du -sh /root/mygithub/
3.6G	/root/mygithub/
root@b490f11da401:~/.local# du -sh /root/mygithub/grpc/
3.3G	/root/mygithub/grpc/
```


照着官方教程执行：
https://grpc.io/docs/languages/cpp/quickstart/


# podman

`podman pull debian` // 正好教程是 apt

`podman images`

`podman run -itd debian:latest`

`podman exec -it b490f11da4016 /bin/bash`


# grpc

完全按照官方的教程

`export MY_INSTALL_DIR=$HOME/.local`

`mkdir -p $MY_INSTALL_DIR`

`export PATH="$MY_INSTALL_DIR/bin:$PATH"`   // 当前shell，所以重启后，再来一遍。

`apt update`  // 慢的话，换个源，我转了一圈，用了aliyun，163，腾讯，华为，清华，上交，中科大，兰州大学。 最后用了 兰州大学的， aliyun，兰州大学的可用，另外几个连不上。 最开始是用 aliyun的，但是 apt update，有个 红色的E， 所以切换其他的，更差。最后 兰州大学可以，当然 它也有个 红色的 E 。但后续没有出问题。


---

`/etc/apt/sources.list` 换源的话，修改这个文件，没有就新建。

下面有8个源

https://blog.csdn.net/u011805907/article/details/138962902

我用的兰州大学的
```text
deb http://mirror.lzu.edu.cn/debian stable main contrib non-free
deb http://mirror.lzu.edu.cn/debian stable-updates main contrib non-free
deb http://mirror.lzu.edu.cn/debian/ buster-backports main contrib non-free
deb http://mirror.lzu.edu.cn/debian-security/ buster/updates main contrib non-free
deb-src http://mirror.lzu.edu.cn/debian stable main contrib non-free
deb-src http://mirror.lzu.edu.cn/debian stable-updates main contrib non-free
deb-src http://mirror.lzu.edu.cn/debian/ buster-backports main contrib non-free
deb-src http://mirror.lzu.edu.cn/debian-security/ buster/updates main contrib non-free
```

E: The repository 'http://mirror.lzu.edu.cn/debian buster-backports Release' does not have a Release file.

这个就是 update 时出现的 红色E， aliyun也有一个，但是具体的地址没有记录下来

---

`apt install sudo`

`sudo apt install cmake`

`sudo apt install -y build-essential autoconf libtool pkg-config`

`apt install git`

`git clone --recurse-submodules -b v1.64.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc`

---
上面的git下不动，主机传一个 fastgithub

`podman cp fastgithub_linux-x64/ b490f11da401:/root/`

---

`cd grpc`

`mkdir -p cmake/build`

`pushd cmake/build`

```text
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
```

`make -j 4`

`make install`

`popd`


---

第二天，所以需要启动

`podman start b490f11da401`

`podman exec -it b490f11da401 /bin/bash`

进入 grpc 目录后

`cd examples/cpp/helloworld`

`mkdir -p cmake/build`

`pushd cmake/build`  // 教程里没有 popd。

`export MY_INSTALL_DIR=$HOME/.local` // 不然下面报错

`cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..`

`make -j 4`

`./greeter_server &`

回车下，然后

`./greeter_client`

`fg 1`

`ctrl + c` 关闭 server

教程中还有修改的部分，也执行了，这里没有写。 
要注意下执行 cmake时的文件夹。有次在 grpc/cmake/build 中执行了 cmake。不过我很快就ctrl+c了。

---


## (如果)新工程

grpc太大了，不清楚新工程是不是需要以grpc为基础，如果是的话，太复杂了。

试下来，不需要以 grpc为工程的基础。所以 grpc 是用来安装依赖的。

所以quick start执行完后，grpc 可以删除了？(没试，不过应该可以吧)

---

1. 新建一个文件夹，作为自己的新工程。
2. 复制 quick start 用到的 common.cmake (grpc/examples/cpp/cmake/ 中) 到第一步新建的文件夹中
3. 复制 quick start 用到的 helloworld.proto (grpc/examples/protos/ 中) 到第一步新建的文件夹中
4. 复制 quick start 用到的 helloworld文件夹 (grpc/examples/cpp/ 中) 到第一步新建的文件夹中
5. 修改下 helloworld 文件夹中的 CMakefile 中 common.cmake 和 helloworld.proto 的路径。
6. 在helloworld下，`mkdir cmake/build`，并进入文件夹
7. `cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..`
  注意`export MY_INSTALL_DIR=$HOME/.local`
8. `make`
9. 运行server， client 可以看到输出



---

## further more

quick start 的最下面还有一些更进一步的资料
- https://grpc.io/docs/what-is-grpc/introduction/
- https://grpc.io/docs/what-is-grpc/core-concepts/
- https://grpc.io/docs/languages/cpp/basics/
- https://grpc.io/docs/languages/cpp/api



---

grpc 适用于少量链接， 内部服务。  内部服务 节点不多。 外部服务，节点多，占用连接。
外部服务，直接用 asio 网络通信。




## 从0开始



`podman start b490f11da401`

`podman exec -it b490f11da401 /bin/bash`


1. 新建文件夹，并进入
2. 编写 myhiservice.proto
3. `export PATH="$PATH:$HOME/.local/bin/"`
4. 这条命令不对，报错了，用下面的。使用 protoc 生成 模板 `protoc --grpc_out=. --plugin=protoc-gen-grpc="grpc_cpp_plugin" myhiservice.proto `  
  报错 `grpc_cpp_plugin: program not found or is not executable`  
  可能是它新开一个 终端，所以 没有我 export 的东西？但是应该继承我的上下文的吧。  
  下面的可以  
  `protoc --grpc_out=. --plugin=protoc-gen-grpc="/root/.local/bin/grpc_cpp_plugin" myhiservice.proto`  
  生成了 xx.grpc.pb.cc
5. `protoc --cpp_out=./ --proto_path=./ myhiservice.proto`， 生成了 myhiservice.pb.cc, myhiservice.pb.h

```text
-rw-r--r--. 1 root root  7243 Jun  3 10:09 myhiservice.grpc.pb.cc
-rw-r--r--. 1 root root 23009 Jun  3 10:09 myhiservice.grpc.pb.h
-rw-r--r--. 1 root root 19968 Jun  3 10:03 myhiservice.pb.cc
-rw-r--r--. 1 root root 19610 Jun  3 10:03 myhiservice.pb.h
```
第四点中的命令 生成的 是 grpc.pb.cc,  第五点中的是 pg.cc

**对比了下本次生成的4个文件及 之前的HelloWorld，发现：业务代码依赖 myhiservice.grpc.pb.h, myhiservice.grpc.pb.h 依赖了 myhiservice.pb.h**

.cpp 和 .cc 哪个才是正统啊。  
https://isocpp.org/wiki/faq/coding-standards  
What source-file-name convention is best? foo.cpp? foo.C? foo.cc?  
看起来 .cpp 是正统。  
.C 第一次知道  

6. 仿照 helloworld 编写 myserver.cpp, myclient.cpp

7. 新建 cmake/build 并进入
8. `cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..`
9. `make`

make失败了
```text
/myhiservice.pb.h:13:10: fatal error: google/protobuf/port_def.inc: No such file or directory
   13 | #include "google/protobuf/port_def.inc"
```
`/root/.local/include/google/protobuf/port_def.inc`

解决： CMakeLists 增加 `include_directories(/root/.local/include)`   , 有其他方法吗？ 我看 官方的 CMakeLists.txt 中并没有这些啊。  
在能运行server,client后，移除了这行，依然可以。 已经不知道 当时为什么不行了( 当时 的 命令，CMakeList全文 已经无法知道了)

`myclient.cpp:9:10: fatal error: myhiservice.grpc.pb.h: No such file or directory`  
有点道理， `<>` 是走 标准路径， 这个.h 在当前目录下，不会被搜索到的。  官方的 CMakeLists.txt 怎么弄的？ 不，我写错了，官方用的就是 `"xx.h"`

`undefined reference to 'mymsgpkg::HiResponse::HiResponse(google::protobuf::Arena*)'`  
link 开始崩了，而且容器，每次开一个 vim，切换好繁。  
解决：CMakeLists.txt 中的 foreach 中的 target_link_libraries 中少了 my_grpc_lib


还有其他各种错误很多，比如，  
- 使用 `.` 来调用 指针 (req.req())
- 拼写错误(service -> srevice， grpcpp -> grpccpp)
- 单词的错误(InsecureServer -> InsecureService)


10. 运行 myserver, myclient


`podman cp b490f11da401:/root/mygithub/mygrpc0/ asd/`






