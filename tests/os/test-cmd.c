#include "nc/os/cmd.h"

#include "nc/core/arena.h"
#include "nc/core/error.h"

void run_test_cmd(void);
void run_test_cmd(void) {
    nc_Arena arena = {0};
    nc_Cmd cmd = {0};
    nc_list_init(&cmd, &arena);

#if defined(_WIN32)
    nc_cmd_push(&cmd, "cmd.exe", "/c", "exit", "0");
#else
    nc_cmd_push(&cmd, "true");
#endif
    NC_ASSERT(nc_cmd_exec_list(&cmd) == NC_CMD_OK);
    nc_list_clear(&cmd);

    nc_cmd_push(&cmd, "not_existing_command");
    NC_ASSERT(nc_cmd_exec_list(&cmd) == NC_CMD_NOT_FOUND);
    nc_list_clear(&cmd);

    NC_ASSERT(nc_cmd_exec_list(&cmd) == NC_CMD_NOT_FOUND);
    nc_list_clear(&cmd);

    nc_arena_free(&arena);
}
