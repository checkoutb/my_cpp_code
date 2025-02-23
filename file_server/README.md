


浏览器禁止 file:// 连接。

所以必须写一个server 来返回图片

rust不想弄了，编译的太大了。 而且 C++没写过server，所以试下


。。httplib 提供了 file server 的功能。非常简单。1行就可以了。。

# 坑

- 服务器只能 挂载 服务器目录下的目录。不能挂载其他目录。

**可以软连接**

连到 "/run/xxx/abc"

/run/media/asdf/16K600G/pic

- 文件夹名带空格。img src 没有加 ""，无法显示。。

- 文件夹名一些特殊文字。



