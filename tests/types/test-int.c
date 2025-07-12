#include "nsl.h"

/* u8 */
static void test_u8_leading_bits(void) {
    NSL_ASSERT(nsl_u8_leading_ones(0xe0) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u8_leading_zeros(0x1f) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u8_trailing_ones(0x07) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u8_trailing_zeros(0x08) == 3 && "Did not count correctly");
}

static void test_u8_swaping_bits(void) {
    NSL_ASSERT(nsl_u8_reverse_bits(0x12) == 0x48 && "Did not reverse correctly");
    NSL_ASSERT(nsl_u8_swap_bytes(0x12) == 0x12 && "Did not swap correctly");
}

static void test_u8_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u8_to_le(0x12) == 0x12 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u8_to_be(0x12) == 0x12 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_u8_to_le(0x12) == 0x12 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u8_to_be(0x12) == 0x12 && "Bytes are somehow different");
#endif
}

static void test_u8_count_bits(void) {
    NSL_ASSERT(nsl_u8_count_ones(0x12) == 2 && "Did count correctly");
    NSL_ASSERT(nsl_u8_count_zeros(0x12) == 6 && "Did count correctly");
}

static void test_u8_from_bytes(void) {
    NSL_ASSERT(nsl_u8_from_be_bytes(NSL_BYTES(0x12)) == 0x12 && "Conversion not correct");
    NSL_ASSERT(nsl_u8_from_le_bytes(NSL_BYTES(0x12)) == 0x12 && "Conversion not correct");
    NSL_ASSERT(nsl_u8_from_ne_bytes(NSL_BYTES(0x12)) == 0x12 && "Conversion not correct");
}

static void test_u8_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_u8_to_be_bytes(0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
    NSL_ASSERT(nsl_bytes_eq(nsl_u8_to_le_bytes(0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_u8_to_ne_bytes(0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_u8_to_ne_bytes(0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
#endif
    nsl_arena_free(&arena);
}

static void test_u8_hash(void) {
    NSL_ASSERT(nsl_u8_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_u8_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_u8_hash(42) == 0x333883cd70fb570);
}

static void test_u8_next_pow2(void) {
    NSL_ASSERT(nsl_u8_next_pow2(0) == 1);
    NSL_ASSERT(nsl_u8_next_pow2(1) == 1);
    NSL_ASSERT(nsl_u8_next_pow2(5) == 8);
    NSL_ASSERT(nsl_u8_next_pow2(255) == 255); // clamped
}

/* u8 */

/* i8 */
static void test_i8_leading_bits(void) {
    NSL_ASSERT(nsl_i8_leading_ones((i8)0xe0) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i8_leading_zeros((i8)0x1f) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i8_trailing_ones((i8)0x07) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i8_trailing_zeros((i8)0x08) == 3 && "Did not count correctly");
}

static void test_i8_swaping_bits(void) {
    NSL_ASSERT(nsl_i8_reverse_bits((i8)0x12) == 0x48 && "Did not reverse correctly");
    NSL_ASSERT(nsl_i8_swap_bytes((i8)0x12) == 0x12 && "Did not swap correctly");
}

static void test_i8_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i8_to_le((i8)0x1a) == 0x1a && "Bytes are somehow different");
    NSL_ASSERT(nsl_i8_to_be((i8)0x1a) == 0x1a && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_i8_to_le((i8)0x1a) == 0x1a && "Bytes are somehow different");
    NSL_ASSERT(nsl_i8_to_be((i8)0x1a) == 0x1a && "Bytes are somehow different");
#endif
}

static void test_i8_count_bits(void) {
    NSL_ASSERT(nsl_i8_count_ones((i8)0x12) == 2 && "Did count correctly");
    NSL_ASSERT(nsl_i8_count_zeros((i8)0x12) == 6 && "Did count correctly");
}

static void test_i8_from_bytes(void) {
    NSL_ASSERT(nsl_i8_from_be_bytes(NSL_BYTES(0x12)) == 0x12 && "Conversion not correct");
    NSL_ASSERT(nsl_i8_from_le_bytes(NSL_BYTES(0x12)) == 0x12 && "Conversion not correct");
    NSL_ASSERT(nsl_i8_from_ne_bytes(NSL_BYTES(0x12)) == 0x12 && "Conversion not correct");
}

static void test_i8_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_i8_to_be_bytes((i8)0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
    NSL_ASSERT(nsl_bytes_eq(nsl_i8_to_le_bytes((i8)0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_i8_to_ne_bytes(0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_i8_to_ne_bytes((i8)0x12, &arena), NSL_BYTES(0x12)) &&
           "Not converted correctly");
#endif
    nsl_arena_free(&arena);
}

static void test_i8_hash(void) {
    NSL_ASSERT(nsl_i8_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_i8_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_i8_hash(-69) == 0xe8a748836a8337d1);
    NSL_ASSERT(nsl_i8_hash(42) == 0x333883cd70fb570);
}

static void test_i8_next_pow2(void) {
    NSL_ASSERT(nsl_i8_next_pow2(0) == 1);
    NSL_ASSERT(nsl_i8_next_pow2(1) == 1);
    NSL_ASSERT(nsl_i8_next_pow2(5) == 8);
    NSL_ASSERT(nsl_i8_next_pow2(127) == 127); // clamped
}

/* i8 */

/* u16 */
static void test_u16_leading_bits(void) {
    NSL_ASSERT(nsl_u16_leading_ones(0xe001) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u16_leading_zeros(0x1f00) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u16_trailing_ones(0x1007) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u16_trailing_zeros(0x1008) == 3 && "Did not count correctly");
}

static void test_u16_swaping_bits(void) {
    NSL_ASSERT(nsl_u16_reverse_bits(0x1234) == 0x2c48 && "Did not reverse correctly");
    NSL_ASSERT(nsl_u16_swap_bytes(0x1234) == 0x3412 && "Did not swap correctly");
}

static void test_u16_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u16_to_le(0x1234) == 0x3412 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u16_to_be(0x1234) == 0x1234 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_u16_to_le(0x1234) == 0x1234 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u16_to_be(0x1234) == 0x3412 && "Bytes are somehow different");
#endif
}

static void test_u16_count_bits(void) {
    NSL_ASSERT(nsl_u16_count_ones(0x1234) == 5 && "Did count correctly");
    NSL_ASSERT(nsl_u16_count_zeros(0x1234) == 11 && "Did count correctly");
}

static void test_u16_from_bytes(void) {
    NSL_ASSERT(nsl_u16_from_be_bytes(NSL_BYTES(0x12, 0x34)) == 0x1234 && "Conversion not correct");
    NSL_ASSERT(nsl_u16_from_le_bytes(NSL_BYTES(0x34, 0x12)) == 0x1234 && "Conversion not correct");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u16_from_ne_bytes(NSL_BYTES(0x12, 0x34)) == 0x1234 && "Not converted correctly");
#else
    NSL_ASSERT(nsl_u16_from_ne_bytes(NSL_BYTES(0x34, 0x12)) == 0x1234 && "Not converted correctly");
#endif
}

static void test_u16_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_u16_to_be_bytes(0x1234, &arena), NSL_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
    NSL_ASSERT(nsl_bytes_eq(nsl_u16_to_le_bytes(0x1234, &arena), NSL_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_u16_to_ne_bytes(0x1234, &arena), NSL_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_u16_to_ne_bytes(0x1234, &arena), NSL_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nsl_arena_free(&arena);
}

static void test_u16_hash(void) {
    NSL_ASSERT(nsl_u16_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_u16_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_u16_hash(42) == 0x333883cd70fb570);
}

static void test_u16_next_pow2(void) {
    NSL_ASSERT(nsl_u16_next_pow2(0) == 1);
    NSL_ASSERT(nsl_u16_next_pow2(300) == 512);
    NSL_ASSERT(nsl_u16_next_pow2(65535) == 65535); // clamped
}

/* u16 */

/* i16 */
static void test_i16_leading_bits(void) {
    NSL_ASSERT(nsl_i16_leading_ones((i16)0xe001) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i16_leading_zeros((i16)0x1f00) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i16_trailing_ones((i16)0x1007) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i16_trailing_zeros((i16)0x1008) == 3 && "Did not count correctly");
}

static void test_i16_swaping_bits(void) {
    NSL_ASSERT(nsl_i16_reverse_bits(0x1234) == 0x2c48 && "Did not reverse correctly");
    NSL_ASSERT(nsl_i16_swap_bytes(0x1234) == 0x3412 && "Did not swap correctly");
}

static void test_i16_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i16_to_le(0x1234) == 0x3412 && "Bytes are somehow different");
    NSL_ASSERT(nsl_i16_to_be(0x1234) == 0x1234 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_i16_to_le(0x1234) == 0x1234 && "Bytes are somehow different");
    NSL_ASSERT(nsl_i16_to_be(0x1234) == 0x3412 && "Bytes are somehow different");
#endif
}

static void test_i16_count_bits(void) {
    NSL_ASSERT(nsl_i16_count_ones(0x1234) == 5 && "Did count correctly");
    NSL_ASSERT(nsl_i16_count_zeros(0x1234) == 11 && "Did count correctly");
}

static void test_i16_from_bytes(void) {
    NSL_ASSERT(nsl_i16_from_be_bytes(NSL_BYTES(0x12, 0x34)) == 0x1234 && "Conversion not correct");
    NSL_ASSERT(nsl_i16_from_le_bytes(NSL_BYTES(0x34, 0x12)) == 0x1234 && "Conversion not correct");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i16_from_ne_bytes(NSL_BYTES(0x12, 0x34)) == 0x1234 && "Not converted correctly");
#else
    NSL_ASSERT(nsl_i16_from_ne_bytes(NSL_BYTES(0x34, 0x12)) == 0x1234 && "Not converted correctly");
#endif
}

static void test_i16_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_i16_to_be_bytes(0x1234, &arena), NSL_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
    NSL_ASSERT(nsl_bytes_eq(nsl_i16_to_le_bytes(0x1234, &arena), NSL_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_i16_to_ne_bytes(0x1234, &arena), NSL_BYTES(0x12, 0x34)) &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_i16_to_ne_bytes(0x1234, &arena), NSL_BYTES(0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nsl_arena_free(&arena);
}

static void test_i16_hash(void) {
    NSL_ASSERT(nsl_i16_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_i16_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_i16_hash(42) == 0x333883cd70fb570);
}

static void test_i16_next_pow2(void) {
    NSL_ASSERT(nsl_i16_next_pow2(0) == 1);
    NSL_ASSERT(nsl_i16_next_pow2(300) == 512);
    NSL_ASSERT(nsl_i16_next_pow2(32767) == 32767); // clamped
}

/* i16 */

/* u32 */
static void test_u32_leading_bits(void) {
    NSL_ASSERT(nsl_u32_leading_ones(0xe0000001) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u32_leading_zeros(0x1f000000) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u32_trailing_ones(0x80000007) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u32_trailing_zeros(0x80000008) == 3 && "Did not count correctly");
}

static void test_u32_swaping_bits(void) {
    NSL_ASSERT(nsl_u32_reverse_bits(0x12345678) == 0x1e6a2c48 && "Did not reverse correctly");
    NSL_ASSERT(nsl_u32_swap_bytes(0x12345678) == 0x78563412 && "Did not swap correctly");
}

static void test_u32_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u32_to_le(0x12345678) == 0x78563412 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u32_to_be(0x12345678) == 0x12345678 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_u32_to_le(0x12345678) == 0x12345678 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u32_to_be(0x12345678) == 0x78563412 && "Bytes are somehow different");
#endif
}

static void test_u32_count_bits(void) {
    NSL_ASSERT(nsl_u32_count_ones(0x12345678) == 13 && "Did count correctly");
    NSL_ASSERT(nsl_u32_count_zeros(0x12345678) == 19 && "Did count correctly");
}

static void test_u32_from_bytes(void) {
    NSL_ASSERT(nsl_u32_from_be_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Conversion not correct");
    NSL_ASSERT(nsl_u32_from_le_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Conversion not correct");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u32_from_ne_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_u32_from_ne_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Not converted correctly");
#endif
}

static void test_u32_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_u32_to_be_bytes(0x12345678, &arena), NSL_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
    NSL_ASSERT(nsl_bytes_eq(nsl_u32_to_le_bytes(0x12345678, &arena), NSL_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_u32_to_ne_bytes(0x12345678, &arena), NSL_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_u32_to_ne_bytes(0x12345678, &arena), NSL_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nsl_arena_free(&arena);
}

static void test_u32_hash(void) {
    NSL_ASSERT(nsl_u32_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_u32_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_u32_hash(42) == 0x333883cd70fb570);
}

static void test_u32_next_pow2(void) {
    NSL_ASSERT(nsl_u32_next_pow2(0) == 1);
    NSL_ASSERT(nsl_u32_next_pow2(50000) == 65536);
    NSL_ASSERT(nsl_u32_next_pow2(4294967295U) == 4294967295U); // clamped
}

/* u32 */

/* i32 */
static void test_i32_leading_bits(void) {
    NSL_ASSERT(nsl_i32_leading_ones((i32)0xe0000001) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i32_leading_zeros((i32)0x1f000000) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i32_trailing_ones((i32)0x80000007) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i32_trailing_zeros((i32)0x80000008) == 3 && "Did not count correctly");
}

static void test_i32_swaping_bits(void) {
    NSL_ASSERT(nsl_i32_reverse_bits(0x12345678) == 0x1e6a2c48 && "Did not reverse correctly");
    NSL_ASSERT(nsl_i32_swap_bytes(0x12345678) == 0x78563412 && "Did not swap correctly");
}

static void test_i32_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i32_to_le(0x12345678) == 0x78563412 && "Bytes are somehow different");
    NSL_ASSERT(nsl_i32_to_be(0x12345678) == 0x12345678 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_i32_to_le(0x12345678) == 0x12345678 && "Bytes are somehow different");
    NSL_ASSERT(nsl_i32_to_be(0x12345678) == 0x78563412 && "Bytes are somehow different");
#endif
}

static void test_i32_count_bits(void) {
    NSL_ASSERT(nsl_i32_count_ones(0x12345678) == 13 && "Did count correctly");
    NSL_ASSERT(nsl_i32_count_zeros(0x12345678) == 19 && "Did count correctly");
}

static void test_i32_from_bytes(void) {
    NSL_ASSERT(nsl_i32_from_be_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Conversion not correct");
    NSL_ASSERT(nsl_i32_from_le_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Conversion not correct");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i32_from_ne_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78)) == 0x12345678 &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_i32_from_ne_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12)) == 0x12345678 &&
           "Not converted correctly");
#endif
}

static void test_i32_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_i32_to_be_bytes(0x12345678, &arena), NSL_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
    NSL_ASSERT(nsl_bytes_eq(nsl_i32_to_le_bytes(0x12345678, &arena), NSL_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_i32_to_ne_bytes(0x12345678, &arena), NSL_BYTES(0x12, 0x34, 0x56, 0x78)) &&
           "Not converted correctly");
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_i32_to_ne_bytes(0x12345678, &arena), NSL_BYTES(0x78, 0x56, 0x34, 0x12)) &&
           "Not converted correctly");
#endif
    nsl_arena_free(&arena);
}

static void test_i32_hash(void) {
    NSL_ASSERT(nsl_i32_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_i32_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_i32_hash(42) == 0x333883cd70fb570);
}

static void test_i32_next_pow2(void) {
    NSL_ASSERT(nsl_i32_next_pow2(0) == 1);
    NSL_ASSERT(nsl_i32_next_pow2(50000) == 65536);
    NSL_ASSERT(nsl_i32_next_pow2(2147483647) == 2147483647); // clamped
}

/* i32 */

/* u64 */
static void test_u64_leading_bits(void) {
    NSL_ASSERT(nsl_u64_leading_ones(0xe000000000000001ULL) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u64_leading_zeros(0x1f00000000000000) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u64_trailing_ones(0x8000000000000007) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_u64_trailing_zeros(0x8000000000000008) == 3 && "Did not count correctly");
}

static void test_u64_swaping_bits(void) {
    NSL_ASSERT(nsl_u64_reverse_bits(0x1234567812345678) == 0x1e6a2c481e6a2c48 &&
           "Did not reverse correctly");
    NSL_ASSERT(nsl_u64_swap_bytes(0x1234567812345678) == 0x7856341278563412 && "Did not swap correctly");
}

static void test_u64_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u64_to_le(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u64_to_be(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_u64_to_le(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
    NSL_ASSERT(nsl_u64_to_be(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
#endif
}

static void test_u64_count_bits(void) {
    NSL_ASSERT(nsl_u64_count_ones(0x1234567812345678) == 26 && "Did count correctly");
    NSL_ASSERT(nsl_u64_count_zeros(0x1234567812345678) == 38 && "Did count correctly");
}

static void test_u64_from_bytes(void) {
    NSL_ASSERT(nsl_u64_from_be_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
    NSL_ASSERT(nsl_u64_from_le_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_u64_from_ne_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
#else
    NSL_ASSERT(nsl_u64_from_ne_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#endif
}

static void test_u64_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_u64_to_be_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
    NSL_ASSERT(nsl_bytes_eq(nsl_u64_to_le_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_u64_to_ne_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_u64_to_ne_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#endif
    nsl_arena_free(&arena);
}

static void test_u64_hash(void) {
    NSL_ASSERT(nsl_u64_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_u64_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_u64_hash(42) == 0x333883cd70fb570);
}

static void test_u64_next_pow2(void) {
    NSL_ASSERT(nsl_u64_next_pow2(0) == 1);
    NSL_ASSERT(nsl_u64_next_pow2(10000000000ULL) == 17179869184ULL);
    NSL_ASSERT(nsl_u64_next_pow2(18446744073709551615ULL) == 18446744073709551615ULL); // clamped
}

/* u64 */

/* i64 */
static void test_i64_leading_bits(void) {
    NSL_ASSERT(nsl_i64_leading_ones((i64)0xe000000000000001LL) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i64_leading_zeros((i64)0x1f00000000000000) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i64_trailing_ones((i64)0x8000000000000007) == 3 && "Did not count correctly");
    NSL_ASSERT(nsl_i64_trailing_zeros((i64)0x8000000000000008) == 3 && "Did not count correctly");
}

static void test_i64_swaping_bits(void) {
    NSL_ASSERT(nsl_i64_reverse_bits(0x1234567812345678) == 0x1e6a2c481e6a2c48 &&
           "Did not reverse correctly");
    NSL_ASSERT(nsl_i64_swap_bytes(0x1234567812345678) == 0x7856341278563412 && "Did not swap correctly");
}

static void test_i64_endian(void) {
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i64_to_le(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
    NSL_ASSERT(nsl_i64_to_be(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
#else
    NSL_ASSERT(nsl_i64_to_le(0x1234567812345678) == 0x1234567812345678 && "Bytes are somehow different");
    NSL_ASSERT(nsl_i64_to_be(0x1234567812345678) == 0x7856341278563412 && "Bytes are somehow different");
#endif
}

static void test_i64_count_bits(void) {
    NSL_ASSERT(nsl_i64_count_ones(0x1234567812345678) == 26 && "Did count correctly");
    NSL_ASSERT(nsl_i64_count_zeros(0x1234567812345678) == 38 && "Did count correctly");
}

static void test_i64_from_bytes(void) {
    NSL_ASSERT(nsl_i64_from_be_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
    NSL_ASSERT(nsl_i64_from_le_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_i64_from_ne_bytes(NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)) ==
           0x1234567812345678);
#else
    NSL_ASSERT(nsl_i64_from_ne_bytes(NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)) ==
           0x1234567812345678);
#endif
}

static void test_i64_to_bytes(void) {
    nsl_Arena arena = {0};
    NSL_ASSERT(nsl_bytes_eq(nsl_i64_to_be_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
    NSL_ASSERT(nsl_bytes_eq(nsl_i64_to_le_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#if NSL_BYTE_ORDER == NSL_ENDIAN_BIG
    NSL_ASSERT(nsl_bytes_eq(nsl_i64_to_ne_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78)));
#else
    NSL_ASSERT(nsl_bytes_eq(nsl_i64_to_ne_bytes(0x1234567812345678, &arena),
                       NSL_BYTES(0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12)));
#endif
    nsl_arena_free(&arena);
}

static void test_i64_hash(void) {
    NSL_ASSERT(nsl_i64_hash(0) == 0x130f353e031ba7);
    NSL_ASSERT(nsl_i64_hash(69) == 0x53620494cc400a2);
    NSL_ASSERT(nsl_i64_hash(42) == 0x333883cd70fb570);
}

static void test_i64_next_pow2(void) {
    NSL_ASSERT(nsl_i64_next_pow2(0) == 1);
    NSL_ASSERT(nsl_i64_next_pow2(10000000000LL) == 17179869184LL);
    NSL_ASSERT(nsl_i64_next_pow2(9223372036854775807LL) == 9223372036854775807LL); // clamped
}

/* i64 */

void run_test_int(void);
void run_test_int(void) {
    test_u8_leading_bits();
    test_u8_swaping_bits();
    test_u8_endian();
    test_u8_count_bits();
    test_u8_from_bytes();
    test_u8_to_bytes();
    test_u8_hash();
    test_u8_next_pow2();

    test_i8_leading_bits();
    test_i8_swaping_bits();
    test_i8_endian();
    test_i8_count_bits();
    test_i8_from_bytes();
    test_i8_to_bytes();
    test_i8_hash();
    test_i8_next_pow2();

    test_u16_leading_bits();
    test_u16_swaping_bits();
    test_u16_endian();
    test_u16_count_bits();
    test_u16_from_bytes();
    test_u16_to_bytes();
    test_u16_hash();
    test_u16_next_pow2();

    test_i16_leading_bits();
    test_i16_swaping_bits();
    test_i16_endian();
    test_i16_count_bits();
    test_i16_from_bytes();
    test_i16_to_bytes();
    test_i16_hash();
    test_i16_next_pow2();

    test_u32_leading_bits();
    test_u32_swaping_bits();
    test_u32_endian();
    test_u32_count_bits();
    test_u32_from_bytes();
    test_u32_to_bytes();
    test_u32_hash();
    test_u32_next_pow2();

    test_i32_leading_bits();
    test_i32_swaping_bits();
    test_i32_endian();
    test_i32_count_bits();
    test_i32_from_bytes();
    test_i32_to_bytes();
    test_i32_hash();
    test_i32_next_pow2();

    test_u64_leading_bits();
    test_u64_swaping_bits();
    test_u64_endian();
    test_u64_count_bits();
    test_u64_from_bytes();
    test_u64_to_bytes();
    test_u64_hash();
    test_u64_next_pow2();

    test_i64_leading_bits();
    test_i64_swaping_bits();
    test_i64_endian();
    test_i64_count_bits();
    test_i64_from_bytes();
    test_i64_to_bytes();
    test_i64_hash();
    test_i64_next_pow2();
}
