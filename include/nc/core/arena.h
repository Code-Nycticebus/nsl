#ifndef _NC_ARENA_H_
#define _NC_ARENA_H_

#include "nc/defines.h"

typedef struct nc_Chunk nc_Chunk;

typedef struct {
    nc_Chunk *begin, *end;
} nc_Arena;


NC_API void nc_arena_free(nc_Arena *arena);

NC_API void *nc_arena_alloc(nc_Arena *arena, usize size);
NC_API void *nc_arena_calloc(nc_Arena *arena, usize size);
NC_API void nc_arena_reset(nc_Arena *arena);

NC_API usize nc_arena_size(nc_Arena *arena);
NC_API usize nc_arena_real_size(nc_Arena *arena);

////////////////////////////////////////////////////////////////////////////

NC_API void *nc_arena_alloc_chunk(nc_Arena *arena, usize size);
NC_API void *nc_arena_calloc_chunk(nc_Arena *arena, usize size);
NC_API void *nc_arena_realloc_chunk(nc_Arena *arena, void *ptr, usize size);
NC_API void nc_arena_free_chunk(nc_Arena *arena, void *ptr);

#endif // _NC_ARENA_H_
