#include "nc/os/os.h"

#include "nc/core/error.h"
#include "nc/types/int.h"
#include "nc/types/path.h"
#include "nc/types/str.h"

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

void nc_os_mkdir(nc_Path path, nc_Error *error, nc_OsDirConfig config) {
    if (config.parents) {
        if (nc_path_is_root(path)) return;
        if (path.len == 1 && path.data[0] == '.') return;
        config.exists_ok = true;
        nc_os_mkdir(nc_path_parent(path), error, config);
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

void nc_os_chdir(nc_Path path, nc_Error* error) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nc_usize_min(path.len, FILENAME_MAX - 1));
    if (chdir(filepath) != 0) {
        NC_ERROR_EMIT(error, errno, strerror(errno));
    }
}

nc_Path nc_os_cwd(nc_Arena *arena, nc_Error *error) {
    errno = 0;
    char *buf = nc_arena_alloc(arena, FILENAME_MAX);
    char *ret = getcwd(buf, FILENAME_MAX - 1);
    if (ret == NULL) {
        NC_ERROR_EMIT(error, errno, strerror(errno));
        return (nc_Str){0};
    }
    return nc_str_from_cstr(ret);
}

nc_Str nc_os_getenv(const char *env, nc_Error *error) {
    const char *var = getenv(env);
    if (var == NULL) {
        NC_ERROR_EMIT(error, -1, env);
        return (nc_Str){0};
    }
    return nc_str_from_cstr(var);
}

