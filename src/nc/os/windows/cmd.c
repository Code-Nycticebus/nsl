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

void nc_cmd_exec(nc_Error *error, size_t argc, const char **argv) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    nc_Arena arena = {0};

    nc_StrBuilder sb = {0};
    nc_list_init(&sb, &arena);

    _nc_cmd_win32_wrap(argc, argv, &sb);
    nc_list_push(&sb, '\0');

    if (!CreateProcessA(NULL, sb.items, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        DWORD ec = GetLastError();
        if (ec == 0x2) {
            NC_ERROR_EMIT(error, CMD_NOT_FOUND, "command not found");
        } else {
            NC_ERROR_EMIT(error, (i32)ec, "command creation failed");
        }
        goto defer;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
        DWORD ec = GetLastError();
        NC_ERROR_EMIT(error, (i32)ec, "Could not get exit code of process");
        goto defer;
    }
    if (exit_code != 0) {
        NC_ERROR_EMIT(error, (i32)exit_code, "command failed");
        goto defer;
    }

defer:
    nc_arena_free(&arena);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


