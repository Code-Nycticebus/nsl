#ifndef _NSL_SET_H_
#define _NSL_SET_H_ 

#include "nsl/defines.h"
#include "nsl/core/arena.h"

typedef struct {
    usize len;
    usize _cap;
    usize _del;
    nsl_Arena *arena;
    u64 *items;
} nsl_Set;

#define NSL_SET_DEFAULT_SIZE 8
#define NSL_SET_DELETED ((u64)0xdeaddeaddeaddead)

NSL_API void nsl_set_init(nsl_Set* set, nsl_Arena *arena);

NSL_API void nsl_set_resize(nsl_Set *set, usize size);
NSL_API void nsl_set_reserve(nsl_Set *set, usize size);

NSL_API bool nsl_set_remove(nsl_Set* set, u64 hash);

NSL_API bool nsl_set_add(nsl_Set *set, u64 hash);
NSL_API bool nsl_set_has(const nsl_Set *set, u64 hash);

NSL_API void nsl_set_update(nsl_Set* set, const nsl_Set* other);
NSL_API void nsl_set_extend(nsl_Set* set, usize count, const u64* hashes);

NSL_API bool nsl_set_eq(const nsl_Set* set, const nsl_Set* other);
NSL_API bool nsl_set_subset(const nsl_Set* set, const nsl_Set* other);
NSL_API bool nsl_set_disjoint(const nsl_Set* set, const nsl_Set* other);

NSL_API void nsl_set_intersection(const nsl_Set *set, const nsl_Set *other, nsl_Set* out);
NSL_API void nsl_set_difference(const nsl_Set *set, const nsl_Set *other, nsl_Set* out);
NSL_API void nsl_set_union(const nsl_Set *set, const nsl_Set *other, nsl_Set* out);

#endif // _NSL_SET_H_
