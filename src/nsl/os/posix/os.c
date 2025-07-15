#include "nsl/os/os.h"

#include "nsl/core/error.h"
#include "nsl/types/int.h"
#include "nsl/types/path.h"
#include "nsl/types/str.h"

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

NSL_API nsl_Error nsl_os_mkdir(nsl_Path path, nsl_OsDirConfig config) {
    if (config.parents) {
        if (nsl_path_is_root(path)) return NSL_NO_ERROR;
        if (path.len == 1 && path.data[0] == '.') return NSL_NO_ERROR;;
        nsl_OsDirConfig c = config;
        c.exists_ok = true;
        nsl_Error recursive_error = nsl_os_mkdir(nsl_path_parent(path), c);
        if (recursive_error) return recursive_error;
    }

    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    if (mkdir(filepath, config.mode ? config.mode : 0755) != 0) {
        if (config.exists_ok && errno == EEXIST) {
            struct stat info;
            if (stat(filepath, &info) == 0 && S_ISDIR(info.st_mode)) return NSL_NO_ERROR;
        }
        if (errno == EACCES) return NSL_ERROR_ACCESS_DENIED;
        if (errno == EEXIST) return NSL_ERROR_ALREADY_EXISTS;
        if (errno == ENOTDIR) return NSL_ERROR_NOT_DIRECTORY;
        NSL_PANIC(strerror(errno));
    }
    return NSL_NO_ERROR;
}

NSL_API nsl_Error nsl_os_chdir(nsl_Path path) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    if (chdir(filepath) != 0) {
        if (errno == EACCES)  return NSL_ERROR_ACCESS_DENIED;
        if (errno == ENOENT)  return NSL_ERROR_FILE_NOT_FOUND;
        if (errno == ENOTDIR) return NSL_ERROR_NOT_DIRECTORY;
        NSL_PANIC(strerror(errno));
    }

    return NSL_NO_ERROR;
}

NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena) {
    errno = 0;
    char *temp_path = getcwd(NULL, 0);
    if (temp_path == NULL) {
        NSL_PANIC(strerror(errno));
    }
    nsl_Path path = nsl_str_copy(nsl_str_from_cstr(temp_path), arena);
    free(temp_path);
    return path;
}

NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Arena* arena) {
    const char *var = getenv(env);
    return var ? nsl_str_copy(nsl_str_from_cstr(var), arena) : (nsl_Str){0};
}

NSL_API bool nsl_os_exists(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    return access(filepath, 0) == 0;
}

NSL_API bool nsl_os_is_dir(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));

    struct stat info;
    if (stat(filepath, &info) == -1) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

NSL_API bool nsl_os_remove(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    return (unlink(filepath) != 0);
}
