#include "nc/structs/map.h"

#include "nc/types/int.h"

#include <string.h>

static void nc_map_insert(nc_Map *map, u64 hash, nc_MapValue value) {
    if (map->cap <= map->len + map->del) {
        nc_map_resize(map, map->cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (map->cap - 1);

        for (usize i = 0; i < map->cap; i++) {
            if (map->items[idx].hash == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    map->items[del_idx] = (nc_MapItem){.hash = hash, .value = value};
                    map->len++;
                    map->del--;
                    return;
                }
                map->items[idx] = (nc_MapItem){.hash = hash, .value = value};
                map->len++;
                return;
            } else if (map->items[idx].hash == hash) {
                map->items[idx].value = value;
                return;
            } else if (map->items[idx].hash == NC_MAP_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (map->cap - 1);
        }

        nc_map_resize(map, map->cap * 2);
    }

    NC_UNREACHABLE("nc_map_insert");
}

static nc_MapValue *nc_map_get(const nc_Map *map, u64 hash) {
    if (map->len == 0) {
        return NULL;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize idx = hash & (map->cap - 1);
    for (usize i = 0; i < map->cap; i++) {
        if (map->items[idx].hash == 0) {
            return NULL;
        }
        if (map->items[idx].hash == hash) {
            return &map->items[idx].value;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }

    return NULL;
}

void nc_map_init(nc_Map *map, nc_Arena *arena) {
    NC_ASSERT(map->items == NULL && "The map was already initialized");

    map->type = 0;
    map->len = 0;
    map->cap = 0;
    map->del = 0;
    map->arena = arena;
    map->items = NULL;
}

void nc_map_update(nc_Map *map, nc_Map *other) {
    NC_ASSERT(map->items && other->items);
    nc_map_reserve(map, other->len);
    for (usize i = 0; i < other->cap; ++i) {
        if (other->items[i].hash && other->items[i].hash != NC_MAP_DELETED) {
            nc_map_insert(map, other->items[i].hash, other->items[i].value);
        }
    }
}

void nc_map_extend(nc_Map* map, usize count, nc_MapItem* items) {
    nc_map_reserve(map, count);
    for (usize i = 0; i < count; i++) {
        nc_map_insert(map, items[i].hash, items[i].value);
    }
}

void nc_map_clear(nc_Map* map) {
    map->len = 0;
    map->del = 0;
    memset(map->items, 0, sizeof(map->items[0]) * map->cap);
}

void nc_map_resize(nc_Map *map, usize size) {
    if (size < map->cap) {
        return;
    }
    usize old_cap = map->cap;
    nc_MapItem *old_items = map->items;

    map->cap = size == 0 ? NC_MAP_DEFAULT_SIZE : nc_usize_next_pow2(size);
    map->items = nc_arena_calloc_chunk(map->arena, map->cap * sizeof(map->items[0]));

    map->len = 0;
    map->del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i].hash && old_items[i].hash != NC_MAP_DELETED) {
            nc_map_insert(map, old_items[i].hash, old_items[i].value);
        }
    }
    nc_arena_free_chunk(map->arena, old_items);
}

void nc_map_reserve(nc_Map *map, usize size) {
    usize target = map->len + size;
    if (target <= map->cap) return;
    nc_map_resize(map, target);
}

bool nc_map_remove(nc_Map *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize idx = hash & (map->cap - 1);
    for (usize i = 0; i < map->cap; i++) {
        if (map->items[idx].hash == 0) {
            return false;
        }
        if (map->items[idx].hash && map->items[idx].hash == hash) {
            map->items[idx].hash = NC_MAP_DELETED;
            map->len--;
            map->del++;
            return true;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }
    return false;
}

void nc_map_insert_i64(nc_Map *map, u64 hash, i64 value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_I64);
    nc_map_insert(map, hash, (nc_MapValue){.i64 = value});
}

void nc_map_insert_u64(nc_Map *map, u64 hash, u64 value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_U64);
    nc_map_insert(map, hash, (nc_MapValue){.u64 = value});
}

void nc_map_insert_f64(nc_Map *map, u64 hash, f64 value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_F64);
    nc_map_insert(map, hash, (nc_MapValue){.f64 = value});
}

void nc_map_insert_ptr(nc_Map *map, u64 hash, void *value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_PTR);
    nc_map_insert(map, hash, (nc_MapValue){.ptr = value});
}

i64 *nc_map_get_i64(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_I64);
    return &nc_map_get(map, hash)->i64;
}

u64 *nc_map_get_u64(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_U64);
    return &nc_map_get(map, hash)->u64;
}

f64 *nc_map_get_f64(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_F64);
    return &nc_map_get(map, hash)->f64;
}

void *nc_map_get_ptr(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_PTR);
    nc_MapValue* value = nc_map_get(map, hash);
    return value ? value->ptr : NULL;
}

const i64 *nc_map_get_i64_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_I64);
    return &nc_map_get(map, hash)->i64;
}

const u64 *nc_map_get_u64_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_U64);
    return &nc_map_get(map, hash)->u64;
}

const f64 *nc_map_get_f64_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_F64);
    return &nc_map_get(map, hash)->f64;
}

const void *nc_map_get_ptr_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_PTR);
    nc_MapValue* value = nc_map_get(map, hash);
    return value ? value->ptr : NULL;
}
