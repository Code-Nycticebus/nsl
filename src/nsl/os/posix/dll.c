#include "nsl/os/dll.h"

#include "nsl/core/error.h"
#include "nsl/types/int.h"
#include "nsl/types/str.h"
#include "nsl/os/fs.h"

#include <string.h>

#include <dlfcn.h>

NSL_API nsl_Error dll_load(nsl_Dll* dll, nsl_Path path) {
    if (!nsl_fs_exists(path)) {
        return NSL_ERROR_FILE_NOT_FOUND;
    }
    char lib_path[FILENAME_MAX] = {0};
    memcpy(lib_path, path.data, nsl_usize_min(path.len, FILENAME_MAX));

    dll->handle = dlopen(lib_path, RTLD_LAZY);
    if (dll->handle == NULL) {
        return NSL_ERROR;
    }

    return NSL_NO_ERROR;
}

NSL_API void dll_close(nsl_Dll *dll) {
    dlclose(dll->handle);
}

NSL_API Function dll_symbol(nsl_Dll *handle, nsl_Str symbol) {
    Function result = NULL;
    nsl_Arena arena = {0};

    const char* s = nsl_str_to_cstr(symbol, &arena);
    *(void **)(&result) = dlsym(handle, s);

    nsl_arena_free(&arena);
    return result;
}
