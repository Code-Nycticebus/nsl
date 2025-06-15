#include "nc/os/cmd.h"

#include "nc/core/arena.h"
#include "nc/core/error.h"
#include "nc/structs/list.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void nc_cmd_exec_list(nc_Error *error, const nc_Cmd *cmd) {
    nc_cmd_exec(error, cmd->len, cmd->items);
}

#if defined(_WIN32)

static void _nc_cmd_win32_wrap(usize argc, const char **argv, nc_StrBuilder *sb) {
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

void cmd_exec(Error *error, size_t argc, const char **argv) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    nc_Arena arena = {0};

    nc_StringBuilder sb = {0};
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
        NC_ERROR_EMIT(error, (i32)exit_code, "command failed", exit_code);
        goto defer;
    }

defer:
    nc_arena_free(&arena);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

#else

#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

void nc_cmd_exec(nc_Error *error, size_t argc, const char **argv) {
    errno = 0;
    pid_t pid = fork();
    if (pid == -1) {
        NC_ERROR_EMIT(error, CMD_FORK, strerror(errno));
    } else if (pid == 0) {
        nc_Arena arena = {0};

        nc_List(const char *) args = {0};
        nc_list_init(&args, &arena);

        nc_list_extend(&args, argc, argv);
        nc_list_push(&args, NULL);
        execvp(args.items[0], (char *const *)(void *)args.items);

        nc_arena_free(&arena);
        exit(CMD_NOT_FOUND);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code == CMD_NOT_FOUND) {
            NC_ERROR_EMIT(error, CMD_NOT_FOUND, "command not found");
        } else if (exit_code != 0) {
            NC_ERROR_EMIT(error, exit_code, "command failed");
        }
    }
}

#endif
