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

typedef struct nc_MapNode nc_MapNode;

typedef struct nc_Map {
    nc_MapType type;
    usize len;
    usize _cap;
    usize _del;
    nc_Arena *_arena;
    nc_MapNode *_nodes;
} nc_Map;

void nc_map_init(nc_Map *map, nc_Arena *arena);

void nc_map_clear(nc_Map *map);
void nc_map_update(nc_Map *map, nc_Map *other);

void nc_map_resize(nc_Map *map, usize size);
void nc_map_reserve(nc_Map *map, usize size);

bool nc_map_remove(nc_Map *map, u64 hash);

void nc_map_insert_i64(nc_Map *map, u64 hash, i64 value);
void nc_map_insert_u64(nc_Map *map, u64 hash, u64 value);
void nc_map_insert_f64(nc_Map *map, u64 hash, f64 value);
void nc_map_insert_ptr(nc_Map *map, u64 hash, void *value);

i64 *nc_map_get_i64(nc_Map *map, u64 hash);
u64 *nc_map_get_u64(nc_Map *map, u64 hash);
f64 *nc_map_get_f64(nc_Map *map, u64 hash);
void *nc_map_get_ptr(nc_Map *map, u64 hash);

const i64 *nc_map_get_i64_const(const nc_Map *map, u64 hash);
const u64 *nc_map_get_u64_const(const nc_Map *map, u64 hash);
const f64 *nc_map_get_f64_const(const nc_Map *map, u64 hash);
const void *nc_map_get_ptr_const(const nc_Map *map, u64 hash);

#endif // _NC_MAP_H_
