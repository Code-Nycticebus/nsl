#include "nc/os/fs.h"

#include "nc/core/error.h"
#include "nc/types/int.h"
#include "nc/types/path.h"

#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
#    include <direct.h>
#    include <io.h>
#    define stat _stat
#    define access(path, mode) _access(path, mode)
#    define unlink(path) _unlink(path)
#else
#    include <unistd.h>
#endif

bool nc_fs_exists(nc_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nc_usize_min(path.len, FILENAME_MAX - 1));
    return access(filepath, 0) == 0;
}

bool nc_fs_is_dir(nc_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nc_usize_min(path.len, FILENAME_MAX - 1));

    struct stat info;
    if (stat(filepath, &info) == -1) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

bool nc_fs_remove(nc_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nc_usize_min(path.len, FILENAME_MAX - 1));
    return (unlink(filepath) != 0);
}
