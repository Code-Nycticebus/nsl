#ifndef _NSL_MAP_H_
#define _NSL_MAP_H_

#include "nsl/core/arena.h"
#include "nsl/defines.h"

typedef struct {
    u64 hash;
    u64 value;
} nsl_MapItem;

typedef struct nsl_Map {
    nsl_Arena *arena;
    usize len;
    usize cap;
    usize del;
    nsl_MapItem *items;
} nsl_Map;

#define NSL_MAP_DEFAULT_SIZE 8
#define NSL_MAP_DELETED ((u64)0xdeaddeaddeaddead)

NSL_API void nsl_map_free(nsl_Map *map);
NSL_API void nsl_map_clear(nsl_Map *map);

NSL_API bool nsl_map_has(const nsl_Map *map, u64 hash);

NSL_API void nsl_map_update(nsl_Map *map, nsl_Map *other);
NSL_API void nsl_map_extend(nsl_Map *map, usize count, const nsl_MapItem *items);

NSL_API void nsl_map_resize(nsl_Map *map, usize size);
NSL_API void nsl_map_reserve(nsl_Map *map, usize size);

NSL_API bool nsl_map_remove(nsl_Map *map, u64 hash);

NSL_API bool nsl_map_insert(nsl_Map *map, u64 hash, u64 value);
NSL_API u64 *nsl_map_get(nsl_Map *map, u64 hash);
NSL_API u64 *nsl_map_get_or_insert(nsl_Map *map, u64 hash, u64 value);

NSL_API bool nsl_map_eq(const nsl_Map *map, const nsl_Map *other);
NSL_API bool nsl_map_subset(const nsl_Map *map, const nsl_Map *other);
NSL_API bool nsl_map_disjoint(const nsl_Map *map, const nsl_Map *other);

NSL_API void nsl_map_intersection(const nsl_Map *map, const nsl_Map *other, nsl_Map *out);
NSL_API void nsl_map_difference(const nsl_Map *map, const nsl_Map *other, nsl_Map *out);
NSL_API void nsl_map_union(const nsl_Map *map, const nsl_Map *other, nsl_Map *out);

#endif // _NSL_MAP_H_
