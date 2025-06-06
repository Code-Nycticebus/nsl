#include "nc/os/fs.h"

#include "nc/core/error.h"
#include "nc/types/int.h"
#include "nc/types/path.h"

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
#    include <direct.h>
#    define mkdir(path, mode) _mkdir(path)
#    define stat _stat
#    define access(path, mode) _access(path, mode)
#else
#    include <unistd.h>
#endif

void nc_fs_mkdir(nc_Path path, nc_Error *error, nc_FsDirConfig config) {
    if (config.parents) {
        if (nc_path_is_root(path)) return;
        if (path.len == 1 && path.data[0] == '.') return;
        config.exists_ok = true;
        nc_fs_mkdir(nc_path_parent(path), error, config);
    }
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nc_usize_min(path.len, FILENAME_MAX - 1));
    if (mkdir(filepath, config.mode ? config.mode : 0755) != 0) {
        if (config.exists_ok && errno == EEXIST) {
            struct stat info;
            if (stat(filepath, &info) == 0 && S_ISDIR(info.st_mode)) return;
        }
        NC_ERROR_EMIT(error, errno, strerror(errno));
    }
}

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


