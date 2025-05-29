#ifndef _NC_DIR_
#define _NC_DIR_

#include "nc/defines.h"
#include "nc/core/arena.h"
#include "nc/core/error.h"

typedef struct {
    nc_Path path;
    bool is_dir;
    usize size;
    u64 mtime;
} nc_DirEntry;

typedef struct {
    nc_Arena scratch; // per file scratch buffer
    bool recursive;   // recursive
    nc_Error *error;  // Error
    void *_handle;    // platform specific handle
} nc_DirIter;

nc_DirIter nc_dir_begin(nc_Path directory, bool recursive, nc_Error* error);
void nc_dir_end(nc_DirIter *it);

nc_DirEntry* nc_dir_next(nc_DirIter *it);

#endif // _NC_DIR_
