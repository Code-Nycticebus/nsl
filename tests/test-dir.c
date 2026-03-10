#include "../nsl.h"

static void test_dir_iteration(void) {
    bool is_empty = true;
    nsl_dir_walk(e, NSL_PATH("tests"), true) {
        is_empty = false;
        break;
    }
    NSL_ASSERT(is_empty == false && "the directory should not be empty");
}

int main(void) {
    test_dir_iteration();
}
