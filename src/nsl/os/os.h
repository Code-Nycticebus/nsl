#ifndef _NSL_OS_H_
#define _NSL_OS_H_

#include "nsl/defines.h"

typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // no error when the directory exists
    bool parents;   // create parent paths
} nsl_OsDirConfig;

#define nsl_os_mkdir(path, ...) nsl_os_mkdir_(path, (nsl_OsDirConfig){ __VA_ARGS__ }) 
NSL_API nsl_Error nsl_os_mkdir_(nsl_Path path, nsl_OsDirConfig config);

NSL_API nsl_Error nsl_os_chdir(nsl_Path path);
NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena);
NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Arena *arena);

NSL_API bool nsl_os_exists(nsl_Path path);
NSL_API bool nsl_os_is_dir(nsl_Path path);
NSL_API nsl_Error nsl_os_remove(nsl_Path path);

#endif // _NSL_OS_H_
