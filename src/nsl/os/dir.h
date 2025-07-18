#ifndef _NSL_FS_H_
#define _NSL_FS_H_

#include "nsl/defines.h"

typedef struct {
    nsl_Path path;
    bool is_dir;
    usize size;
    u64 mtime;
} nsl_DirEntry;

typedef struct {
    bool done;
    nsl_Arena scratch;
    bool recursive;
    void *_handle;
} nsl_DirIter;

#define nsl_dir_walk(e, dir, r)                                                                    \
    for (nsl_DirIter e##_iter = nsl_dir_begin(dir, r); !e##_iter.done; nsl_dir_end(&e##_iter))     \
        for (nsl_DirEntry *e = NULL; (e = nsl_dir_next(&e##_iter));)

NSL_API nsl_DirIter nsl_dir_begin(nsl_Path directory, bool recursive);
NSL_API void nsl_dir_end(nsl_DirIter *it);
NSL_API nsl_DirEntry *nsl_dir_next(nsl_DirIter *it);

#endif // _NSL_FS_H_
