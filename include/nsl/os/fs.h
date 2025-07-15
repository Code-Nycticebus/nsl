#ifndef _NSL_FS_H_
#define _NSL_FS_H_

#include "nsl/defines.h"

typedef struct {
    nsl_Path path;
    bool is_dir;
    usize size;
    u64 mtime;
} nsl_FsEntry;

typedef struct {
    nsl_Arena scratch;
    bool recursive;
    nsl_Error error;
    void *_handle;
} nsl_FsIter;

NSL_API nsl_Error nsl_fs_begin(nsl_FsIter* it, nsl_Path directory, bool recursive);
NSL_API void nsl_fs_end(nsl_FsIter *it);

NSL_API nsl_FsEntry *nsl_fs_next(nsl_FsIter *it);

NSL_API bool nsl_fs_exists(nsl_Path path);
NSL_API bool nsl_fs_is_dir(nsl_Path path);
NSL_API bool nsl_fs_remove(nsl_Path path);

#endif // _NSL_FS_H_
