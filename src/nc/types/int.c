#include "nc/types/int.h"

#include "nc/types/byte.h"

#include <string.h>

#define BITS(T) (sizeof(T) * 8)

#define INTEGER_IMPL(T)                                                                            \
    NC_API T nc_##T##_reverse_bits(T value) {                                                      \
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
    NC_API usize nc_##T##_leading_ones(T value) {                                                  \
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
    NC_API usize nc_##T##_trailing_ones(T value) {                                                 \
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
    NC_API usize nc_##T##_leading_zeros(T value) {                                                 \
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
    NC_API usize nc_##T##_trailing_zeros(T value) {                                                \
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
    NC_API usize nc_##T##_count_zeros(T value) {                                                   \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> i & (T)0x1)) {                                                          \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API usize nc_##T##_count_ones(T value) {                                                    \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> i & (T)0x1) {                                                             \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_swap_bytes(T value) {                                                        \
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
    NC_API T nc_##T##_to_be(T value) {                                                             \
        if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                                   \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_be(T value) {                                                           \
        if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                                   \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_be_bytes(nc_Bytes bytes) {                                              \
        NC_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                                   \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API nc_Bytes nc_##T##_to_be_bytes(T value, nc_Arena *arena) {                               \
        u8 *buffer = nc_arena_alloc(arena, sizeof(value));                                         \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                  \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nc_bytes_from_parts(sizeof(value), buffer);                                         \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_to_le(T value) {                                                             \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_le(T value) {                                                           \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_le_bytes(nc_Bytes bytes) {                                              \
        NC_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API nc_Bytes nc_##T##_to_le_bytes(T value, nc_Arena *arena) {                               \
        u8 *buffer = nc_arena_alloc(arena, sizeof(value));                                         \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                               \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nc_bytes_from_parts(sizeof(value), buffer);                                         \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_ne_bytes(nc_Bytes bytes) {                                              \
        NC_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API nc_Bytes nc_##T##_to_ne_bytes(T value, nc_Arena *arena) {                               \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_to_be_bytes(value, arena);                                             \
        }                                                                                          \
        return nc_##T##_to_le_bytes(value, arena);                                                 \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_max(T a, T b) {                                                              \
        return a < b ? b : a;                                                                      \
    }                                                                                              \
    NC_API T nc_##T##_min(T a, T b) {                                                              \
        return a > b ? b : a;                                                                      \
    }                                                                                              \
    NC_API T nc_##T##_clamp(T min, T max, T value) {                                               \
        return value < min ? min : max < value ? max : value;                                      \
    }                                                                                              \
                                                                                                   \
    NC_API u64 nc_##T##_hash(T value) {                                                            \
        u64 hash = ((u64)value) + 1;                                                               \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (hash >> 16) ^ hash;                                                                \
        return hash;                                                                               \
    }                                                                                              \
                                                                                                   \
    NC_API void nc_##T##_swap(T *v1, T *v2) {                                                      \
        T temp = *v1;                                                                              \
        *v1 = *v2;                                                                                 \
        *v2 = temp;                                                                                \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_next_pow2(T n) {                                                             \
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
