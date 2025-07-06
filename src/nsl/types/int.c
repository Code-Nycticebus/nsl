#include "nsl/types/int.h"

#include "nsl/types/byte.h"

#include <string.h>

#define BITS(T) (sizeof(T) * 8)

#define INTEGER_IMPL(T)                                                                            \
    NSL_API T nsl_##T##_reverse_bits(T value) {                                                    \
        T reversed = 0;                                                                            \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            reversed = (T)(reversed << 1);                                                         \
            if (value & 1) {                                                                       \
                reversed = reversed | 1;                                                           \
            }                                                                                      \
            value = value >> 1;                                                                    \
        }                                                                                          \
        return reversed;                                                                           \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_leading_ones(T value) {                                                \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> (BITS(T) - i - 1) & (T)0x1)) {                                          \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_trailing_ones(T value) {                                               \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> i & (T)0x1)) {                                                          \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_leading_zeros(T value) {                                               \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> (BITS(T) - i - 1) & (T)0x1) {                                             \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_trailing_zeros(T value) {                                              \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> i & (T)0x1) {                                                             \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_count_zeros(T value) {                                                 \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> i & (T)0x1)) {                                                          \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_count_ones(T value) {                                                  \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> i & (T)0x1) {                                                             \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_swap_bytes(T value) {                                                      \
        if (1 < sizeof(T)) {                                                                       \
            u8 *bytes = (u8 *)&value;                                                              \
            for (usize i = 0; i < (sizeof(T) + 1) / 2; i++) {                                      \
                u8 temp = bytes[i];                                                                \
                bytes[i] = bytes[sizeof(T) - i - 1];                                               \
                bytes[sizeof(T) - i - 1] = temp;                                                   \
            }                                                                                      \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_to_be(T value) {                                                           \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                                 \
            return nsl_##T##_swap_bytes(value);                                                    \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_be(T value) {                                                         \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                                 \
            return nsl_##T##_swap_bytes(value);                                                    \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_be_bytes(nsl_Bytes bytes) {                                           \
        NSL_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                     \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                                 \
            return nsl_##T##_swap_bytes(value);                                                    \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API nsl_Bytes nsl_##T##_to_be_bytes(T value, nsl_Arena *arena) {                           \
        u8 *buffer = nsl_arena_alloc(arena, sizeof(value));                                        \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nsl_bytes_from_parts(sizeof(value), buffer);                                        \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_to_le(T value) {                                                           \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                    \
            return nsl_##T##_swap_bytes(value);                                                    \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_le(T value) {                                                         \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                    \
            return nsl_##T##_swap_bytes(value);                                                    \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_le_bytes(nsl_Bytes bytes) {                                           \
        NSL_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                     \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                    \
            return nsl_##T##_swap_bytes(value);                                                    \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API nsl_Bytes nsl_##T##_to_le_bytes(T value, nsl_Arena *arena) {                           \
        u8 *buffer = nsl_arena_alloc(arena, sizeof(value));                                        \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                             \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nsl_bytes_from_parts(sizeof(value), buffer);                                        \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_ne_bytes(nsl_Bytes bytes) {                                           \
        NSL_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                     \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API nsl_Bytes nsl_##T##_to_ne_bytes(T value, nsl_Arena *arena) {                           \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                    \
            return nsl_##T##_to_be_bytes(value, arena);                                            \
        }                                                                                          \
        return nsl_##T##_to_le_bytes(value, arena);                                                \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_max(T a, T b) {                                                            \
        return a < b ? b : a;                                                                      \
    }                                                                                              \
    NSL_API T nsl_##T##_min(T a, T b) {                                                            \
        return a > b ? b : a;                                                                      \
    }                                                                                              \
    NSL_API T nsl_##T##_clamp(T min, T max, T value) {                                             \
        return value < min ? min : max < value ? max : value;                                      \
    }                                                                                              \
                                                                                                   \
    NSL_API u64 nsl_##T##_hash(T value) {                                                          \
        u64 hash = ((u64)value) + 1;                                                               \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (hash >> 16) ^ hash;                                                                \
        return hash;                                                                               \
    }                                                                                              \
                                                                                                   \
    NSL_API void nsl_##T##_swap(T *v1, T *v2) {                                                    \
        T temp = *v1;                                                                              \
        *v1 = *v2;                                                                                 \
        *v2 = temp;                                                                                \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_next_pow2(T n) {                                                           \
        /* https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 */                 \
        if (n == 0) return 1;                                                                      \
        u64 x = (u64)n;                                                                            \
        x--;                                                                                       \
        for (size_t i = 1; i < sizeof(T) * 8; i <<= 1) {                                           \
            x |= x >> i;                                                                           \
        }                                                                                          \
        T max = (((T)-1) > 0) ? (T)-1 : (T)((1ULL << (sizeof(T) * 8 - 1)) - 1);                    \
        if (x >= (u64)max) return max;                                                             \
        return (T)(x + 1);                                                                         \
    }

INTEGER_IMPL(u8)
INTEGER_IMPL(i8)
INTEGER_IMPL(u16)
INTEGER_IMPL(i16)
INTEGER_IMPL(u32)
INTEGER_IMPL(i32)
INTEGER_IMPL(u64)
INTEGER_IMPL(i64)
INTEGER_IMPL(usize)

#undef INTEGER_IMPL
#undef BITS
