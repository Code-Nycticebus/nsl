#include "nc/core/arena.h"
#include "nc/core/error.h"
#include "nc/structs/list.h"

#include <windows.h>

static void _nc_cmd_win32_wrap(usize argc, const char **argv, nc_StrBuilder *sb) {
    // https://github.com/tsoding/nob.h/blob/45fa6efcd3e105bb4e39fa4cb9b57c19690d00a2/nob.h#L893
    for (usize i = 0; i < argc; i++) {
        if (0 < i) nc_list_push(sb, ' ');
        const char *arg = argv[i];

        nc_list_push(sb, '\"');
        usize backslashes = 0;
        while (*arg) {
            char c = *arg;
            if (c == '\\') {
                backslashes += 1;
            } else {
                if (c == '\"') {
                    for (size_t k = 0; k < 1 + backslashes; k++) {
                        nc_list_push(sb, '\\');
                    }
                }
                backslashes = 0;
            }
            nc_list_push(sb, c);
            arg++;
        }

        for (usize k = 0; k < backslashes; k++) {
            nc_list_push(sb, '\\');
        }

        nc_list_push(sb, '\"');
    }
}

nc_CmdError nc_cmd_exec(size_t argc, const char **argv) {
    if (argc == 0) return NC_CMD_NOT_FOUND;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD exit_code = 0;

    nc_Arena arena = {0};

    nc_StrBuilder sb = {0};
    nc_list_init(&sb, &arena);

    _nc_cmd_win32_wrap(argc, argv, &sb);
    nc_list_push(&sb, '\0');

    if (!CreateProcessA(NULL, sb.items, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        DWORD ec = GetLastError();
        if (ec == ERROR_FILE_NOT_FOUND || ec == ERROR_PATH_NOT_FOUND) {
            exit_code = NC_CMD_NOT_FOUND;
            goto defer;
        }

        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer, (DWORD)sizeof(msg), NULL
        );
        NC_PANIC(msg);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer, (DWORD)sizeof(msg), NULL
        );
        NC_PANIC(msg);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
defer:
    nc_arena_free(&arena);
    return (nc_CmdError)exit_code;
}


