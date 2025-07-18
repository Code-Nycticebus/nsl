#ifndef _NSL_ARENA_H_
#define _NSL_ARENA_H_

#include "nsl/defines.h"

NSL_API void nsl_arena_free(nsl_Arena *arena);

NSL_API void *nsl_arena_alloc(nsl_Arena *arena, usize size);
NSL_API void *nsl_arena_calloc(nsl_Arena *arena, usize size);
NSL_API void nsl_arena_reset(nsl_Arena *arena);

NSL_API usize nsl_arena_size(nsl_Arena *arena);
NSL_API usize nsl_arena_real_size(nsl_Arena *arena);

NSL_API void *nsl_arena_alloc_chunk(nsl_Arena *arena, usize size);
NSL_API void *nsl_arena_calloc_chunk(nsl_Arena *arena, usize size);
NSL_API void *nsl_arena_realloc_chunk(nsl_Arena *arena, void *ptr, usize size);
NSL_API void nsl_arena_free_chunk(nsl_Arena *arena, void *ptr);

#endif // _NSL_ARENA_H_
