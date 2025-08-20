#include "nsl/os/cmd.h"

#include "nsl/core/arena.h"
#include "nsl/structs/list.h"

#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

NSL_API nsl_Error nsl_cmd_exec(size_t argc, const char **argv) {
    if (argc == 0) return NSL_ERROR_FILE_NOT_FOUND;

    errno = 0;
    pid_t pid = fork();
    if (pid == -1) {
        NSL_PANIC("fork failed");
    } else if (pid == 0) {
        nsl_List(const char *) args = {0};

        nsl_list_extend(&args, argc, argv);
        nsl_list_push(&args, NULL);
        execvp(args.items[0], (char *const *)(void *)args.items);

        nsl_list_free(&args);
        exit(127);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        nsl_Error exit_code = WEXITSTATUS(status);
        return exit_code == 127 ? NSL_ERROR_FILE_NOT_FOUND : exit_code;
    }

    return NSL_NO_ERROR;
}
