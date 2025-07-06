#include "nsl/os/os.h"

#include "nsl/core/error.h"
#include "nsl/types/int.h"
#include "nsl/types/path.h"
#include "nsl/types/str.h"

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

NSL_API void nsl_os_mkdir(nsl_Path path, nsl_Error *error, nsl_OsDirConfig config) {
    if (config.parents) {
        if (nsl_path_is_root(path)) return;
        if (path.len == 1 && path.data[0] == '.') return;
        config.exists_ok = true;
        nsl_os_mkdir(nsl_path_parent(path), error, config);
    }
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    if (mkdir(filepath, config.mode ? config.mode : 0755) != 0) {
        if (config.exists_ok && errno == EEXIST) {
            struct stat info;
            if (stat(filepath, &info) == 0 && S_ISDIR(info.st_mode)) return;
        }
        NSL_ERROR_EMIT(error, errno, strerror(errno));
    }
}

NSL_API void nsl_os_chdir(nsl_Path path, nsl_Error* error) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    if (chdir(filepath) != 0) {
        NSL_ERROR_EMIT(error, errno, strerror(errno));
    }
}

NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena, nsl_Error *error) {
    errno = 0;
    char *buf = nsl_arena_alloc(arena, FILENAME_MAX);
    char *ret = getcwd(buf, FILENAME_MAX - 1);
    if (ret == NULL) {
        NSL_ERROR_EMIT(error, errno, strerror(errno));
        return (nsl_Str){0};
    }
    return nsl_str_from_cstr(ret);
}

NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Error *error) {
    const char *var = getenv(env);
    if (var == NULL) {
        NSL_ERROR_EMIT(error, -1, env);
        return (nsl_Str){0};
    }
    return nsl_str_from_cstr(var);
}

