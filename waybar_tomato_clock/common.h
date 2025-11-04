
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>


namespace {
    
    const char* FIFO_FILE = "/tmp/waybar_tomato_clock_43f98we4";
    const char LEFT_CLICK = 'L';
    const char RIGHT_CLICK = 'R'; 
    const char MIDDLE_CLICK = 'M';
    const char NON_CLICK = '_';

    bool fifo_created = false;
    void create_fifo() {
        if (!fifo_created) {
            fifo_created = true;
            remove(FIFO_FILE);
            mkfifo(FIFO_FILE, 0666);
        }
    }
}

namespace common {

    // odr violation...
    int open_fifo_read_non_block() {
        create_fifo();
        int fd = open(FIFO_FILE, O_RDONLY | O_NONBLOCK);
        return fd;
    }

    int open_fifo_read_block() {
        create_fifo();
        int fd = open(FIFO_FILE, O_RDWR);
        return fd;
    }

    int open_fifo_write_non_block() {
        int fd = open(FIFO_FILE, O_WRONLY | O_NONBLOCK);
        return fd;
    }

    void write_left_click(int fd) {
        write(fd, &LEFT_CLICK, sizeof(LEFT_CLICK));
    }

    void write_right_click(int fd) {
        write(fd, &RIGHT_CLICK, sizeof(RIGHT_CLICK));
    }

    void write_middle_click(int fd) {
        write(fd, &MIDDLE_CLICK, sizeof(MIDDLE_CLICK));
    }

    // fd must non_block
    // return latest action
    char read_action_non_block(int fd) {
        char tmp = NON_CLICK;
        char ch = NON_CLICK;
        int t2 = 1;
        while (t2 > 0) {
            t2 = read(fd, &tmp, sizeof(tmp));
            if (tmp >= ' ')
                ch = tmp;
        }
        return ch;
    }

    char read_action_block(int fd) {
        char ch = NON_CLICK;
        read(fd, &ch, sizeof(ch));
        return ch;
    }

    bool is_left_click(char ch) {
        return ch == LEFT_CLICK;
    }

    bool is_right_click(char ch) {
        return ch == RIGHT_CLICK;
    }

    bool is_middle_click(char ch) {
        return ch == MIDDLE_CLICK;
    }

    bool is_non_click(char ch) {
        return NON_CLICK == ch;
    }

    void closefd(int fd) {
        close(fd);
    }
}


