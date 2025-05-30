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
} nc_FsEntry;

typedef struct {
    nc_Arena scratch; // per file scratch buffer
    bool recursive;   // recursive
    nc_Error *error;  // Error
    void *_handle;    // platform specific handle
} nc_FsIter;

nc_FsIter nc_fs_begin(nc_Path directory, bool recursive, nc_Error* error);
void nc_fs_end(nc_FsIter *it);

nc_FsEntry* nc_fs_next(nc_FsIter *it);

FILE* nc_fs_open(nc_Path path, const char* mode, nc_Error* error);
void nc_fs_close(FILE* file);

nc_Str nc_fs_read_str(FILE* file, usize size);
nc_Bytes nc_fs_read_bytes(FILE* file, usize size);

#endif // _NC_DIR_
