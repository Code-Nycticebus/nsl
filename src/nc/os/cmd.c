#include "nc/os/cmd.h"

nc_CmdError nc_cmd_exec_list(const nc_Cmd *cmd) {
    return nc_cmd_exec(cmd->len, cmd->items);
}

