

https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/




# mongo-cxx-driver

## vcpkg install

`./vcpkg install mongo-cxx-driver`
默认版本： mongo-cxx-driver:x64-linux@3.10.1   可以。是最新的


Perl cannot find IPC::Cmd.  Please install it through your system package manager.
安装 `perl-IPC-Cmd    noarch      2:1.04-504.fc40        fedora     40 k`

`vcpkg/buildtrees/openssl/config-x64-linux-dbg-err.log`
Can't locate FindBin.pm in @INC (you may need to install the FindBin module)
安装 `perl-FindBin               noarch               1.53-506.fc40`

Can't locate File/Compare.pm in @INC (you may need to install the File::Compare module) 
安装 `perl-File-Compare` 

安装 `perl-File-Copy`


```text
    find_package(bsoncxx CONFIG REQUIRED)
    target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)

    find_package(mongocxx CONFIG REQUIRED)
    target_link_libraries(main PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
```

## 头文件 库文件

头文件
`vcpkg/installed/<CPU ARCHITECTURE>-<OPERATING SYSTEM>/include/`

库文件
`vcpkg/installed/<CPU ARCHITECTURE>-<OPERATING SYSTEM>/lib/`



# 代码

https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/tutorial/#std-label-cpp-tutorial



# 执行



`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/mnt/239G/z_other_git/vcpkg/scripts/buildsystems/vcpkg.cmake`
`cmake --build build`
`./build/main`



