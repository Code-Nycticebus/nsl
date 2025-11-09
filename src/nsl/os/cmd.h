#ifndef _NSL_CMD_H_
#define _NSL_CMD_H_

#include "nsl/defines.h"

#include "nsl/structs/list.h"

typedef nsl_List(const char*) nsl_Cmd;

#define nsl_cmd_push(cmd, ...)                                                                     \
    nsl_list_extend(cmd,                                                                           \
        NSL_ARRAY_LEN((const char *[]){__VA_ARGS__}),                                              \
        (const char *[]){__VA_ARGS__}                                                              \
    )

#define NSL_CMD(...)                                                                               \
    nsl_cmd_exec_argv(NSL_ARRAY_LEN((const char *[]){__VA_ARGS__}), (const char *[]){__VA_ARGS__})

NSL_API nsl_Error nsl_cmd_exec_argv(usize argc, const char** argv);
NSL_API nsl_Error nsl_cmd_exec(const nsl_Cmd* args);

#endif // _NSL_CMD_H_
