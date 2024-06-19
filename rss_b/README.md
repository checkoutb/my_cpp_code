
RSS backend


# 从0开始的一些记录

不知道前端显示用什么好，估计会浏览器。毕竟不会qt。而且要手写一个编辑器，也挺难的。  
而且浏览器本身就支持 `<html>`， 不需要自己根据标签来转换成普通文本

应该会保存到数据库中，或者有什么文件，啊 nosql。知道 MongoDB 是用来保存文档的，不知道有没有其他的选择。  
关系型数据库挺麻烦的，不如直接保存到nosql中。 没用过MongoDB。

文件用 MongoDB  
RSS的地址，上次查看时间，这些还是要 MySQL的。

---

功能感觉挺简单的
1. 对RSS地址的 curd， 直接数据库操作也可以。 或者弄个简单的命令框

2. 定时器启动，遍历数据库中的RSS，解析，和数据库中对比，是否有新增，新增的就保存到数据库。
    1. 定时器肯定要有一个 手动触发的功能。 要展示上次触发时间
    2. 定时器分类(1h/3h/24h/7d)，定时器分为根据每次启动后，RSS是否有新增，对RSS升降级，比如 3h档的，连续3次都有新增，就升级成1h档。连续3次都没有，就降级。  不急。一般看之前手动刷新下就可以了。
    3. 失败的处理。现在直接访问RSS，不稳定啊。 多次失败，也降级，失败多了，标记为dead。不再处理。

3. http访问RSS地址， 解析XML。 判重，入库
    1. 失败处理，升降级


表1
- id, 
- rss_name(unique，应该是url unqiue，差不多), 
- rss_url, 
- status(上次访问成功就是0，失败就 min(status, 0)减去1，-3就说明连续3次失败; 有新数据，就 max(0, status)加1),  
- last_visit_time, 
- visit_frequence

频率也可以 *2, /2, 以小时为单位，至少1小时。

表2
- id，
- rss_id, 
- rss_name, 
- title(xml的title标签)，用来判重(link应该更标准，但是这个应该也。。。不够，可能很久以前的文章 和 新的文章 重复了。)
- link(xml的)，用来判重。



需要
http库
xml库
数据库 * 2  (sql, nosql)
数据库连接器 * 2
定时器， 可能要注册到 自启动，不然 怎么后台 每1h 执行一次。
日志


---
---

## xml parser

boost 没有 xml啊，
https://www.boost.org/doc/libs/1_84_0/  这里全文搜索 xml，没有。  json，http 是有的。
后续搜索的时候，发现 Property Tree 可以处理 xml 。 但估计不好用吧。

folly 也没有， 没有 http，json， 感觉里面的东西 更加 底层一些，没有 到库的级别。 更像 stl 的 扩展。 对，其他文章也有说， 是 标准库的补充。
https://github.com/facebook/folly/blob/main/folly/docs/Overview.md


有
- tinyxml
- pugixml
- xerces   apache的。 应该已经不再维护了。 最后是 2022年更新
- rapidxml  最后是2013年更新的， https://rapidxml.sourceforge.net/manual.html 的 第4章，性能秒杀 pugxml 和 tinyxml 啊。  但是 2013年的。。
- libxml2


pugixml  或  tinyxml
release 的 source code.tar.gz     tinyxml2-10.0.0 627k,  pugixml-1.14.0 563k

tinyxml 正好是一个大版本。 不过 是2023-12-31 发布的， 有问题 应该也已经修复了吧。
tinyxml 的issue 有100+个，  pugixml 的 10+个


https://blog.csdn.net/weixin_43679037/article/details/127268573
看这里的测试， pugixml  比 tinyxml2 快 4倍
rapidxml 也很快。可惜了，2013 。 不过， xml 没有什么改变   ， rapid xml 1.13.zip 才 43k 。 但C++ 有改变。



### 选pugixml。

实际上都一样，对于 RSS来说，  直接 string 的 split 都可以。

还要看下 vcpkg 行不行
tiny-xml 可以，  pugi 没有提到， 当然可以直接 github依赖，但要指定版本。






## http

- boost.beast
- libcurl
- cpp-httplib


### cpp-httplib

https://github.com/yhirose/cpp-httplib





## timer

boost 有 timer

### 直接 chrono sleep



## sql

### mysql



## nosql

- mongodb
- couchdb

文档数据库 和我想象的不太一样啊。。  我只是想要保存一个 可变长的 string。  文档数据库 所提供的 功能 远超 我需要的。

之前 视频迁移， 用过 mongodb 。对， 功能似乎用不着。 忘记了， 查询的是 mongodb 还是 amazon s3 了。 反正就是 一个json ，里面 各种信息， 应该是 s3的， 因为 记得有 路径， 时间 大小 之类的东西。


文档数据库是 直接对 文档中内容 进行 搜索，查询 的。  我只是需要一个 可变长 string。

https://www.zhihu.com/question/408729025
频繁读不频繁写的新闻资讯类文本，在保存的时候，直接写成静态的html，访问的时候直接从nginx返回
OSS

确实，直接文件就完事了，不需要数据库啊。 在mysql中记录下 路径就可以了。  还需要一些 总结/摘要 (前100个字)


### mongbodb

就当学习下吧。

可以启动并操作，具体见 my_cpp_code/any/mongodb_vcpkg_demo/


---





# 代码

目前先 一整个文件: main.cpp
多个命名空间，后期再看情况，是否把 命名空间 移出为单独的文件。

。。那分为那些 namespace 呢？ 要拆分方法，肯定要先定义好 namesapce。
- http
- 定时器
- 2个db交互
- xml转换
- 其他逻辑全 main中， 现在想到的就是 连续有新数据，就加大频率, 还有错误处理


httplib 只需要一个文件就可以了。 就不需要 vcpkg 了？
嗯。 不要了，懒得写


是否需要一个 schedule_job表， 来保存每次 定时任务的 情况


2038放弃了，直接 timestmap 再说。 不再对外部提供 具体的时间， 只提供一个 修改功能，就是 改成 now + 多少小时




## lib

cpp-httplib 的 版本： commitid： 0b657d2， 2024-6-15 的


`sudo dnf install openssl-devel`

我还以为 直接 源文件可以简单很多，结果 还是很多 错，链接时报错。
SSL support is available with CPPHTTPLIB_OPENSSL_SUPPORT. **libssl and libcrypto** should be linked.
没有注意到

`g++ main.cpp -lssl -lcrypto`

cmake 的 增加 这2个依赖。 忘记 怎么加了。


## timer

最好的肯定是 时间轮， 直接触发 应该触发的。
。。。

退一步就是，  定时器启动， 遍历所有 server，判断是否需要下载。

不如直接把  下次执行时间 保存到 数据库， 然后 数据库就 选择那些 下次执行时间 小于 now 的， 不是 拿取全部，然后 剔除。  而是 拿到的 就是 所有要执行的。   所以 增加一列。   而且 最好是 每个整点 执行 ，这样自己心里也有点数
可以的，  
启动的时候， 是直接 访问rss，还是 计算下sleep多久才到下一个整点，然后 sleep，   感觉 要直接访问，  启动的时候 肯定是 有时间的。


可以用 linux 的 cron 来执行， 这样就不需要 自己写了，  先自己试下， 我只会sleep额。  sleep 的 消耗 和 cron 能比吗？ 应该 都是 微乎其微吧。 当然 cron肯定更微一些。




## nosql

到 mysql_demo 中 尝试使用 time_t 的时候，才发现， mysql 有 nosql。。
但是 mongodb 装都装了。





## mysql ddl


`create database rss;`

```sql
create table rss.rss_source (
    id int auto_increment primary key,
    `name` varchar(64) not null,
    `path` varchar(64) not null,
    `status` int not null,
    last_execute timestamp not null,
    next_execute timestamp not null,
    unique(name, path)
);
```
`alter table rss.rss_source add column gap int default 1;`
`alter table rss.rss_source add active tinyint default 1;`


```sql
create table rss.rss_item (
	id int auto_increment primary key,
	sourceid int not null,
	title varchar(256) not null,
	link varchar(256) not null,
	createtime timestamp default current_timestamp on update current_timestamp,
	unique(link),
	foreign key (sourceid) references rss.rss_source(id)
);
```


----

`insert into rss_source (name,path,status,last_execute,next_execute) values ('https://36kr.com','/feed',0,'2022-2-2 22:22:22', '2022-1-1 11:11:11');`



## mongodb

存放
- id   uuid?  RssItem 保存， 或许 还要要保存 title，link， 冗余， 就不必每次 走mongodb了， 有些 看了title 就不想看了。 实际上 link 就可以唯一确定了。   或者，这个id 就等于 RssItem的id， 让mysql确保 RssItem的id 的唯一。
- title   xml.title
- link  xml.link
- description  xml.description



## pugixml

也只有 3个文件，直接放到 工程中

commitid: ac0ef85   2024-5-28



应该把 include 和 thrid_part 分开， 分为 2个文件。

channel -> 多个item -> title, link, description

有些rss，直接把内容放到 description， 有些是 分开的。 。。要分类了。。要多看几个。


```text
<channel><item><title>title1111</title><link>http://www.baidu.com</link><description>asdasdasd description</description></item><item><title>title 2222</title><link>http://www.goooooooogle.com</link><description>zxczxczxc description</description></item></channel>
```


.. value(), text(), child_value() 有什么区别。。


有2种
一种 `<feed>`  极少。
另一种 `<channel>`





# execute

在 rss_b 目录下 执行下面3条语句
`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/mnt/239G/z_other_git/vcpkg/scripts/buildsystems/vcpkg.cmake`
`cmake --build build`
`./build/main`



# service

想装成service。 这样开机自启动。
不行。  退出码 203，  试了一些解决方案，都不行。。

可以把 密码 放到 service 文件的 Environment 中。

看了下，这个还可以写 定时任务。。额，那就不需要 rss_timer 这种 sleep 了。

https://zhuanlan.zhihu.com/p/670693753


不行， 203！！！




