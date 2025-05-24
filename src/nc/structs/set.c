#include "nc/structs/set.h"

#include "nc/types/int.h"

#define NC_SET_DEFAULT_SIZE 8
#define NC_SET_DELETED ((u64)0xdeaddeaddeaddead)

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

void nc_set_init(nc_Set *set, nc_Arena *arena) {
    assert(set && arena);
    assert(set->items == NULL && "The map was already initialized");

    set->len = 0;
    set->_cap = 0;
    set->_del = 0;
    set->arena = arena;
    set->items = NULL;
}

void nc_set_resize(nc_Set *set, usize size) {
    if (size < set->_cap) {
        return;
    }
    usize old_cap = set->_cap;
    u64 *old_items = set->items;

    set->_cap = size == 0 ? NC_SET_DEFAULT_SIZE : nc_next_pow2(size);
    set->items = nc_arena_calloc_chunk(set->arena, set->_cap * sizeof(set->items[0]));

    set->len = 0;
    set->_del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i] && old_items[i] != NC_SET_DELETED) {
            nc_set_add(set, old_items[i]);
        }
    }
    nc_arena_free_chunk(set->arena, old_items);
}

void nc_set_reserve(nc_Set *map, usize size) {
    usize target = map->len + size;
    if (target <= map->_cap) return;
    nc_set_resize(map, target);
}

bool nc_set_remove(nc_Set *set, u64 hash) {
    if (set->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_SET_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize idx = hash & (set->_cap - 1);
    for (usize i = 0; i < set->_cap; i++) {
        if (set->items[idx] == 0) {
            return false;
        }
        if (set->items[idx] && set->items[idx] == hash) {
            set->items[idx] = NC_SET_DELETED;
            set->len--;
            set->_del++;
            return true;
        }
        idx = (idx + i * i) & (set->_cap - 1);
    }
    return false;
}

bool nc_set_add(nc_Set *set, u64 hash) {
    if (set->_cap <= set->len + set->_del) {
        nc_set_resize(set, set->_cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_SET_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (set->_cap - 1);

        for (usize i = 0; i < set->_cap; i++) {
            if (set->items[idx] == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    set->items[del_idx] = hash;
                    set->len++;
                    set->_del--;
                    return true;
                }
                set->items[idx] = hash;
                set->len++;
                return true;
            } else if (set->items[idx] == hash) {
                return false;
            } else if (set->items[idx] == NC_SET_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (set->_cap - 1);
        }

        nc_set_resize(set, set->_cap * 2);
    }

    NC_UNREACHABLE("nc_set_add");
}

bool nc_set_has(const nc_Set *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_SET_DELETED)) {
        hash = nc_u64_hash(hash);
    }

    usize idx = hash & (map->_cap - 1);
    for (usize i = 0; i < map->_cap; i++) {
        if (map->items[idx] == 0) {
            return false;
        }
        if (map->items[idx] == hash) {
            return true;
        }
        idx = (idx + i * i) & (map->_cap - 1);
    }

    return false;
}

void nc_set_update(nc_Set* map, const nc_Set* other) {
    nc_set_reserve(map, other->len);
    for (usize i = 0; i < other->_cap; ++i) {
        if (other->items[i] && other->items[i] != NC_SET_DELETED) {
            nc_set_add(map, other->items[i]);
        }
    }
}

void nc_set_extend(nc_Set* set, usize count, const u64* hashes) {
  nc_set_reserve(set, count);
  for (usize i = 0; i < count; i++) {
    nc_set_add(set, hashes[i]);
  }
}

bool nc_set_eq(const nc_Set *set, const nc_Set *other) {
  if (other->len != set->len) {
    return false;
  }

  if (other->_cap < set->_cap) {
    const nc_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

bool nc_set_subset(const nc_Set *set, const nc_Set *other) {
  if (other->len < set->len) {
    return false;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

bool nc_set_disjoint(const nc_Set *set, const nc_Set *other) {
  if (other->len == 0 || set->len == 0) {
    return true;
  }

  if (other->_cap < set->_cap) {
    const nc_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (nc_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

void nc_set_intersection(const nc_Set *set, const nc_Set *other, nc_Set* out) {
  if (other->_cap < set->_cap) {
    const nc_Set *temp = set;
    set = other;
    other = temp;
  }

  nc_set_reserve(out, nc_usize_min(set->len, other->len) * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (nc_set_has(other, set->items[i])) {
        nc_set_add(out, set->items[i]);
      }
    }
  }
}

void nc_set_difference(const nc_Set *set, const nc_Set *other, nc_Set* out) {
  nc_set_reserve(out, set->len * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        nc_set_add(out, set->items[i]);
      }
    }
  }
}

void nc_set_union(const nc_Set *set, const nc_Set *other, nc_Set* out) {
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        nc_set_add(out, set->items[i]);
      }
    }
  }

  for (usize i = 0; i < other->_cap; i++) {
    if (other->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(set, other->items[i])) {
        nc_set_add(out, other->items[i]);
      }
    }
  }
}
