#ifndef _NC_FS_H_
#define _NC_FS_H_

#include "nc/defines.h"

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

nc_FsIter nc_fs_begin(nc_Path directory, bool recursive, nc_Error *error);
void nc_fs_end(nc_FsIter *it);

nc_FsEntry *nc_fs_next(nc_FsIter *it);

typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // error when the directory exists
    bool parents;   // create parent paths
} nc_FsDirConfig;
void nc_fs_mkdir(nc_Path path, nc_Error *error, nc_FsDirConfig config);

bool nc_fs_exists(nc_Path path);
bool nc_fs_is_dir(nc_Path path);

#endif // _NC_FS_H_
