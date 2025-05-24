#include "nc/structs/map.h"

#include "nc/types/int.h"

#include <string.h>

typedef union {
    i64 i64;
    u64 u64;
    f64 f64;
    void* ptr;
    const void* const_ptr;
} nc_MapValue;

struct nc_MapNode {
    u64 hash;
    nc_MapValue value;
};

#define NC_MAP_DEFAULT_SIZE 8
#define NC_MAP_DELETED ((u64)0xdeaddeaddeaddead)

static inline usize nc_next_pow2(usize n) {
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
#if __SIZEOF_SIZE_T__ == 8
    n |= n >> 32;
#endif
    return n + 1;
}

static void nc_map_insert(nc_Map *map, u64 hash, nc_MapValue value) {
    if (map->_cap <= map->len + map->_del) {
        nc_map_resize(map, map->_cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (map->_cap - 1);

        for (usize i = 0; i < map->_cap; i++) {
            if (map->_nodes[idx].hash == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    map->_nodes[del_idx] = (nc_MapNode){.hash = hash, .value = value};
                    map->len++;
                    map->_del--;
                    return;
                }
                map->_nodes[idx] = (nc_MapNode){.hash = hash, .value = value};
                map->len++;
                return;
            } else if (map->_nodes[idx].hash == hash) {
                map->_nodes[idx].value = value;
                return;
            } else if (map->_nodes[idx].hash == NC_MAP_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (map->_cap - 1);
        }

        nc_map_resize(map, map->_cap * 2);
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

    usize idx = hash & (map->_cap - 1);
    for (usize i = 0; i < map->_cap; i++) {
        if (map->_nodes[idx].hash == 0) {
            return NULL;
        }
        if (map->_nodes[idx].hash == hash) {
            return &map->_nodes[idx].value;
        }
        idx = (idx + i * i) & (map->_cap - 1);
    }

    return NULL;
}

void nc_map_init(nc_Map *map, nc_Arena *arena) {
    assert(map && arena);
    assert(map->_nodes == NULL && "The map was already initialized");

    map->type = 0;
    map->len = 0;
    map->_cap = 0;
    map->_del = 0;
    map->_arena = arena;
    map->_nodes = NULL;
}

void nc_map_clear(nc_Map* map) {
    map->len = 0;
    map->_del = 0;
    memset(map->_nodes, 0, sizeof(map->_nodes[0]) * map->_cap);
}

void nc_map_update(nc_Map *map, nc_Map *other) {
    nc_map_reserve(map, other->len);
    for (usize i = 0; i < other->_cap; ++i) {
        if (other->_nodes[i].hash && other->_nodes[i].hash != NC_MAP_DELETED) {
            nc_map_insert(map, other->_nodes[i].hash, other->_nodes[i].value);
        }
    }
}

void nc_map_resize(nc_Map *map, usize size) {
    if (size < map->_cap) {
        return;
    }
    usize old_cap = map->_cap;
    nc_MapNode *old_nodes = map->_nodes;

    map->_cap = size == 0 ? NC_MAP_DEFAULT_SIZE : nc_next_pow2(size);
    map->_nodes = nc_arena_calloc_chunk(map->_arena, map->_cap * sizeof(map->_nodes[0]));

    map->len = 0;
    map->_del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_nodes[i].hash && old_nodes[i].hash != NC_MAP_DELETED) {
            nc_map_insert(map, old_nodes[i].hash, old_nodes[i].value);
        }
    }
    nc_arena_free_chunk(map->_arena, old_nodes);
}

void nc_map_reserve(nc_Map *map, usize size) {
    usize target = map->len + size;
    if (target <= map->_cap) return;
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

    usize idx = hash & (map->_cap - 1);
    for (usize i = 0; i < map->_cap; i++) {
        if (map->_nodes[idx].hash == 0) {
            return false;
        }
        if (map->_nodes[idx].hash && map->_nodes[idx].hash == hash) {
            map->_nodes[idx].hash = NC_MAP_DELETED;
            map->len--;
            map->_del++;
            return true;
        }
        idx = (idx + i * i) & (map->_cap - 1);
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
