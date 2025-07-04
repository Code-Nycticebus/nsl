#ifndef _NSL_PATH_H_
#define _NSL_PATH_H_ 

#include "nsl/core/arena.h"
#include "nsl/defines.h"

NSL_API nsl_Path nsl_path_join(usize len, const nsl_Path* parts, nsl_Arena* arena);
NSL_API nsl_Path nsl_path_normalize(nsl_Path path, nsl_Arena* arena);

NSL_API bool nsl_path_eq(nsl_Path p1, nsl_Path p2);
NSL_API bool nsl_path_is_absolute(nsl_Path path);
NSL_API bool nsl_path_is_root(nsl_Path path);

NSL_API nsl_Str nsl_path_name(nsl_Path path);
NSL_API nsl_Str nsl_path_suffix(nsl_Path path);
NSL_API nsl_Str nsl_path_stem(nsl_Path path);
NSL_API nsl_Path nsl_path_parent(nsl_Path path);

#endif // _NSL_PATH_H_
