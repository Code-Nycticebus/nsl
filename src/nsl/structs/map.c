#include "nsl/structs/map.h"

#include "nsl/types/int.h"

#include <string.h>

static void nsl_map_insert(nsl_Map *map, u64 hash, nsl_MapValue value) {
    if (map->cap <= map->len + map->del) {
        nsl_map_resize(map, map->cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_MAP_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (map->cap - 1);

        for (usize i = 0; i < map->cap; i++) {
            if (map->items[idx].hash == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    map->items[del_idx] = (nsl_MapItem){.hash = hash, .value = value};
                    map->len++;
                    map->del--;
                    return;
                }
                map->items[idx] = (nsl_MapItem){.hash = hash, .value = value};
                map->len++;
                return;
            } else if (map->items[idx].hash == hash) {
                map->items[idx].value = value;
                return;
            } else if (map->items[idx].hash == NSL_MAP_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (map->cap - 1);
        }

        nsl_map_resize(map, map->cap * 2);
    }

    NSL_UNREACHABLE("nsl_map_insert");
}

static nsl_MapValue *nsl_map_get(const nsl_Map *map, u64 hash) {
    if (map->len == 0) {
        return NULL;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_MAP_DELETED)) {
        hash = nsl_u64_hash(hash);
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

NSL_API void nsl_map_init(nsl_Map *map, nsl_MapType type, nsl_Arena *arena) {
    NSL_ASSERT(map->items == NULL && "The map was already initialized");

    map->type = type;
    map->len = 0;
    map->cap = 0;
    map->del = 0;
    map->arena = arena;
    map->items = NULL;
}

NSL_API void nsl_map_update(nsl_Map *map, nsl_Map *other) {
    nsl_map_reserve(map, other->len);
    for (usize i = 0; i < other->cap; ++i) {
        if (other->items[i].hash && other->items[i].hash != NSL_MAP_DELETED) {
            nsl_map_insert(map, other->items[i].hash, other->items[i].value);
        }
    }
}

NSL_API void nsl_map_extend(nsl_Map* map, usize count, nsl_MapItem* items) {
    nsl_map_reserve(map, count);
    for (usize i = 0; i < count; i++) {
        nsl_map_insert(map, items[i].hash, items[i].value);
    }
}

NSL_API void nsl_map_clear(nsl_Map* map) {
    map->len = 0;
    map->del = 0;
    memset(map->items, 0, sizeof(map->items[0]) * map->cap);
}

NSL_API void nsl_map_resize(nsl_Map *map, usize size) {
    if (size < map->cap) {
        return;
    }
    usize old_cap = map->cap;
    nsl_MapItem *old_items = map->items;

    map->cap = size == 0 ? NSL_MAP_DEFAULT_SIZE : nsl_usize_next_pow2(size);
    map->items = nsl_arena_calloc_chunk(map->arena, map->cap * sizeof(map->items[0]));

    map->len = 0;
    map->del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i].hash && old_items[i].hash != NSL_MAP_DELETED) {
            nsl_map_insert(map, old_items[i].hash, old_items[i].value);
        }
    }
    nsl_arena_free_chunk(map->arena, old_items);
}

NSL_API void nsl_map_reserve(nsl_Map *map, usize size) {
    usize target = map->len + size;
    if (target <= map->cap) return;
    nsl_map_resize(map, target);
}

NSL_API bool nsl_map_remove(nsl_Map *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_MAP_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize idx = hash & (map->cap - 1);
    for (usize i = 0; i < map->cap; i++) {
        if (map->items[idx].hash == 0) {
            return false;
        }
        if (map->items[idx].hash && map->items[idx].hash == hash) {
            map->items[idx].hash = NSL_MAP_DELETED;
            map->len--;
            map->del++;
            return true;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }
    return false;
}

NSL_API void nsl_map_insert_i64(nsl_Map *map, u64 hash, i64 value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_I64);
    nsl_map_insert(map, hash, (nsl_MapValue){.i64 = value});
}

NSL_API void nsl_map_insert_u64(nsl_Map *map, u64 hash, u64 value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_U64);
    nsl_map_insert(map, hash, (nsl_MapValue){.u64 = value});
}

NSL_API void nsl_map_insert_f64(nsl_Map *map, u64 hash, f64 value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_F64);
    nsl_map_insert(map, hash, (nsl_MapValue){.f64 = value});
}

NSL_API void nsl_map_insert_ptr(nsl_Map *map, u64 hash, void *value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_PTR);
    nsl_map_insert(map, hash, (nsl_MapValue){.ptr = value});
}

NSL_API i64 *nsl_map_get_i64(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_I64);
    return (i64 *)nsl_map_get(map, hash);
}

NSL_API u64 *nsl_map_get_u64(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_U64);
    return (u64 *)nsl_map_get(map, hash);
}

NSL_API f64 *nsl_map_get_f64(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_F64);
    return (f64 *)nsl_map_get(map, hash);
}

NSL_API void *nsl_map_get_ptr(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_PTR);
    nsl_MapValue *value = nsl_map_get(map, hash);
    return value ? value->ptr : NULL;
}

NSL_API const i64 *nsl_map_get_i64_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_I64);
    return (const i64 *)nsl_map_get(map, hash);
}

NSL_API const u64 *nsl_map_get_u64_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_U64);
    return (const u64 *)nsl_map_get(map, hash);
}

NSL_API const f64 *nsl_map_get_f64_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_F64);
    return (const f64 *)nsl_map_get(map, hash);
}

NSL_API const void *nsl_map_get_ptr_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_PTR);
    nsl_MapValue *value = nsl_map_get(map, hash);
    return value ? value->ptr : NULL;
}
