#include "nsl/os/os.h"

#include "nsl/core/arena.h"
#include "nsl/types/path.h"
#include "nsl/types/str.h"

#include <windows.h>
#include <string.h>
#include <errno.h>

NSL_API nsl_Error nsl_os_mkdir_(nsl_Path path, nsl_OsDirConfig config) {
    nsl_Error result = NSL_NO_ERROR;

    nsl_Arena arena = {0};

    if (config.parents) {
        if (nsl_path_is_root(path)) NSL_DEFER(NSL_NO_ERROR);
        if (path.len == 1 && path.data[0] == '.') NSL_DEFER(NSL_NO_ERROR);
        nsl_OsDirConfig c = config;
        c.exists_ok = true;
        nsl_Error recursive_error = nsl_os_mkdir_(nsl_path_parent(path), c);
        if (recursive_error) NSL_DEFER(recursive_error);
    }

    nsl_Str filepath = nsl_str_copy(path, &arena);

    if (!CreateDirectoryA(filepath.data, NULL)) {
        DWORD ec = GetLastError();
        if (config.exists_ok && ec == ERROR_ALREADY_EXISTS) {
            DWORD attrs = GetFileAttributes(filepath.data);
            if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                NSL_DEFER(NSL_NO_ERROR);
            }
        }
        if (ec == ERROR_ALREADY_EXISTS)     NSL_DEFER(NSL_ERROR_ALREADY_EXISTS);
        if (ec == ERROR_ACCESS_DENIED)      NSL_DEFER(NSL_ERROR_ACCESS_DENIED);
        if (ec == ERROR_PRIVILEGE_NOT_HELD) NSL_DEFER(NSL_ERROR_ACCESS_DENIED);
        if (ec == ERROR_PATH_NOT_FOUND)     NSL_DEFER(NSL_ERROR_FILE_NOT_FOUND);
        if (ec == ERROR_DIRECTORY)          NSL_DEFER(NSL_ERROR_NOT_DIRECTORY);

        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            msg, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

defer:
    nsl_arena_free(&arena);
    return result;
}

NSL_API nsl_Error nsl_os_chdir(nsl_Path path) {
    NSL_ASSERT(path.len >= MAX_PATH - 1 && "Filename is too long for windows to handle");

    char pathname[MAX_PATH] = {0};
    memcpy(pathname, path.data, path.len);

    if (!SetCurrentDirectoryA(pathname)) {
        DWORD ec = GetLastError();
        if (ec == ERROR_ALREADY_EXISTS)     return NSL_ERROR_ALREADY_EXISTS;
        if (ec == ERROR_ACCESS_DENIED)      return NSL_ERROR_ACCESS_DENIED;
        if (ec == ERROR_SHARING_VIOLATION)  return NSL_ERROR_ACCESS_DENIED;
        if (ec == ERROR_PATH_NOT_FOUND)     return NSL_ERROR_FILE_NOT_FOUND;
        if (ec == ERROR_DIRECTORY)          return NSL_ERROR_NOT_DIRECTORY;

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

NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena) {
    DWORD size = GetCurrentDirectoryA(0, NULL);
    if (size == 0) {
        DWORD ec = GetLastError();
        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            msg, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

    LPTSTR buffer = nsl_arena_alloc(arena, size);
    GetCurrentDirectoryA(size, buffer);
    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Arena *arena) {
    nsl_Arena scratch = {0};

    DWORD size = GetEnvironmentVariableA(env, NULL, 0);
    if (size == 0) {
        DWORD ec = GetLastError();
        if (ec == ERROR_ENVVAR_NOT_FOUND) return (nsl_Str){0};

        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            msg, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

    char *buffer = nsl_arena_calloc(&scratch, size);
    GetEnvironmentVariableA(env, buffer, size);

    nsl_Str result = nsl_str_copy(nsl_str_from_parts(size, buffer), arena);
    nsl_arena_free(&scratch);
    return result;
}

NSL_API bool nsl_os_exists(nsl_Path path) {
    NSL_ASSERT(path.len < MAX_PATH && "pathname is too long for windows to handle");

    char filepath[MAX_PATH] = {0};
    memcpy(filepath, path.data, path.len);
    DWORD dwAttrib = GetFileAttributesA(filepath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

NSL_API bool nsl_os_is_dir(nsl_Path path) {
    NSL_ASSERT(path.len < MAX_PATH && "pathname is too long for windows to handle");

    char filepath[MAX_PATH] = {0};
    memcpy(filepath, path.data, path.len);
    DWORD dwAttrib = GetFileAttributesA(filepath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

NSL_API nsl_Error nsl_os_remove(nsl_Path path) {
    NSL_ASSERT(path.len < MAX_PATH && "pathname is too long for windows to handle");

    char filepath[MAX_PATH] = {0};
    memcpy(filepath, path.data, path.len);
    if (DeleteFileA(filepath) == 0) {
        DWORD ec = GetLastError();
        if (ec == ERROR_ACCESS_DENIED)     return NSL_ERROR_ACCESS_DENIED;
        if (ec == ERROR_SHARING_VIOLATION) return NSL_ERROR_FILE_BUSY;
        if (ec == ERROR_FILE_NOT_FOUND)    return NSL_ERROR_FILE_NOT_FOUND;
        if (ec == ERROR_PATH_NOT_FOUND)    return NSL_ERROR_FILE_NOT_FOUND;

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

NSL_API bool nsl_os_older_than(nsl_Path p1, nsl_Path p2) {
    char filepath[MAX_PATH] = {0};
    FILETIME ft[2] = {0};
    HANDLE hFile[2] = {INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};

    if (p1.len > MAX_PATH || p2.len > MAX_PATH) return false;

    memcpy(filepath, p1.data, p1.len);
    filepath[p1.len] = '\0';
    hFile[0] = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile[0] == INVALID_HANDLE_VALUE)           NSL_DEFER(false);
    if (GetFileTimeA(hFile[0], NULL, NULL, &ft[0])) NSL_DEFER(false);

    memcpy(filepath, p2.data, p2.len);
    filepath[p2.len] = '\0';
    hFile[1] = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile[1] == INVALID_HANDLE_VALUE)           NSL_DEFER(false);
    if (GetFileTimeA(hFile[1], NULL, NULL, &ft[1])) NSL_DEFER(false);

    result = CompareFileTime(&ft[0], &ft[1]) == 1;
defer:
    if (hFile[0] != INVALID_HANDLE_VALUE) CloseHandle(hFile[0]);
    if (hFile[1] != INVALID_HANDLE_VALUE) CloseHandle(hFile[1]);
    return result;
}
