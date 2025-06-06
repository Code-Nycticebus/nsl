#include "nc/os/fs.h"

#include "nc/core/arena.h"
#include "nc/core/error.h"

#include "nc/types/str.h"

#include "nc/structs/list.h"

static void test_fs_iteration(void) {
    bool is_empty = true;

    nc_Error error = {0};
    nc_FsIter it = nc_fs_begin(NC_PATH("tests/"), true, &error);
    for (nc_FsEntry *e; (e = nc_fs_next(&it));) {
        is_empty = false;
        break;
    }
    nc_fs_end(&it);
    NC_ERROR_HANDLE(&error, NC_ASSERT(false && "Should not throw an error"));
    NC_ASSERT(is_empty == false && "the directory should not be empty");
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
