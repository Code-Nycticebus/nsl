#include "nsl.h"

static void test_dir_iteration(void) {
    bool is_empty = true;
    nsl_dir_walk(e, NSL_PATH("tests"), true) {
        is_empty = false;
        break;
    }
    NSL_ASSERT(is_empty == false && "the directory should not be empty");
}



void run_test_dir(void);
void run_test_dir(void) {
    test_dir_iteration();
}
