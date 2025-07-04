#include "nsl/os/fs.h"

#include "nsl/core/error.h"
#include "nsl/types/int.h"
#include "nsl/types/path.h"

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

bool nsl_fs_exists(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    return access(filepath, 0) == 0;
}

bool nsl_fs_is_dir(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));

    struct stat info;
    if (stat(filepath, &info) == -1) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

bool nsl_fs_remove(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    return (unlink(filepath) != 0);
}
