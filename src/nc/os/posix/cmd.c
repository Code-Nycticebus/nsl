#include "nc/os/cmd.h"

#include "nc/core/arena.h"
#include "nc/core/error.h"
#include "nc/structs/list.h"

#include <string.h>
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
