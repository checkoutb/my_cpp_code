
#include "common.h"

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>
#include <format>

#include <sys/stat.h>






namespace {
    namespace config {
        const int WORK_MIN = 25;
        const int REST_MIN = 5;
    }

    const int COUNT_PER_MIN = 60;  // should always 60 (60sec=1min); but for test, you can set 1 for fast result
    const int SLEEP_SEC = 10;  // should a factor of COUNT_PER_MIN
    const int WORK_DOWN_COUNT = config::WORK_MIN * COUNT_PER_MIN / SLEEP_SEC;
    const int REST_DOWN_COUNT = config::REST_MIN * COUNT_PER_MIN / SLEEP_SEC;

    int fifo_fd = -1;
    int fifo_fd_block = -1;

    // use a variable to hold last status(work/rest/manual), if now status != last status, record (now status, clock time) to db(or append to file).
    void print_work(int work) {
        std::cout<<std::format("{{\"text\":\"Wk:{:2d}\",\"class\":\"work\"}}", work * SLEEP_SEC / COUNT_PER_MIN)<<std::endl;
    }
    void print_rest(int rest) {
        std::cout<<std::format("{{\"text\":\"Rt:{:2d}\",\"class\":\"rest\"}}", rest * SLEEP_SEC / COUNT_PER_MIN)<<std::endl;
    }
    void print_manual() {
        std::cout<<std::format("{{\"text\":\"Click\",\"class\":\"manual\"}}")<<std::endl;
    }
}

int main() {

    bool is_work = true;
    int worked_cnt = WORK_DOWN_COUNT;
    int rested_cnt = REST_DOWN_COUNT;

    fifo_fd = common::open_fifo_read_non_block();
    fifo_fd_block = common::open_fifo_read_block();
    while (true) {
        char action = common::read_action_non_block(fifo_fd);
        if (common::is_left_click(action)) {
            // start work
            is_work = true;
            worked_cnt = WORK_DOWN_COUNT + 1;
        } else if (common::is_right_click(action)) {
            // start rest
            is_work = false;
            rested_cnt = REST_DOWN_COUNT + 1;  // 因为下面会 --rested_cnt，所以 +1
        }
        
        if (is_work) {
            --worked_cnt;
            if (worked_cnt <= 0) {
                is_work = false;
                rested_cnt = REST_DOWN_COUNT;
            }
        } else {
            --rested_cnt;
            if (rested_cnt <= 0) {
                // 这里需要一个阻塞read，但是我只有非阻塞的fd。是增加一个阻塞fd 还是while循环？
                // clean fifo
                while (!common::is_non_click(common::read_action_non_block(fifo_fd))) {
                    ;
                }
                print_manual();
                // block wait action
                if (common::is_left_click(common::read_action_block(fifo_fd_block))) {
                    is_work = true;
                    worked_cnt = WORK_DOWN_COUNT;                    
                } else {
                    is_work = false;
                    rested_cnt = REST_DOWN_COUNT;
                }
                // 上面只读取了第一个字符，需要清空后面的其他字符和\n
                while (!common::is_non_click(common::read_action_non_block(fifo_fd))) {
                    ;
                }
                
            }
        }

        if (is_work) {
            print_work(worked_cnt);
        } else {
            print_rest(rested_cnt);
        }
        
        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(5min);
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_SEC));
    }

    common::closefd(fifo_fd);
    common::closefd(fifo_fd_block);
    
    return EXIT_SUCCESS;
}


