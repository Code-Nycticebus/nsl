#ifndef _NC_SET_H_
#define _NC_SET_H_ 

#include "nc/defines.h"
#include "nc/core/arena.h"

typedef struct {
    usize len;
    usize _cap;
    usize _del;
    nc_Arena *arena;
    u64 *items;
} nc_Set;

#define NC_SET_DEFAULT_SIZE 8
#define NC_SET_DELETED ((u64)0xdeaddeaddeaddead)

NC_API void nc_set_init(nc_Set* set, nc_Arena *arena);

NC_API void nc_set_resize(nc_Set *set, usize size);
NC_API void nc_set_reserve(nc_Set *set, usize size);

NC_API bool nc_set_remove(nc_Set* set, u64 hash);

NC_API bool nc_set_add(nc_Set *set, u64 hash);
NC_API bool nc_set_has(const nc_Set *set, u64 hash);

NC_API void nc_set_update(nc_Set* set, const nc_Set* other);
NC_API void nc_set_extend(nc_Set* set, usize count, const u64* hashes);

NC_API bool nc_set_eq(const nc_Set* set, const nc_Set* other);
NC_API bool nc_set_subset(const nc_Set* set, const nc_Set* other);
NC_API bool nc_set_disjoint(const nc_Set* set, const nc_Set* other);

NC_API void nc_set_intersection(const nc_Set *set, const nc_Set *other, nc_Set* out);
NC_API void nc_set_difference(const nc_Set *set, const nc_Set *other, nc_Set* out);
NC_API void nc_set_union(const nc_Set *set, const nc_Set *other, nc_Set* out);

#endif // _NC_SET_H_
