#include "nsl/os/dll.h"

#include "nsl/types/int.h"
#include "nsl/types/str.h"
#include "nsl/os/os.h"

#include <windows.h>

#include <string.h>

NSL_API nsl_Error nsl_dll_load(nsl_Dll *dll, nsl_Path path) {
    if (!nsl_os_exists(path)) {
        return NSL_ERROR_FILE_NOT_FOUND;
    }
    char lib_path[MAX_PATH] = {0};
    memcpy(lib_path, path.data, nsl_usize_max(path.len, FILENAME_MAX));

    char temp_path[MAX_PATH];
    GetTempPathA(MAX_PATH, temp_path);

    char temp_file_name[MAX_PATH];
    GetTempFileNameA(temp_path, TEXT("lib"), 0, temp_file_name);
    CopyFile(lib_path, temp_file_name, 0);

    dll->handle = LoadLibraryA(temp_file_name);
    if (dll->handle == NULL) {
        DWORD ec = GetLastError();
        if (ec == ERROR_FILE_NOT_FOUND) return NSL_ERROR_FILE_NOT_FOUND;
        if (ec == ERROR_PATH_NOT_FOUND) return NSL_ERROR_FILE_NOT_FOUND;
        if (ec == ERROR_ACCESS_DENIED)  return NSL_ERROR_ACCESS_DENIED;

        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            msg, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

    return NSL_NO_ERROR;
}

NSL_API void nsl_dll_close(nsl_Dll *dll) {
  char temp_file_name[MAX_PATH];
  GetModuleFileNameA(dll->handle, temp_file_name, MAX_PATH);
  FreeLibrary(dll->handle);
  DeleteFileA(temp_file_name);
}

NSL_API Function nsl_dll_symbol(nsl_Dll *dll, nsl_Str symbol) {
  nsl_Arena arena = {0};
  const char* s = nsl_str_to_cstr(symbol, &arena);
  Function fn = (Function)GetProcAddress(dll->handle, s);
  nsl_arena_free(&arena);
  return fn;
}
