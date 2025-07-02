#ifndef _NC_CMD_H_
#define _NC_CMD_H_

#include "nc/defines.h"

#include "nc/structs/list.h"

typedef enum {
    NC_CMD_OK = 0,
    // the command return code 1-255
    NC_CMD_NOT_FOUND = 256,
} nc_CmdError;

typedef nc_List(const char*) nc_Cmd;

#define nc_cmd_push(cmd, ...)                                                                      \
    nc_list_extend(cmd, NC_ARRAY_LEN((const char *[]){__VA_ARGS__}), (const char *[]){__VA_ARGS__})

nc_CmdError nc_cmd_exec(usize argc, const char** argv);
nc_CmdError nc_cmd_exec_list(const nc_Cmd* args);

#endif // _NC_CMD_H_
