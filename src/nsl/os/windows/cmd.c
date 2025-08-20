#include "nsl/os/cmd.h"

#include "nsl/core/arena.h"
#include "nsl/structs/list.h"

#include <windows.h>

static void _nc_cmd_win32_wrap(usize argc, const char **argv, nsl_StrBuilder *sb) {
    // https://github.com/tsoding/nob.h/blob/45fa6efcd3e105bb4e39fa4cb9b57c19690d00a2/nob.h#L893
    for (usize i = 0; i < argc; i++) {
        if (0 < i) nsl_list_push(sb, ' ');
        const char *arg = argv[i];

        nsl_list_push(sb, '\"');
        usize backslashes = 0;
        while (*arg) {
            char c = *arg;
            if (c == '\\') {
                backslashes += 1;
            } else {
                if (c == '\"') {
                    for (size_t k = 0; k < 1 + backslashes; k++) {
                        nsl_list_push(sb, '\\');
                    }
                }
                backslashes = 0;
            }
            nsl_list_push(sb, c);
            arg++;
        }

        for (usize k = 0; k < backslashes; k++) {
            nsl_list_push(sb, '\\');
        }

        nsl_list_push(sb, '\"');
    }
}

NSL_API nsl_Error nsl_cmd_exec(size_t argc, const char **argv) {
    if (argc == 0) return NSL_ERROR_FILE_NOT_FOUND;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD result = 0;

    nsl_StrBuilder sb = {0};

    _nc_cmd_win32_wrap(argc, argv, &sb);
    nsl_list_push(&sb, '\0');

    if (!CreateProcessA(NULL, sb.items, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        DWORD ec = GetLastError();
        if (ec == ERROR_FILE_NOT_FOUND || ec == ERROR_PATH_NOT_FOUND) NSL_DEFER(NSL_ERROR_FILE_NOT_FOUND);

        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            msg, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    if (!GetExitCodeProcess(pi.hProcess, &result)) {
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

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
defer:
    nsl_list_free(&sb);
    return (nsl_Error)result;
}


