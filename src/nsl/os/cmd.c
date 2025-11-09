#include "nsl/os/cmd.h"

NSL_API nsl_Error nsl_cmd_exec(const nsl_Cmd *cmd) {
    return nsl_cmd_exec_argv(cmd->len, cmd->items);
}

