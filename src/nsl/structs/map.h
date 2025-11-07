#ifndef _NSL_MAP_H_
#define _NSL_MAP_H_

#include "nsl/core/arena.h"
#include "nsl/defines.h"

typedef enum {
    NSL_MAP_DYNAMIC,
    NSL_MAP_I64,
    NSL_MAP_U64,
    NSL_MAP_F64,
    NSL_MAP_PTR,
} nsl_MapType;

typedef union {
    i64 i64;
    u64 u64;
    f64 f64;
    void* ptr;
} nsl_MapValue;

typedef struct {
    u64 hash;
    nsl_MapValue value;
} nsl_MapItem;

typedef struct nsl_Map {
    nsl_Arena *arena;
    nsl_MapType type;
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

NSL_API void nsl_map_insert_i64(nsl_Map *map, u64 hash, i64 value);
NSL_API void nsl_map_insert_u64(nsl_Map *map, u64 hash, u64 value);
NSL_API void nsl_map_insert_f64(nsl_Map *map, u64 hash, f64 value);
NSL_API void nsl_map_insert_ptr(nsl_Map *map, u64 hash, void *value);

NSL_API i64 *nsl_map_get_i64(nsl_Map *map, u64 hash);
NSL_API u64 *nsl_map_get_u64(nsl_Map *map, u64 hash);
NSL_API f64 *nsl_map_get_f64(nsl_Map *map, u64 hash);
NSL_API void *nsl_map_get_ptr(nsl_Map *map, u64 hash);

NSL_API const i64 *nsl_map_get_i64_const(const nsl_Map *map, u64 hash);
NSL_API const u64 *nsl_map_get_u64_const(const nsl_Map *map, u64 hash);
NSL_API const f64 *nsl_map_get_f64_const(const nsl_Map *map, u64 hash);
NSL_API const void *nsl_map_get_ptr_const(const nsl_Map *map, u64 hash);

NSL_API bool nsl_map_eq(const nsl_Map *map, const nsl_Map *other);
NSL_API bool nsl_map_subset(const nsl_Map *map, const nsl_Map *other);
NSL_API bool nsl_map_disjoint(const nsl_Map *map, const nsl_Map *other);

NSL_API void nsl_map_intersection(const nsl_Map *map, const nsl_Map *other, nsl_Map *out);
NSL_API void nsl_map_difference(const nsl_Map *map, const nsl_Map *other, nsl_Map *out);
NSL_API void nsl_map_union(const nsl_Map *map, const nsl_Map *other, nsl_Map *out);

#endif // _NSL_MAP_H_
