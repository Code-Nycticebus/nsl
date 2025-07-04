#ifndef _NSL_OS_H_
#define _NSL_OS_H_

#include "nsl/defines.h"

typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // error when the directory exists
    bool parents;   // create parent paths
} nsl_OsDirConfig;
void nsl_os_mkdir(nsl_Path path, nsl_Error *error, nsl_OsDirConfig config);

void nsl_os_chdir(nsl_Path path, nsl_Error* error);
nsl_Path nsl_os_cwd(nsl_Arena *arena, nsl_Error *error);
nsl_Str nsl_os_getenv(const char *env, nsl_Error *error);

#endif // _NSL_OS_H_
