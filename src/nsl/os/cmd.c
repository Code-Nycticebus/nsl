#include "nsl/os/cmd.h"

nsl_CmdError nsl_cmd_exec_list(const nsl_Cmd *cmd) {
    return nsl_cmd_exec(cmd->len, cmd->items);
}

