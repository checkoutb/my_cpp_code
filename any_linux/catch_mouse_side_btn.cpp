
#include <cstddef>
#include <cstdlib>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <string.h>



/*

没有驱动，所以鼠标侧键没有用。

sudo dnf install evtest
sudo evtest

ls /dev/input/ 只能看到符号，看不到代表了哪个硬件。  所以需要 evtest

不过搞不懂，下面哪个才是我的鼠标
/dev/input/event2:	INSTANT USB GAMING MOUSE 
/dev/input/event3:	INSTANT USB GAMING MOUSE  Keyboard
/dev/input/event4:	Raspberry Pi Pico Keyboard
/dev/input/event5:	Raspberry Pi Pico Mouse

4和5应该是键盘的，因为键盘是用树莓派的，但是不知道为什么键盘也能成为 mouse。

所以2才是我的鼠标？ 是的。 上面的3和5 进入后，操作键盘鼠标，没有任何反应

这个符号 是和 主机的USB端口绑定的？还是 启动时 分配的？   可以交换USB端口试下。 没试



gcc xxxx
sudo ./a.out


左 272
右 273
中 274
后 275
前 276


可以使用 xbindkeys 或 imwheel 来将 侧键绑定到某些快捷键

不过，我在想，这里的 open是 只读打开的，那么 我读写打开，然后往里写，会有用吗？
。。不太对，这个是鼠标。 给鼠标一个 ctrl+s 会是什么？
。所以 open 键盘的 可以？
。我先发个 中键试试。


xbindkeys  是 X window 的， Sway 是Wayland的。
imwheel 主要是为 鼠标滚轮添加功能的。  也是 X window的。


写什么。。。

..g

1. 可能因为没有fflush 或 fsync，导致 read无法立刻读到，需要 下次真实鼠标事件 时 才会读到
2. 这个感觉只是一个 日志，往里面写入东西，无法让 linux对 写入的 操作 进行 真正的执行。   。。因为我写入 中键，没有粘贴任何文本。

---

AI： 可以往 /dev/input/eventX 写入数据，但是取决于 具体设备和驱动。
大多数 eventx是只读的，写入会收到 EPERM 错误，或者写入失败
某些设备 允许写入，可以用来模拟 按键，鼠标移动 等。
推荐使用 uinput 子系统，而不是直接写入 eventX

。。可以了， 之前只捕捉了 EV_KEY，所以 只复制了 EV_KEY的 按下，释放。 捕捉全部后，补了 syn 可以 使用 侧键 模拟 中键粘贴了。

---

使用 uinput 试试

。。看来 监听 只能用 eventX，但是 好多事件。。。特别是 鼠标移动。。event太多了。。


.不过我也不知道我要什么.就是侧键绑定什么呢?

而且 emacs也有绑定，按下侧键，会提示 mouse-8 未定义，而且 鼠标移动时 侧键，提示 drag-mouse-8 未定义。

就绑定成 右shift吧。 不过只是demo，以后不会用的。


ok 可以切换 rime 的中英文。 但是 侧键确实没什么用。 至少linux上没什么用。当初是为了一键alt q。



*/


void init_write_device(int fd) {
    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "My Virtual KeyBoard");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x0001;  // 厂商ID
    uidev.id.product = 0x0002; // 产品ID
    uidev.id.version = 1;

    write(fd, &uidev, sizeof(uidev));

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_RIGHTSHIFT); // /usr/include/linux/input-event-codes.h 

    ioctl(fd, UI_DEV_CREATE);
}

void clean(int fd) {
    ioctl(fd, UI_DEV_DESTROY);
}

void send_key(int fd, int key, int value) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    
    gettimeofday(&ev.time, NULL);

    ev.type = EV_KEY;
    ev.code = key;
    ev.value = value;
    write(fd, &ev, sizeof(ev));

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd, &ev, sizeof(ev));
}

int main() {

    int read_fd = open("/dev/input/event2", O_RDONLY);
    if (read_fd < 0) {
        perror("failed open event2\n");
        return EXIT_FAILURE;
    }

    int write_fd = open("/dev/uinput", O_WRONLY);
    if (write_fd < 0) {
        perror("failed open uinput\n");
        return EXIT_FAILURE;
    }

    struct input_event ev;
    init_write_device(write_fd);

    int sz1 = sizeof(struct input_event);
    while (1) {
        if (read(read_fd, &ev, sz1) == sz1) {
            if (ev.code != 275) {
                continue;
            }
            if (ev.type == EV_KEY) {
                if (ev.value == 0) {
                    send_key(write_fd, KEY_RIGHTSHIFT, 1);
                    usleep(100000);
                    send_key(write_fd, KEY_RIGHTSHIFT, 0);
                }
            }
        }
    }

    clean(write_fd);   // while (1)  ...
    close(write_fd);
    return EXIT_SUCCESS;
}


// 鼠标侧键 附送一个 鼠标中键， 为了使用侧键来粘贴。
int main__ok() {

    const char* mouse_device = "/dev/input/event2";

    int fd = open(mouse_device, O_RDWR);

    if (fd == -1) {
        perror("failed open mouse device\n");
        return EXIT_FAILURE;
    }

    struct input_event ev;

    struct input_event midpush;
    struct input_event midrel;
    midpush.type = EV_KEY;
    midpush.code = 274;
    midpush.value = 1;
    midrel.type = EV_KEY;
    midrel.code = 274;
    midrel.value = 0;

    struct input_event syn;
    // struct input_event msc;
    syn.type = EV_SYN;
    syn.code = SYN_REPORT;
    syn.value = 0;
    // msc.type = EV_MSC;
    // msc.code = 4;
    // msc.value = 589827;
    
    while (1) {
        if (read(fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event)) {
            if (ev.type == EV_KEY) {
                if (ev.value == 1) {
                    printf("key press, %d\n", ev.code);
                } else if (ev.value == 0) {
                    printf("key relrase, %d\n", ev.code);

                    if (ev.code == 275) {
                        midpush.time = ev.time;
                        midrel.time = ev.time;
                        syn.time = ev.time;
                        // msc.time = ev.time;
                        size_t sz = sizeof(struct input_event);
                        
                        write(fd, &syn, sz);
                        // write(fd, &msc, sz);
                        int res = write(fd, &midpush, sizeof(struct input_event));
                        if (res != sizeof(struct input_event)) {
                            printf(".... error write\n");
                        }
                        write(fd, &syn, sz);
                        // write(fd, &msc, sz);
                        res = write(fd, &midrel, sizeof(struct input_event));
                        if (res != sizeof(struct input_event)) {
                            printf("..error write 2\n");
                        }
                    }
                }
            } else {
                // printf("key %d %d %d", ev.type, ev.code, ev.value);
            }
        }
    }
    close(fd);
    
    return EXIT_SUCCESS;
}


