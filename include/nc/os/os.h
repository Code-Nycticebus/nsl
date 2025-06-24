#ifndef _NC_OS_H_
#define _NC_OS_H_

#include "nc/defines.h"

typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // error when the directory exists
    bool parents;   // create parent paths
} nc_OsDirConfig;
void nc_os_mkdir(nc_Path path, nc_Error *error, nc_OsDirConfig config);

void nc_os_chdir(nc_Path path, nc_Error* error);
nc_Path nc_os_cwd(nc_Arena *arena, nc_Error *error);
nc_Str nc_os_getenv(const char *env, nc_Error *error);

#endif // _NC_OS_H_
