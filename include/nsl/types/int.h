#ifndef _NSL_INT_H_
#define _NSL_INT_H_

#include "nsl/core/arena.h"
#include "nsl/defines.h"

#define INTEGER_DECL(T)                                                                            \
    NSL_API NSL_CONST_FN T nsl_##T##_reverse_bits(T value);                                        \
    NSL_API NSL_CONST_FN usize nsl_##T##_leading_ones(T value);                                    \
    NSL_API NSL_CONST_FN usize nsl_##T##_trailing_ones(T value);                                   \
    NSL_API NSL_CONST_FN usize nsl_##T##_leading_zeros(T value);                                   \
    NSL_API NSL_CONST_FN usize nsl_##T##_trailing_zeros(T value);                                  \
    NSL_API NSL_CONST_FN usize nsl_##T##_count_zeros(T value);                                     \
    NSL_API NSL_CONST_FN usize nsl_##T##_count_ones(T value);                                      \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_swap_bytes(T value);                                          \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_to_be(T value);                                               \
    NSL_API NSL_CONST_FN T nsl_##T##_from_be(T value);                                             \
    NSL_API T nsl_##T##_from_be_bytes(nsl_Bytes bytes);                                            \
    NSL_API nsl_Bytes nsl_##T##_to_be_bytes(T value, nsl_Arena *arena);                            \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_to_le(T value);                                               \
    NSL_API NSL_CONST_FN T nsl_##T##_from_le(T value);                                             \
    NSL_API T nsl_##T##_from_le_bytes(nsl_Bytes bytes);                                            \
    NSL_API nsl_Bytes nsl_##T##_to_le_bytes(T value, nsl_Arena *arena);                            \
                                                                                                   \
    NSL_API T nsl_##T##_from_ne_bytes(nsl_Bytes bytes);                                            \
    NSL_API nsl_Bytes nsl_##T##_to_ne_bytes(T value, nsl_Arena *arena);                            \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_max(T a, T b);                                                \
    NSL_API NSL_CONST_FN T nsl_##T##_min(T a, T b);                                                \
    NSL_API NSL_CONST_FN T nsl_##T##_clamp(T min, T max, T value);                                 \
                                                                                                   \
    NSL_API NSL_CONST_FN u64 nsl_##T##_hash(T value);                                              \
    NSL_API void nsl_##T##_swap(T *v1, T *v2);                                                     \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_next_pow2(T n);

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

#endif // _NSL_INT_H_
