#include "nc/os/cmd.h"

void nc_cmd_exec_list(nc_Error *error, const nc_Cmd *cmd) {
    nc_cmd_exec(error, cmd->len, cmd->items);
}

