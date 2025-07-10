#include "nsl/os/os.h"

#include "nsl/core/arena.h"
#include "nsl/types/path.h"

#include <windows.h>


NSL_API void nsl_os_mkdir(nsl_Path path, nsl_Error *error, nsl_OsDirConfig config) {
    if (config.parents) {
        if (nsl_path_is_root(path)) return;
        if (path.len == 1 && path.data[0] == '.') return;
        nsl_OsDirConfig c = config;
        c.exists_ok = true;
        nsl_os_mkdir(nsl_path_parent(path), error, c);
        NSL_ERROR_PROPAGATE(error, { goto defer; });
    }

    nsl_Arena arena = {0};
    nsl_Str filepath = nsl_str_copy(path, &arena);
    
    if (CreateDirectoryA(filepath.data, NULL) != 0) {
        DWORD ec = GetLastError();
        if (config.exists_ok && ec == ERROR_ALREADY_EXISTS) {
            DWORD attrs = GetFileAttributes(filepath.data);
            if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                goto defer;
            }
        }
        NSL_ERROR_EMIT(error, ec, "could not create directory");
    }

defer:
    nsl_arena_free(&arena);
}

// NSL_API void nsl_os_chdir(nsl_Path path, nsl_Error* error) {}

NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena, nsl_Error *error) {
    DWORD size = GetCurrentDirectory(0, NULL);
    if (size == 0) {
        DWORD ec = GetLastError();
        NSL_ERROR_EMIT(error, ec, "could not get current directory");
    }

    LPTSTR buffer = nsl_arena_alloc(arena, size);
    GetCurrentDirectory(size, buffer);
    return nsl_str_from_parts(size, buffer);
}

// NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Error *error) {}
