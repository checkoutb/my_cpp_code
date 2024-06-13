


https://zhuanlan.zhihu.com/p/656128456
这里的更加全面点，但我的代码是从官方的教程里抄的。



指定vcpkg中依赖库的版本

https://learn.microsoft.com/en-us/vcpkg/consume/lock-package-versions?tabs=inspect-powershell

https://learn.microsoft.com/zh-cn/vcpkg/users/versioning


指定版本是靠 vcpkg.json 中的 overrides 部分指定的。

一些猜测
- 如果没有指定 buildin-baseline， vcpkg 无法知道版本，所以无法下载。需要手工install，估计不带 baseline的话，就是使用 下载的 库，但是如果我下载多个不同版本的库，vcpkg 怎么作出选择？。反正现在不指定baseline是选择了之前install的版本。 可能是： 不指定 baseline 无法下载，版本是 git当前的commit 的版本。


而且 install 可以在工程中 `/mnt/xx/xx/x/vcpkg install`，在写好 vcpkg.json 后。(没试)
确实可以，在 zhihu 的那篇文章中有



# no version


教程里没说要install啊。
我还以为可以 cmake 的时候 自动install 呢。
而且 install的时候 没有办法选择 版本啊。

就是 必然要先下载一个 预定义版本的依赖，然后才能通过 指定版本来改？
不，看后续吧，如果必须 install 的话， 工程里指定版本也没用啊。 cmake 又不会下载，还是得 人工下载， 但是 人工的话，我不知道怎么指定版本哦。

在 vfpkg 下 install  fmt 和 zlib 后就可以编译了。



在vfpkg目录下
`./vcpkg install zlib`
`./vcpkg install fmt`

在工程根目录下
`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/mnt/239G/z_other_git/vcpkg/scripts/buildsystems/vcpkg.cmake`
`cmake --build build`
`./build/main`

输出
```text
fmt version is 100201
zlib version is 1.3.1
```


# add version by git commit

在 vcpkf 根目录下
`git show 3426db05b996481ca31e95fff3734cf23e0f51bc:versions/baseline.json | egrep -A 3 -e '"zlib"|"fmt"'`
看到输出
```text
    "fmt": {
      "baseline": "7.1.3",
      "port-version": 1
    },
--
    "zlib": {
      "baseline": "1.2.11",
      "port-version": 9
    },
```

在 vcpkg.json 中增加了 builtin-baseline，但是编译后输出没有变啊。 当然，教程上也没有 执行 编译运行。 所以这里编译运行 也不会有效果的。
builtin-baseline 是一个最小版本，所以 估计 只是在 cmake 的时候 进行校验下 vcpkg 的版本。


有点尴尬，教程上执行了：
`vcpkg x-update-baseline`

我得
`/mnt/239G/z_other_git/vcpkg/vcpkg x-update-baseline`

`updated registry 'https://github.com/microsoft/vcpkg': baseline '3426db05b996481ca31e95fff3734cf23e0f51bc' -> 'a34c873a9717a888f58dc05268dea15592c2f0ff'`

把 vcpkg.json 里的 baseline 改成 vcpkg git的 HEAD commit 了



# add version force

我看了下，zlib,fmt 的版本就是最新的。

通过 overrides 就可以指定版本。 会自动下载。
估计之前不能自动下载，是因为没有 buildin-baseline， vcpkg 不知道 下什么版本。


```text
CMake Error at scripts/cmake/vcpkg_extract_archive.cmake:19 (message):
  /mnt/239G/z_other_git/vcpkg/buildtrees/zlib/src was an extraction target,
  but it already exists.
```
删除 /vcpkg/buildtrees/zlib


- 使用version>=, overrides 必须有 builtin-baseline
- 版本可能不兼容， overrides中将 版本指定为 1.2.11，编译失败。
```text
CMake Error at buildtrees/versioning_/versions/zlib/356c8b9d736b19e30f83d7ef735d21e0c063c6ca/portfile.cmake:26 (file):
  file RENAME failed to rename
    /mnt/239G/z_other_git/vcpkg/packages/zlib_x64-linux/lib/zlibstatic.lib
  to
    /mnt/239G/z_other_git/vcpkg/packages/zlib_x64-linux/lib/zlib.lib
  because: No such file or directory
```
确实是没有的，1.3 的是
```text
$ pwd
/mnt/239G/z_other_git/vcpkg/packages/zlib_x64-linux/lib
$ ls
libz.a  pkgconfig
```

指定版本后，输出
```text
fmt version is 100201
zlib version is 1.3.0
```












