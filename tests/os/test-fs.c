#include "nsl.h"

static void test_fs_iteration(void) {
    bool is_empty = true;

    nsl_FsIter it = {0};
    nsl_Error error = nsl_fs_begin(&it, NSL_PATH("tests/"), true);
    for (nsl_FsEntry *e; (e = nsl_fs_next(&it));) {
        is_empty = false;
        break;
    }
    nsl_fs_end(&it);

    NSL_ASSERT(error == NSL_NO_ERROR && "Should not throw an error");
    NSL_ASSERT(is_empty == false && "the directory should not be empty");
}



void run_test_fs(void);
void run_test_fs(void) {
    test_fs_iteration();
}
