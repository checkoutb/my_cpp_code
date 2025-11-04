
#include "common.h"
#include <cstdlib>
#include <iostream>


// L R M
int main(int argc, char* argv[]) {
    if (argc == 1) {
        return EXIT_FAILURE;
    }

    int fd = common::open_fifo_write_non_block();

    if (common::is_left_click(argv[1][0])) {
        common::write_left_click(fd);
    } else if (common::is_right_click(argv[1][0])) {
        common::write_right_click(fd);
    } else if (common::is_middle_click(argv[1][0])) {
        common::write_middle_click(fd);
    } else {
        return EXIT_FAILURE;
    }
    
    common::closefd(fd);
    
    return 0;
}



