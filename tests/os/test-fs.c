#include "nsl.h"

#include "nsl.h"
#include "nsl.h"

#include "nsl.h"

#include "nsl.h"

static void test_fs_iteration(void) {
    bool is_empty = true;

    nsl_Error error = {0};
    nsl_FsIter it = nsl_fs_begin(NSL_PATH("tests/"), true, &error);
    for (nsl_FsEntry *e; (e = nsl_fs_next(&it));) {
        is_empty = false;
        break;
    }
    nsl_fs_end(&it);
    NSL_ERROR_HANDLE(&error, NSL_ASSERT(false && "Should not throw an error"));
    NSL_ASSERT(is_empty == false && "the directory should not be empty");
}



void run_test_fs(void);
void run_test_fs(void) {
    test_fs_iteration();
#if 0
    test_fs_open();
    test_fs_read_str();
    test_fs_read_line();
    test_fs_read_bytes();
#endif
}
