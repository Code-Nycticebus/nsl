#include "nc/os/cmd.h"

#include "nc/core/arena.h"
#include "nc/core/error.h"
#include "nc/structs/list.h"

#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

nc_CmdError nc_cmd_exec(size_t argc, const char **argv) {
    if (argc == 0) return NC_CMD_NOT_FOUND;

    errno = 0;
    pid_t pid = fork();
    if (pid == -1) {
        NC_PANIC("fork failed");
    } else if (pid == 0) {
        nc_Arena arena = {0};

        nc_List(const char *) args = {0};
        nc_list_init(&args, &arena);

        nc_list_extend(&args, argc, argv);
        nc_list_push(&args, NULL);
        execvp(args.items[0], (char *const *)(void *)args.items);

        nc_arena_free(&arena);
        exit(127);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        nc_CmdError exit_code = WEXITSTATUS(status);
        return exit_code == 127 ? NC_CMD_NOT_FOUND : exit_code;
    }

    return NC_CMD_OK;
}
