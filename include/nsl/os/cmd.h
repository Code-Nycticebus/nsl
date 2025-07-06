#ifndef _NSL_CMD_H_
#define _NSL_CMD_H_

#include "nsl/defines.h"

#include "nsl/structs/list.h"

typedef enum {
    NSL_CMD_OK = 0,
    // the command return code 1-255
    NSL_CMD_NOT_FOUND = 256,
} nsl_CmdError;

typedef nsl_List(const char*) nsl_Cmd;

#define nsl_cmd_push(cmd, ...)                                                                     \
    nsl_list_extend(cmd,                                                                           \
        NSL_ARRAY_LEN((const char *[]){__VA_ARGS__}),                                              \
        (const char *[]){__VA_ARGS__}                                                              \
    )

#define NSL_CMD(...)                                                                               \
    nsl_cmd_exec(NSL_ARRAY_LEN((const char *[]){__VA_ARGS__}), (const char *[]){__VA_ARGS__})

NSL_API nsl_CmdError nsl_cmd_exec(usize argc, const char** argv);
NSL_API nsl_CmdError nsl_cmd_exec_list(const nsl_Cmd* args);

#endif // _NSL_CMD_H_
