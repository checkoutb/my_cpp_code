

# 需求

- 在swaybar上增加一个模块：番茄时钟
- 就是25min 5min 25min 5min 这种间隔不停地循环
  - 自动循环，还是 5min到期 背景变红，需要左键开启25min ？ 留好结构。倾向于后者
- 25min的时候，模块背景绿色，5min的时候，模块背景黄色，
- 左键点击，25min开始循环； 右键点击，5min开始循环

不实现：
- 声音： 25min：雨声， 5min：xx
- 记录 hh:mm - hh:mm 是 25min的， 几点-几点是5min的
  - 中断 for 吃饭，午休，下班后。
  - 键鼠无操作，自动暂停
    - C++怎么判断 键鼠有无操作？
    - 5min到期后，背景变红，需要 左键开启 25min，而不是自动循环？
- 怎么判断 25min中是不是真的25min的状态，还是依然5min的状态
  - 访问的网站，
  - 是否在 ide中 写代码
  - 会议？
  - 怎么记录事件， 有事件才能打标记，才能了解行为是 25min 还是5min



# do

fedora-sway的 swaybar，使用了 waybar, 所以是 waybar番茄时钟。

https://centlinux.com/waybar-config-linux-status-bar/

https://github.com/LawnGnome/waybar-custom-modules

https://github.com/Alexays/Waybar/blob/master/src/modules/custom.cpp


问题
1. LawnGnome/waybar-custom-modules 中 有些模块的 main方法中是 loop，有些不是，为什么有这种区别？
2. 我写的main方法怎么和 waybar 交互？ 因为 main方法没有返回值啊。

感觉是通过 死循环 + 某种信息传递。
而且 番茄钟 本身要 一直启动着(。应该就是loop的原因)， 不能 每次重新启动。


https://github.com/Alexays/Waybar/wiki/Module:-Custom

这里的 interval 写了 The interval (in seconds) in which the information gets ==polled==.

If no interval is defined, it is assumed that the out script loops it self.
。。？loop 怎么 暂停？ 不可能死循环吧， 但是如果 sleep的话， waybar需要信息的时候 怎么唤醒？

。。上面的 loop 是 Loop， https://github.com/LawnGnome/waybar-custom-modules/blob/master/waybar/src/lib.rs#L18
是自己写的方法，里面 有sleep， 但是 用的是 send，
。。上面是 poll，不是 pull。。

而且 send() 中只是一个 println! ， 难道通过 stdout 来 和 waybar 通信？

https://github.com/2IMT/polycat
C++写的 waybar module
。但是代码有一点点点点点点点点多

https://github.com/2IMT/polycat/blob/main/src/polycat.cpp   main方法在这里， 里面有 while(true) + sleep，并且在 sleep之前 使用了 std::cout。


---

问题：
1. sleep的时候 怎么接收 左键 右键，   emmmmm， 不sleep的时候 怎么接收？








---

# ok

1. 复制 waybar 的配置文件
`cp /etc/xdg/waybar/* ~/.config/waybar/`
config.jsonc  style.css

2. g++ 编译出 二进制文件

3. 修改上面的 config.jsonc
  1. 在 配置文件的 modules-left 中，添加 `"custom/tomato_clock"`
  2. 在 配置文件的最后，添加
    ```json
    "custom/tomato_clock":  {
        "format": "{}",
        "exec": "/mnt/239G/my_github/my_cpp_code/waybar_tomato_clock/way_tomato_clock.out"
    }
    ```

4. mod(即win)+shift+C，重启sway


---

目前只是 使用文字输出 倒计时，没有 背景颜色的修改，鼠标点击的交互

`ps -ef | grep tomato`  可以看到 重启后，只有一个 线程， 所以 sway可以处理 线程的 关闭。

但是，我记得 之前 右上角的组件 是连在一起的，现在 怎么是分开了一点。 但无所谓。


---

颜色有点难，这是通过 style.css 来设置的，估计 变不了。。

问了下，可以的， 通过返回 json， json中的 class 字段来指定 css的 class， 就可以了。


绿色休息 还是 黄色休息？

> 绿工作，蓝休息，黄手动

没有办法做到 等待 点击， 可以靠 D-bus，但是似乎太麻烦了

好像也不是很麻烦，，直接 IPC， 等待数据， 然后 on-click， on-click-right， on-click-middle， 各自 执行单独的 软件，分别 向本程序 发送 信号/消息/socket包 等，让 本程序 能从 无限等待数据 中返回。
。似乎非常完美。 这就是 IPC 啊。

。不过这样的话，sleep 1min 是不是要修改？ 不需要。我可以接受等待1min。
。不，实际上 可以 解决的， 就是 const int WORK_DOWN_COUNT = 25*60/SLEEP_SEC;  这样 每次醒来， WORK_DOWN_COUNT--, 到0 就说明 25min 。！！！


看了下以前的代码， fifo 好像最简单。

---

感觉需要 左键，中键，右键。。算了，就左右，  中键可以用来清空，但是 没什么必要。 误点，应该是 左右键 误点，而不是 不需要点击的时候 进行了 点击。

而且，之前想的是，每个 键一个 程序，但是 现在想到 可以通过 参数 来传递 点击了什么。 这样的话，感觉 中键 也很简单， 试试吧

---

fifo 有点，， 那个文件，，必须在 运行前不存在。。还得删除。
目录得存在，文件得不存在。
目录需要手工建立， 文件 mkfifo 前 delete下。


---

# finale

```sh
 # 编译
make r
 # mv
mv *.out ~/Documents/waybar/
vi ~/.config/waybar/config.jsonc
```
- 在 config.jsonc 开始处的 modules-left/center/right 中添加 `"custom/tomato_clock"`， 注意 json的逗号， 这3个位置随便，可以都添加。
- 在最后一个 } 前添加：
  ```json
      "custom/tomato_clock":  {
          "format": "{}",
          "exec": "~/Documents/waybar/waybar_tomato_clock.out",
          "return-type": "json",
          "on-click": "~/Documents/waybar/waybar_tomato_clock_clicker.out L",
          "on-click-right": "~/Documents/waybar/waybar_tomato_clock_clicker.out R",
          "on-click-middle": "~/Documents/waybar/waybar_tomato_clock_clicker.out M"
      }
  ```


在 config.jsonc 同目录的 `style.css` 的最后 追加
```css
#custom-tomato_clock {
    padding: 0 15px;
    color: white;
}
#custom-tomato_clock.work {
    background-color: #1ca000;
}
#custom-tomato_clock.rest {
    background-color: #0069d4;
}
#custom-tomato_clock.manual {
    background-color: #cf5700;
}
```


`win + shift + c` restart sway

---



