



# install

```sh
sudo dnf install libev  #..not this ..  libev-devel..
```
`Package "libev-4.33-12.fc41.x86_64" is already installed.`

.... `sudo dnf install libev-devel` ...
development lib != runtime lib ...


`ls /usr/include/ev*`   ev++.h, ev.h





---

# Requirement

just hello - hello

select client
epoll server



---

# coding


.. 完全无从下口。 感觉都忘了。 只好从 demo开始。 一点点改造。


..xx, 不小心把 01_client_select.cpp rm 了。 不过 也没什么东西。 就是 doc中的 例子 + 自己写一些读取标准输入 的代码。无所谓了。

client 不需要用 libev的。

..vsc打开了这个文件，所以关闭的时候 提示是否保存。。保存了。 顺便改下名字。
直接就是改名的时候 使用了 rm 而不是 mv。。。


epoll 应该是默认的？，也懒得直接指定了，反正就是 ev_default_loop的参数。 不过要是 一次 flag指定 poll，一次指定 epoll，会发生什么？
没有出问题，这个是单例，所以估计 第一次调用时 的配置 生效。








