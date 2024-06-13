
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


