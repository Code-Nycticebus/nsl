#ifndef _NC_MAP_H_
#define _NC_MAP_H_

#include "nc/core/arena.h"
#include "nc/defines.h"

typedef enum {
    NC_MAP_DYNAMIC,
    NC_MAP_I64,
    NC_MAP_U64,
    NC_MAP_F64,
    NC_MAP_PTR,
} nc_MapType;

typedef union {
    i64 i64;
    u64 u64;
    f64 f64;
    void* ptr;
    const void* const_ptr;
} nc_MapValue;

typedef struct {
    u64 hash;
    nc_MapValue value;
} nc_MapItem;

typedef struct nc_Map {
    nc_MapType type;
    usize len;
    usize cap;
    usize del;
    nc_Arena *arena;
    nc_MapItem *items;
} nc_Map;

#define NC_MAP_DEFAULT_SIZE 8
#define NC_MAP_DELETED ((u64)0xdeaddeaddeaddead)

NC_API void nc_map_init(nc_Map *map, nc_MapType type, nc_Arena *arena);

NC_API void nc_map_update(nc_Map *map, nc_Map *other);
NC_API void nc_map_extend(nc_Map *map, usize count, nc_MapItem *items);

NC_API void nc_map_clear(nc_Map *map);

NC_API void nc_map_resize(nc_Map *map, usize size);
NC_API void nc_map_reserve(nc_Map *map, usize size);

NC_API bool nc_map_remove(nc_Map *map, u64 hash);

NC_API void nc_map_insert_i64(nc_Map *map, u64 hash, i64 value);
NC_API void nc_map_insert_u64(nc_Map *map, u64 hash, u64 value);
NC_API void nc_map_insert_f64(nc_Map *map, u64 hash, f64 value);
NC_API void nc_map_insert_ptr(nc_Map *map, u64 hash, void *value);

NC_API i64 *nc_map_get_i64(nc_Map *map, u64 hash);
NC_API u64 *nc_map_get_u64(nc_Map *map, u64 hash);
NC_API f64 *nc_map_get_f64(nc_Map *map, u64 hash);
NC_API void *nc_map_get_ptr(nc_Map *map, u64 hash);

NC_API const i64 *nc_map_get_i64_const(const nc_Map *map, u64 hash);
NC_API const u64 *nc_map_get_u64_const(const nc_Map *map, u64 hash);
NC_API const f64 *nc_map_get_f64_const(const nc_Map *map, u64 hash);
NC_API const void *nc_map_get_ptr_const(const nc_Map *map, u64 hash);

#endif // _NC_MAP_H_
