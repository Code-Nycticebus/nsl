#include "nc/types/int.h"

#include "nc/types/byte.h"

#include <assert.h>

/* u8 */
static void test_u8_leading_bits(void) {
    assert(nc_u8_leading_ones(0xe0) == 3 && "Did not count correctly");
    assert(nc_u8_leading_zeros(0x1f) == 3 && "Did not count correctly");
    assert(nc_u8_trailing_ones(0x07) == 3 && "Did not count correctly");
    assert(nc_u8_trailing_zeros(0x08) == 3 && "Did not count correctly");
}

static void test_u8_swaping_bits(void) {
    assert(nc_u8_reverse_bits(0x12) == 0x48 && "Did not reverse correctly");
    assert(nc_u8_swap_bytes(0x12) == 0x12 && "Did not swap correctly");
}

static void test_u8_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u8_to_le(0x12) == 0x12 && "Bytes are somehow different");
    assert(nc_u8_to_be(0x12) == 0x12 && "Bytes are somehow different");
#else
    assert(nc_u8_to_le(0x12) == 0x12 && "Bytes are somehow different");
    assert(nc_u8_to_be(0x12) == 0x12 && "Bytes are somehow different");
#endif
}

static void test_u8_count_bits(void) {
    assert(nc_u8_count_ones(0x12) == 2 && "Did count correctly");
    assert(nc_u8_count_zeros(0x12) == 6 && "Did count correctly");
}

static void test_u8_from_bytes(void) {
    assert(nc_u8_from_be_bytes(NC_BYTES(0x12)) == 0x12 && "Conversion not correct");
    assert(nc_u8_from_le_bytes(NC_BYTES(0x12)) == 0x12 && "Conversion not correct");
    assert(nc_u8_from_ne_bytes(NC_BYTES(0x12)) == 0x12 && "Conversion not correct");
}

static void test_u8_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_u8_to_be_bytes(0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
    assert(nc_bytes_eq(nc_u8_to_le_bytes(0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_u8_to_ne_bytes(0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
#else
    assert(nc_bytes_eq(nc_u8_to_ne_bytes(0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
#endif
    nc_arena_free(&arena);
}

static void test_u8_hash(void) {
    assert(nc_u8_hash(0) == 0x130f353e031ba7);
    assert(nc_u8_hash(69) == 0x53620494cc400a2);
    assert(nc_u8_hash(42) == 0x333883cd70fb570);
}
/* u8 */

/* i8 */
static void test_i8_leading_bits(void) {
    assert(nc_i8_leading_ones((i8)0xe0) == 3 && "Did not count correctly");
    assert(nc_i8_leading_zeros((i8)0x1f) == 3 && "Did not count correctly");
    assert(nc_i8_trailing_ones((i8)0x07) == 3 && "Did not count correctly");
    assert(nc_i8_trailing_zeros((i8)0x08) == 3 && "Did not count correctly");
}

static void test_i8_swaping_bits(void) {
    assert(nc_i8_reverse_bits((i8)0x12) == 0x48 && "Did not reverse correctly");
    assert(nc_i8_swap_bytes((i8)0x12) == 0x12 && "Did not swap correctly");
}

static void test_i8_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i8_to_le((i8)0x1a) == 0x1a && "Bytes are somehow different");
    assert(nc_i8_to_be((i8)0x1a) == 0x1a && "Bytes are somehow different");
#else
    assert(nc_i8_to_le((i8)0x1a) == 0x1a && "Bytes are somehow different");
    assert(nc_i8_to_be((i8)0x1a) == 0x1a && "Bytes are somehow different");
#endif
}

static void test_i8_count_bits(void) {
    assert(nc_i8_count_ones((i8)0x12) == 2 && "Did count correctly");
    assert(nc_i8_count_zeros((i8)0x12) == 6 && "Did count correctly");
}

static void test_i8_from_bytes(void) {
    assert(nc_i8_from_be_bytes(NC_BYTES(0x12)) == 0x12 && "Conversion not correct");
    assert(nc_i8_from_le_bytes(NC_BYTES(0x12)) == 0x12 && "Conversion not correct");
    assert(nc_i8_from_ne_bytes(NC_BYTES(0x12)) == 0x12 && "Conversion not correct");
}

static void test_i8_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_i8_to_be_bytes((i8)0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
    assert(nc_bytes_eq(nc_i8_to_le_bytes((i8)0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_i8_to_ne_bytes(0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
#else
    assert(nc_bytes_eq(nc_i8_to_ne_bytes((i8)0x12, &arena), NC_BYTES(0x12)) &&
           "Not converted correctly");
#endif
    nc_arena_free(&arena);
}

static void test_i8_hash(void) {
    assert(nc_i8_hash(0) == 0x130f353e031ba7);
    assert(nc_i8_hash(69) == 0x53620494cc400a2);
    assert(nc_i8_hash(-69) == 0xe8a748836a8337d1);
    assert(nc_i8_hash(42) == 0x333883cd70fb570);
}
/* i8 */

/* u16 */
static void test_u16_leading_bits(void) {
    assert(nc_u16_leading_ones(0xe001) == 3 && "Did not count correctly");
    assert(nc_u16_leading_zeros(0x1f00) == 3 && "Did not count correctly");
    assert(nc_u16_trailing_ones(0x1007) == 3 && "Did not count correctly");
    assert(nc_u16_trailing_zeros(0x1008) == 3 && "Did not count correctly");
}

static void test_u16_swaping_bits(void) {
    assert(nc_u16_reverse_bits(0x1234) == 0x2c48 && "Did not reverse correctly");
    assert(nc_u16_swap_bytes(0x1234) == 0x3412 && "Did not swap correctly");
}

static void test_u16_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u16_to_le(0x1234) == 0x3412 && "Bytes are somehow different");
    assert(nc_u16_to_be(0x1234) == 0x1234 && "Bytes are somehow different");
#else
    assert(nc_u16_to_le(0x1234) == 0x1234 && "Bytes are somehow different");
    assert(nc_u16_to_be(0x1234) == 0x3412 && "Bytes are somehow different");
#endif
}

static void test_u16_count_bits(void) {
    assert(nc_u16_count_ones(0x1234) == 5 && "Did count correctly");
    assert(nc_u16_count_zeros(0x1234) == 11 && "Did count correctly");
}

static void test_u16_from_bytes(void) {
    assert(nc_u16_from_be_bytes(NC_BYTES(0x12, 0x34)) == 0x1234 && "Conversion not correct");
    assert(nc_u16_from_le_bytes(NC_BYTES(0x34, 0x12)) == 0x1234 && "Conversion not correct");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u16_from_ne_bytes(NC_BYTES(0x12, 0x34)) == 0x1234 && "Not converted correctly");
#else
    assert(nc_u16_from_ne_bytes(NC_BYTES(0x34, 0x12)) == 0x1234 && "Not converted correctly");
#endif
}

static void test_u16_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_u16_to_be_bytes(0x1234, &arena), NC_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
    assert(nc_bytes_eq(nc_u16_to_le_bytes(0x1234, &arena), NC_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_u16_to_ne_bytes(0x1234, &arena), NC_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
#else
    assert(nc_bytes_eq(nc_u16_to_ne_bytes(0x1234, &arena), NC_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nc_arena_free(&arena);
}

static void test_u16_hash(void) {
    assert(nc_u16_hash(0) == 0x130f353e031ba7);
    assert(nc_u16_hash(69) == 0x53620494cc400a2);
    assert(nc_u16_hash(42) == 0x333883cd70fb570);
}
/* u16 */

/* i16 */
static void test_i16_leading_bits(void) {
    assert(nc_i16_leading_ones((i16)0xe001) == 3 && "Did not count correctly");
    assert(nc_i16_leading_zeros((i16)0x1f00) == 3 && "Did not count correctly");
    assert(nc_i16_trailing_ones((i16)0x1007) == 3 && "Did not count correctly");
    assert(nc_i16_trailing_zeros((i16)0x1008) == 3 && "Did not count correctly");
}

static void test_i16_swaping_bits(void) {
    assert(nc_i16_reverse_bits(0x1234) == 0x2c48 && "Did not reverse correctly");
    assert(nc_i16_swap_bytes(0x1234) == 0x3412 && "Did not swap correctly");
}

static void test_i16_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i16_to_le(0x1234) == 0x3412 && "Bytes are somehow different");
    assert(nc_i16_to_be(0x1234) == 0x1234 && "Bytes are somehow different");
#else
    assert(nc_i16_to_le(0x1234) == 0x1234 && "Bytes are somehow different");
    assert(nc_i16_to_be(0x1234) == 0x3412 && "Bytes are somehow different");
#endif
}

static void test_i16_count_bits(void) {
    assert(nc_i16_count_ones(0x1234) == 5 && "Did count correctly");
    assert(nc_i16_count_zeros(0x1234) == 11 && "Did count correctly");
}

static void test_i16_from_bytes(void) {
    assert(nc_i16_from_be_bytes(NC_BYTES(0x12, 0x34)) == 0x1234 && "Conversion not correct");
    assert(nc_i16_from_le_bytes(NC_BYTES(0x34, 0x12)) == 0x1234 && "Conversion not correct");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i16_from_ne_bytes(NC_BYTES(0x12, 0x34)) == 0x1234 && "Not converted correctly");
#else
    assert(nc_i16_from_ne_bytes(NC_BYTES(0x34, 0x12)) == 0x1234 && "Not converted correctly");
#endif
}

static void test_i16_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_i16_to_be_bytes(0x1234, &arena), NC_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
    assert(nc_bytes_eq(nc_i16_to_le_bytes(0x1234, &arena), NC_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_i16_to_ne_bytes(0x1234, &arena), NC_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
#else
    assert(nc_bytes_eq(nc_i16_to_ne_bytes(0x1234, &arena), NC_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nc_arena_free(&arena);
}

static void test_i16_hash(void) {
    assert(nc_i16_hash(0) == 0x130f353e031ba7);
    assert(nc_i16_hash(69) == 0x53620494cc400a2);
    assert(nc_i16_hash(42) == 0x333883cd70fb570);
}
/* i16 */

/* u32 */
static void test_u32_leading_bits(void) {
    assert(nc_u32_leading_ones(0xe0000001) == 3 && "Did not count correctly");
    assert(nc_u32_leading_zeros(0x1f000000) == 3 && "Did not count correctly");
    assert(nc_u32_trailing_ones(0x80000007) == 3 && "Did not count correctly");
    assert(nc_u32_trailing_zeros(0x80000008) == 3 && "Did not count correctly");
}

static void test_u32_swaping_bits(void) {
    assert(nc_u32_reverse_bits(0x12345678) == 0x1e6a2c48 && "Did not reverse correctly");
    assert(nc_u32_swap_bytes(0x12345678) == 0x78563412 && "Did not swap correctly");
}

static void test_u32_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u32_to_le(0x12345678) == 0x78563412 && "Bytes are somehow different");
    assert(nc_u32_to_be(0x12345678) == 0x12345678 && "Bytes are somehow different");
#else
    assert(nc_u32_to_le(0x12345678) == 0x12345678 && "Bytes are somehow different");
    assert(nc_u32_to_be(0x12345678) == 0x78563412 && "Bytes are somehow different");
#endif
}

static void test_u32_count_bits(void) {
    assert(nc_u32_count_ones(0x12345678) == 13 && "Did count correctly");
    assert(nc_u32_count_zeros(0x12345678) == 19 && "Did count correctly");
}

static void test_u32_from_bytes(void) {
    assert(nc_u32_from_be_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Conversion not correct");
    assert(nc_u32_from_le_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Conversion not correct");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u32_from_ne_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Not converted correctly");
#else
    assert(nc_u32_from_ne_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Not converted correctly");
#endif
}

static void test_u32_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_u32_to_be_bytes(0x12345678, &arena), NC_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
    assert(nc_bytes_eq(nc_u32_to_le_bytes(0x12345678, &arena), NC_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_u32_to_ne_bytes(0x12345678, &arena), NC_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
#else
    assert(nc_bytes_eq(nc_u32_to_ne_bytes(0x12345678, &arena), NC_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nc_arena_free(&arena);
}

static void test_u32_hash(void) {
    assert(nc_u32_hash(0) == 0x130f353e031ba7);
    assert(nc_u32_hash(69) == 0x53620494cc400a2);
    assert(nc_u32_hash(42) == 0x333883cd70fb570);
}
/* u32 */

/* i32 */
static void test_i32_leading_bits(void) {
    assert(nc_i32_leading_ones((i32)0xe0000001) == 3 && "Did not count correctly");
    assert(nc_i32_leading_zeros((i32)0x1f000000) == 3 && "Did not count correctly");
    assert(nc_i32_trailing_ones((i32)0x80000007) == 3 && "Did not count correctly");
    assert(nc_i32_trailing_zeros((i32)0x80000008) == 3 && "Did not count correctly");
}

static void test_i32_swaping_bits(void) {
    assert(nc_i32_reverse_bits(0x12345678) == 0x1e6a2c48 && "Did not reverse correctly");
    assert(nc_i32_swap_bytes(0x12345678) == 0x78563412 && "Did not swap correctly");
}

static void test_i32_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i32_to_le(0x12345678) == 0x78563412 && "Bytes are somehow different");
    assert(nc_i32_to_be(0x12345678) == 0x12345678 && "Bytes are somehow different");
#else
    assert(nc_i32_to_le(0x12345678) == 0x12345678 && "Bytes are somehow different");
    assert(nc_i32_to_be(0x12345678) == 0x78563412 && "Bytes are somehow different");
#endif
}

static void test_i32_count_bits(void) {
    assert(nc_i32_count_ones(0x12345678) == 13 && "Did count correctly");
    assert(nc_i32_count_zeros(0x12345678) == 19 && "Did count correctly");
}

static void test_i32_from_bytes(void) {
    assert(nc_i32_from_be_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Conversion not correct");
    assert(nc_i32_from_le_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Conversion not correct");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i32_from_ne_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Not converted correctly");
#else
    assert(nc_i32_from_ne_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Not converted correctly");
#endif
}

static void test_i32_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_i32_to_be_bytes(0x12345678, &arena), NC_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
    assert(nc_bytes_eq(nc_i32_to_le_bytes(0x12345678, &arena), NC_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_i32_to_ne_bytes(0x12345678, &arena), NC_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
#else
    assert(nc_bytes_eq(nc_i32_to_ne_bytes(0x12345678, &arena), NC_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nc_arena_free(&arena);
}

static void test_i32_hash(void) {
    assert(nc_i32_hash(0) == 0x130f353e031ba7);
    assert(nc_i32_hash(69) == 0x53620494cc400a2);
    assert(nc_i32_hash(42) == 0x333883cd70fb570);
}
/* i32 */

/* u64 */
static void test_u64_leading_bits(void) {
    assert(nc_u64_leading_ones(0xe000000000000001ULL) == 3 && "Did not count correctly");
    assert(nc_u64_leading_zeros(0x1f00000000000000) == 3 && "Did not count correctly");
    assert(nc_u64_trailing_ones(0x8000000000000007) == 3 && "Did not count correctly");
    assert(nc_u64_trailing_zeros(0x8000000000000008) == 3 && "Did not count correctly");
}

static void test_u64_swaping_bits(void) {
    assert(nc_u64_reverse_bits(0x1234567812345678) == 0x1e6a2c481e6a2c48 &&
           "Did not reverse correctly");
    assert(nc_u64_swap_bytes(0x1234567812345678) == 0x7856341278563412 && "Did not swap correctly");
}

static void test_u64_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u64_to_le(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
    assert(nc_u64_to_be(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
#else
    assert(nc_u64_to_le(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
    assert(nc_u64_to_be(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
#endif
}

static void test_u64_count_bits(void) {
    assert(nc_u64_count_ones(0x1234567812345678) == 26 && "Did count correctly");
    assert(nc_u64_count_zeros(0x1234567812345678) == 38 && "Did count correctly");
}

static void test_u64_from_bytes(void) {
    assert(nc_u64_from_be_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
    assert(nc_u64_from_le_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_u64_from_ne_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
#else
    assert(nc_u64_from_ne_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#endif
}

static void test_u64_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_u64_to_be_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
    assert(nc_bytes_eq(nc_u64_to_le_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_u64_to_ne_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
#else
    assert(nc_bytes_eq(nc_u64_to_ne_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#endif
    nc_arena_free(&arena);
}

static void test_u64_hash(void) {
    assert(nc_u64_hash(0) == 0x130f353e031ba7);
    assert(nc_u64_hash(69) == 0x53620494cc400a2);
    assert(nc_u64_hash(42) == 0x333883cd70fb570);
}
/* u64 */

/* i64 */
static void test_i64_leading_bits(void) {
    assert(nc_i64_leading_ones((i64)0xe000000000000001LL) == 3 && "Did not count correctly");
    assert(nc_i64_leading_zeros((i64)0x1f00000000000000) == 3 && "Did not count correctly");
    assert(nc_i64_trailing_ones((i64)0x8000000000000007) == 3 && "Did not count correctly");
    assert(nc_i64_trailing_zeros((i64)0x8000000000000008) == 3 && "Did not count correctly");
}

static void test_i64_swaping_bits(void) {
    assert(nc_i64_reverse_bits(0x1234567812345678) == 0x1e6a2c481e6a2c48 &&
           "Did not reverse correctly");
    assert(nc_i64_swap_bytes(0x1234567812345678) == 0x7856341278563412 && "Did not swap correctly");
}

static void test_i64_endian(void) {
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i64_to_le(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
    assert(nc_i64_to_be(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
#else
    assert(nc_i64_to_le(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
    assert(nc_i64_to_be(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
#endif
}

static void test_i64_count_bits(void) {
    assert(nc_i64_count_ones(0x1234567812345678) == 26 && "Did count correctly");
    assert(nc_i64_count_zeros(0x1234567812345678) == 38 && "Did count correctly");
}

static void test_i64_from_bytes(void) {
    assert(nc_i64_from_be_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
    assert(nc_i64_from_le_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_i64_from_ne_bytes(NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
#else
    assert(nc_i64_from_ne_bytes(NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#endif
}

static void test_i64_to_bytes(void) {
    nc_Arena arena = {0};
    assert(nc_bytes_eq(nc_i64_to_be_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
    assert(nc_bytes_eq(nc_i64_to_le_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#if NC_BYTE_ORDER == NC_ENDIAN_BIG
    assert(nc_bytes_eq(nc_i64_to_ne_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
#else
    assert(nc_bytes_eq(nc_i64_to_ne_bytes(0x1234567812345678, &arena),
                       NC_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#endif
    nc_arena_free(&arena);
}

static void test_i64_hash(void) {
    assert(nc_i64_hash(0) == 0x130f353e031ba7);
    assert(nc_i64_hash(69) == 0x53620494cc400a2);
    assert(nc_i64_hash(42) == 0x333883cd70fb570);
}
/* i64 */

void run_test_int(void) {
    test_u8_leading_bits();
    test_u8_swaping_bits();
    test_u8_endian();
    test_u8_count_bits();
    test_u8_from_bytes();
    test_u8_to_bytes();
    test_u8_hash();

    test_i8_leading_bits();
    test_i8_swaping_bits();
    test_i8_endian();
    test_i8_count_bits();
    test_i8_from_bytes();
    test_i8_to_bytes();
    test_i8_hash();

    test_u16_leading_bits();
    test_u16_swaping_bits();
    test_u16_endian();
    test_u16_count_bits();
    test_u16_from_bytes();
    test_u16_to_bytes();
    test_u16_hash();

    test_i16_leading_bits();
    test_i16_swaping_bits();
    test_i16_endian();
    test_i16_count_bits();
    test_i16_from_bytes();
    test_i16_to_bytes();
    test_i16_hash();

    test_u32_leading_bits();
    test_u32_swaping_bits();
    test_u32_endian();
    test_u32_count_bits();
    test_u32_from_bytes();
    test_u32_to_bytes();
    test_u32_hash();

    test_i32_leading_bits();
    test_i32_swaping_bits();
    test_i32_endian();
    test_i32_count_bits();
    test_i32_from_bytes();
    test_i32_to_bytes();
    test_i32_hash();

    test_u64_leading_bits();
    test_u64_swaping_bits();
    test_u64_endian();
    test_u64_count_bits();
    test_u64_from_bytes();
    test_u64_to_bytes();
    test_u64_hash();

    test_i64_leading_bits();
    test_i64_swaping_bits();
    test_i64_endian();
    test_i64_count_bits();
    test_i64_from_bytes();
    test_i64_to_bytes();
    test_i64_hash();
}
