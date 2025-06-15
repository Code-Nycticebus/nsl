#ifndef _NC_CMD_H_
#define _NC_CMD_H_

#include "nc/defines.h"

#include "nc/structs/list.h"

typedef enum {
  CMD_OK,
  CMD_FORK,
  CMD_NOT_FOUND = 127,
} nc_CmdError;

typedef nc_List(const char*) nc_Cmd;

#define nc_cmd_push(cmd, ...)                                                                      \
    nc_list_extend(cmd, NC_ARRAY_LEN((const char *[]){__VA_ARGS__}), (const char *[]){__VA_ARGS__})

void nc_cmd_exec(nc_Error* error, usize argc, const char** argv);
void nc_cmd_exec_list(nc_Error* error, const nc_Cmd* args);

#endif // _NC_CMD_H_
