#include "nsl/os/os.h"

#include "nsl/types/int.h"
#include "nsl/types/path.h"
#include "nsl/types/str.h"

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define NSL_OS_PATH_MAX FILENAME_MAX

NSL_API nsl_Error nsl_os_mkdir_conf(nsl_Path path, nsl_OsDirConfig config) {
    if (path.len >= NSL_OS_PATH_MAX - 1) return NSL_ERROR_PATH_TOO_LONG;

    if (config.parents) {
        if (nsl_path_is_root(path))               return NSL_NO_ERROR;
        if (path.len == 1 && path.data[0] == '.') return NSL_NO_ERROR;;
        nsl_OsDirConfig c = config;
        c.exists_ok = true;
        nsl_Error recursive_error = nsl_os_mkdir_conf(nsl_path_parent(path), c);
        if (recursive_error) return recursive_error;
    }

    errno = 0;

    char filepath[NSL_OS_PATH_MAX] = {0};
    memcpy(filepath, path.data, path.len);
    filepath[path.len] = '\0';

    if (mkdir(filepath, config.mode ? config.mode : 0755) != 0) {
        if (config.exists_ok && errno == EEXIST) {
            struct stat info;
            if (stat(filepath, &info) == 0 && S_ISDIR(info.st_mode)) return NSL_NO_ERROR;
        }
        if (errno == EACCES)  return NSL_ERROR_ACCESS_DENIED;
        if (errno == EEXIST)  return NSL_ERROR_ALREADY_EXISTS;
        if (errno == ENOTDIR) return NSL_ERROR_NOT_DIRECTORY;
        NSL_PANIC(strerror(errno));
    }

    return NSL_NO_ERROR;
}

NSL_API nsl_Error nsl_os_chdir(nsl_Path path) {
    if (path.len >= NSL_OS_PATH_MAX - 1) return NSL_ERROR_PATH_TOO_LONG;

    errno = 0;

    char filepath[NSL_OS_PATH_MAX] = {0};
    memcpy(filepath, path.data, path.len);
    filepath[path.len] = '\0';

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
    if (path.len >= NSL_OS_PATH_MAX - 1) return false;

    char filepath[NSL_OS_PATH_MAX] = {0};
    memcpy(filepath, path.data, path.len);
    filepath[path.len] = '\0';

    return access(filepath, 0) == 0;
}

NSL_API bool nsl_os_is_dir(nsl_Path path) {
    if (path.len >= NSL_OS_PATH_MAX - 1) return NSL_ERROR_PATH_TOO_LONG;

    char filepath[NSL_OS_PATH_MAX] = {0};
    memcpy(filepath, path.data, path.len);
    filepath[path.len] = '\0';

    struct stat info;
    if (stat(filepath, &info) == -1) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

NSL_API nsl_Error nsl_os_remove(nsl_Path path) {
    if (path.len >= NSL_OS_PATH_MAX - 1) return NSL_ERROR_PATH_TOO_LONG;

    char filepath[NSL_OS_PATH_MAX] = {0};
    memcpy(filepath, path.data, path.len);
    filepath[path.len] = '\0';

    errno = 0;
    if (unlink(filepath) != 0) {
        if (errno == EACCES) return NSL_ERROR_ACCESS_DENIED;
        if (errno == ENOENT) return NSL_ERROR_FILE_NOT_FOUND;
        if (errno == EISDIR) return NSL_ERROR_NOT_DIRECTORY;
        if (errno == EBUSY)  return NSL_ERROR_FILE_BUSY;
        NSL_PANIC(strerror(errno));
    }

    return NSL_NO_ERROR;
}

NSL_API bool nsl_os_older_than(nsl_Path p1, nsl_Path p2) {
    if (p1.len >= NSL_OS_PATH_MAX - 1 || p1.len >= NSL_OS_PATH_MAX - 1) return false;

    char filepath[NSL_OS_PATH_MAX] = {0};
    struct stat info[2];

    memcpy(filepath, p1.data, p1.len);
    filepath[p1.len] = '\0';
    if (stat(filepath, &info[0]) == -1) return false;

    memcpy(filepath, p1.data, p1.len);
    filepath[p2.len] = '\0';
    if (stat(filepath, &info[1]) == -1) return false;

    return info[0].st_mtime < info[1].st_mtime;
}
