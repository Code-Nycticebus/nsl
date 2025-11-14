#include "nsl/structs/map.h"

#include "nsl/types/int.h"

#include <string.h>

NSL_API void nsl_map_free(nsl_Map *map) {
    nsl_arena_free_chunk(map->arena, map->items);
}

NSL_API void nsl_map_clear(nsl_Map* map) {
    map->len = 0;
    map->del = 0;
    memset(map->items, 0, sizeof(map->items[0]) * map->cap);
}

NSL_API bool nsl_map_has(const nsl_Map *map, u64 hash) {
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
        if (map->items[idx].hash == hash) {
            return true;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }

    return false;
}

NSL_API void nsl_map_update(nsl_Map *map, nsl_Map *other) {
    nsl_map_reserve(map, other->len);
    for (usize i = 0; i < other->cap; ++i) {
        if (other->items[i].hash && other->items[i].hash != NSL_MAP_DELETED) {
            nsl_map_insert(map, other->items[i].hash, other->items[i].value);
        }
    }
}

NSL_API void nsl_map_extend(nsl_Map* map, usize count, const nsl_MapItem* items) {
    nsl_map_reserve(map, count);
    for (usize i = 0; i < count; i++) {
        nsl_map_insert(map, items[i].hash, items[i].value);
    }
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

bool nsl_map_insert(nsl_Map *map, u64 hash, u64 value) {
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
                    return true;
                }
                map->items[idx] = (nsl_MapItem){.hash = hash, .value = value};
                map->len++;
                return true;
            } else if (map->items[idx].hash == hash) {
                map->items[idx].value = value;
                return false;
            } else if (map->items[idx].hash == NSL_MAP_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (map->cap - 1);
        }

        nsl_map_resize(map, map->cap * 2);
    }

    NSL_UNREACHABLE("nsl_map_insert");
}

u64 *nsl_map_get(nsl_Map *map, u64 hash) {
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

NSL_API bool nsl_map_eq(const nsl_Map *map, const nsl_Map *other) {
  if (other->len != map->len) {
    return false;
  }

  if (other->cap < map->cap) {
    const nsl_Map *temp = map;
    map = other;
    other = temp;
  }
  for (usize i = 0; i < map->cap; i++) {
    if (map->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (!nsl_map_has(other, map->items[i].hash)) {
        return false;
      }
    }
  }
  return true;
}

NSL_API bool nsl_map_subset(const nsl_Map *map, const nsl_Map *other) {
  if (other->len < map->len) {
    return false;
  }
  for (usize i = 0; i < map->cap; i++) {
    if (map->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (!nsl_map_has(other, map->items[i].hash)) {
        return false;
      }
    }
  }
  return true;
}

NSL_API bool nsl_map_disjoint(const nsl_Map *map, const nsl_Map *other) {
  if (other->len == 0 || map->len == 0) {
    return true;
  }

  if (other->cap < map->cap) {
    const nsl_Map *temp = map;
    map = other;
    other = temp;
  }
  for (usize i = 0; i < map->cap; i++) {
    if (map->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (nsl_map_has(other, map->items[i].hash)) {
        return false;
      }
    }
  }
  return true;
}

NSL_API void nsl_map_intersection(const nsl_Map *map, const nsl_Map *other, nsl_Map *out) {
  if (other->cap < map->cap) {
    const nsl_Map *temp = map;
    map = other;
    other = temp;
  }

  nsl_map_reserve(out, nsl_usize_min(map->len, other->len) * 2);
  for (usize i = 0; i < map->cap; i++) {
    if (map->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (nsl_map_has(other, map->items[i].hash)) {
        nsl_map_insert(out, map->items[i].hash, other->items[i].value);
      }
    }
  }
}

NSL_API void nsl_map_difference(const nsl_Map *map, const nsl_Map *other, nsl_Map *out) {
  nsl_map_reserve(out, map->len * 2);
  for (usize i = 0; i < map->cap; i++) {
    if (map->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (!nsl_map_has(other, map->items[i].hash)) {
        nsl_map_insert(out, map->items[i].hash, other->items[i].value);
      }
    }
  }
}

NSL_API void nsl_map_union(const nsl_Map *map, const nsl_Map *other, nsl_Map *out) {
  for (usize i = 0; i < map->cap; i++) {
    if (map->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (!nsl_map_has(other, map->items[i].hash)) {
        nsl_map_insert(out, map->items[i].hash, other->items[i].value);
      }
    }
  }

  for (usize i = 0; i < other->cap; i++) {
    if (other->items[i].hash && map->items[i].hash != NSL_MAP_DELETED) {
      if (!nsl_map_has(map, other->items[i].hash)) {
        nsl_map_insert(out, other->items[i].hash, other->items[i].value);
      }
    }
  }
}
