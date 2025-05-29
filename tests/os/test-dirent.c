#include "nc/os/dirent.h"

void run_test_dirent(void);
void run_test_dirent(void) {
    nc_Error error = {0};

    nc_Path path = NC_PATH("tests/");
    nc_DirIter it = nc_dir_begin(path, true, NULL);
    for (nc_DirEntry *e; (e = nc_dir_next(&it));) {}
    nc_dir_end(&it);
    NC_ERROR_HANDLE(&error, {
        NC_ASSERT(false && "Should not throw an error");
    });
}
