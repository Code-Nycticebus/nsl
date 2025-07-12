#ifndef _NSL_OS_H_
#define _NSL_OS_H_

#include "nsl/defines.h"

typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // error when the directory exists
    bool parents;   // create parent paths
} nsl_OsDirConfig;
NSL_API nsl_Error nsl_os_mkdir(nsl_Path path, nsl_OsDirConfig config);

NSL_API nsl_Error nsl_os_chdir(nsl_Path path);
NSL_API nsl_Error nsl_os_cwd(nsl_Path* path, nsl_Arena *arena);
NSL_API nsl_Str nsl_os_getenv(const char *env);

#endif // _NSL_OS_H_
