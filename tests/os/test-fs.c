#include "nc/os/fs.h"

void run_test_fs(void);
void run_test_fs(void) {
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
