#pragma once

#include "nc/core/arena.h"
#include "nc/defines.h"

#define INTEGER_DECL(T)                                                                            \
    NC_API NC_CONST_FN T nc_##T##_reverse_bits(T value);                                           \
    NC_API NC_CONST_FN usize nc_##T##_leading_ones(T value);                                       \
    NC_API NC_CONST_FN usize nc_##T##_trailing_ones(T value);                                      \
    NC_API NC_CONST_FN usize nc_##T##_leading_zeros(T value);                                      \
    NC_API NC_CONST_FN usize nc_##T##_trailing_zeros(T value);                                     \
    NC_API NC_CONST_FN usize nc_##T##_count_zeros(T value);                                        \
    NC_API NC_CONST_FN usize nc_##T##_count_ones(T value);                                         \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_swap_bytes(T value);                                             \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_to_be(T value);                                                  \
    NC_API NC_CONST_FN T nc_##T##_from_be(T value);                                                \
    NC_API T nc_##T##_from_be_bytes(nc_Bytes bytes);                                               \
    NC_API nc_Bytes nc_##T##_to_be_bytes(T value, nc_Arena *arena);                                \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_to_le(T value);                                                  \
    NC_API NC_CONST_FN T nc_##T##_from_le(T value);                                                \
    NC_API T nc_##T##_from_le_bytes(nc_Bytes bytes);                                               \
    NC_API nc_Bytes nc_##T##_to_le_bytes(T value, nc_Arena *arena);                                \
                                                                                                   \
    NC_API T nc_##T##_from_ne_bytes(nc_Bytes bytes);                                               \
    NC_API nc_Bytes nc_##T##_to_ne_bytes(T value, nc_Arena *arena);                                \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_max(T a, T b);                                                   \
    NC_API NC_CONST_FN T nc_##T##_min(T a, T b);                                                   \
    NC_API NC_CONST_FN T nc_##T##_clamp(T min, T max, T value);                                    \
                                                                                                   \
    NC_API NC_CONST_FN u64 nc_##T##_hash(T value);                                                 \
    NC_API void nc_##T##_swap(T *v1, T *v2);                                                       \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_next_pow2(T n);

INTEGER_DECL(u8)
INTEGER_DECL(i8)
INTEGER_DECL(u16)
INTEGER_DECL(i16)
INTEGER_DECL(u32)
INTEGER_DECL(i32)
INTEGER_DECL(u64)
INTEGER_DECL(i64)
INTEGER_DECL(usize)

#undef INTEGER_DECL
