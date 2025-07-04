#include "nsl/structs/set.h"

#include "nsl/types/int.h"

NSL_API void nsl_set_init(nsl_Set *set, nsl_Arena *arena) {
    NSL_ASSERT(set && arena);
    NSL_ASSERT(set->items == NULL && "The map was already initialized");

    set->len = 0;
    set->_cap = 0;
    set->_del = 0;
    set->arena = arena;
    set->items = NULL;
}

NSL_API void nsl_set_resize(nsl_Set *set, usize size) {
    if (size < set->_cap) {
        return;
    }
    usize old_cap = set->_cap;
    u64 *old_items = set->items;

    set->_cap = size == 0 ? NSL_SET_DEFAULT_SIZE : nsl_usize_next_pow2(size);
    set->items = nsl_arena_calloc_chunk(set->arena, set->_cap * sizeof(set->items[0]));

    set->len = 0;
    set->_del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i] && old_items[i] != NSL_SET_DELETED) {
            nsl_set_add(set, old_items[i]);
        }
    }
    nsl_arena_free_chunk(set->arena, old_items);
}

NSL_API void nsl_set_reserve(nsl_Set *map, usize size) {
    usize target = map->len + size;
    if (target <= map->_cap) return;
    nsl_set_resize(map, target);
}

NSL_API bool nsl_set_remove(nsl_Set *set, u64 hash) {
    if (set->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_SET_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize idx = hash & (set->_cap - 1);
    for (usize i = 0; i < set->_cap; i++) {
        if (set->items[idx] == 0) {
            return false;
        }
        if (set->items[idx] && set->items[idx] == hash) {
            set->items[idx] = NSL_SET_DELETED;
            set->len--;
            set->_del++;
            return true;
        }
        idx = (idx + i * i) & (set->_cap - 1);
    }
    return false;
}

NSL_API bool nsl_set_add(nsl_Set *set, u64 hash) {
    if (set->_cap <= set->len + set->_del) {
        nsl_set_resize(set, set->_cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_SET_DELETED)) {
        hash = nsl_u64_hash(hash);
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
            } else if (set->items[idx] == NSL_SET_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (set->_cap - 1);
        }

        nsl_set_resize(set, set->_cap * 2);
    }

    NSL_UNREACHABLE("nsl_set_add");
}

NSL_API bool nsl_set_has(const nsl_Set *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_SET_DELETED)) {
        hash = nsl_u64_hash(hash);
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

NSL_API void nsl_set_update(nsl_Set* map, const nsl_Set* other) {
    nsl_set_reserve(map, other->len);
    for (usize i = 0; i < other->_cap; ++i) {
        if (other->items[i] && other->items[i] != NSL_SET_DELETED) {
            nsl_set_add(map, other->items[i]);
        }
    }
}

NSL_API void nsl_set_extend(nsl_Set* set, usize count, const u64* hashes) {
  nsl_set_reserve(set, count);
  for (usize i = 0; i < count; i++) {
    nsl_set_add(set, hashes[i]);
  }
}

NSL_API bool nsl_set_eq(const nsl_Set *set, const nsl_Set *other) {
  if (other->len != set->len) {
    return false;
  }

  if (other->_cap < set->_cap) {
    const nsl_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

NSL_API bool nsl_set_subset(const nsl_Set *set, const nsl_Set *other) {
  if (other->len < set->len) {
    return false;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

NSL_API bool nsl_set_disjoint(const nsl_Set *set, const nsl_Set *other) {
  if (other->len == 0 || set->len == 0) {
    return true;
  }

  if (other->_cap < set->_cap) {
    const nsl_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (nsl_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

NSL_API void nsl_set_intersection(const nsl_Set *set, const nsl_Set *other, nsl_Set* out) {
  if (other->_cap < set->_cap) {
    const nsl_Set *temp = set;
    set = other;
    other = temp;
  }

  nsl_set_reserve(out, nsl_usize_min(set->len, other->len) * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (nsl_set_has(other, set->items[i])) {
        nsl_set_add(out, set->items[i]);
      }
    }
  }
}

NSL_API void nsl_set_difference(const nsl_Set *set, const nsl_Set *other, nsl_Set* out) {
  nsl_set_reserve(out, set->len * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        nsl_set_add(out, set->items[i]);
      }
    }
  }
}

NSL_API void nsl_set_union(const nsl_Set *set, const nsl_Set *other, nsl_Set* out) {
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        nsl_set_add(out, set->items[i]);
      }
    }
  }

  for (usize i = 0; i < other->_cap; i++) {
    if (other->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(set, other->items[i])) {
        nsl_set_add(out, other->items[i]);
      }
    }
  }
}
