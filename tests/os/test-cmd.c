#include "nsl/os/cmd.h"

#include "nsl/core/arena.h"
#include "nsl/core/error.h"

void run_test_cmd(void);
void run_test_cmd(void) {
    nsl_Arena arena = {0};
    nsl_Cmd cmd = {0};
    nsl_list_init(&cmd, &arena);

#if defined(_WIN32)
    nsl_cmd_push(&cmd, "cmd.exe", "/c", "exit", "0");
#else
    nsl_cmd_push(&cmd, "true");
#endif
    NSL_ASSERT(nsl_cmd_exec_list(&cmd) == NSL_CMD_OK);
    nsl_list_clear(&cmd);

    nsl_cmd_push(&cmd, "not_existing_command");
    NSL_ASSERT(nsl_cmd_exec_list(&cmd) == NSL_CMD_NOT_FOUND);
    nsl_list_clear(&cmd);

    NSL_ASSERT(nsl_cmd_exec_list(&cmd) == NSL_CMD_NOT_FOUND);
    nsl_list_clear(&cmd);

    nsl_arena_free(&arena);
}
