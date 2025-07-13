#include "nsl.h"

void run_test_cmd(void);
void run_test_cmd(void) {
    nsl_Cmd cmd = {0};

#if defined(_WIN32)
    nsl_cmd_push(&cmd, "cmd.exe", "/c", "exit", "0");
#else
    nsl_cmd_push(&cmd, "true");
#endif
    NSL_ASSERT(nsl_cmd_exec_list(&cmd) == NSL_NO_ERROR);
    nsl_list_clear(&cmd);

    nsl_cmd_push(&cmd, "not_existing_command");
    NSL_ASSERT(nsl_cmd_exec_list(&cmd) == NSL_ERROR_FILE_NOT_FOUND);
    nsl_list_clear(&cmd);

    NSL_ASSERT(nsl_cmd_exec_list(&cmd) == NSL_ERROR_FILE_NOT_FOUND);
    nsl_list_clear(&cmd);

    nsl_list_free(&cmd);
}
