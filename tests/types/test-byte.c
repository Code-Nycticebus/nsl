#include "nsl/types/byte.h"

#include "nsl/types/str.h"

static void test_bytes(void) {
    nsl_Arena arena = {0};
    nsl_Bytes b = NSL_BYTES(0x02, 0xFF, 0xAA, 0xBB, 0x41, 0x41, 0x41);
    NSL_ASSERT(b.data[0] == 0x02 && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b.data[1] == 0xff && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b.data[2] == 0xaa && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b.data[3] == 0xbb && "nsl_Bytes are not initialized correctly");
    nsl_arena_free(&arena);
}

static void test_nc_bytes_str(void) {
    nsl_Bytes b = NSL_BYTES_STR("ABC");
    NSL_ASSERT(b.size == 3 && "Size is not correct");
    NSL_ASSERT(b.data[0] == 0x41 && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b.data[1] == 0x42 && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b.data[2] == 0x43 && "nsl_Bytes are not initialized correctly");

    nsl_Bytes b2 = NSL_BYTES_STR("\x41\x42\x43");
    NSL_ASSERT(b2.size == 3 && "Size is not correct");
    NSL_ASSERT(b2.data[0] == 0x41 && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b2.data[1] == 0x42 && "nsl_Bytes are not initialized correctly");
    NSL_ASSERT(b2.data[2] == 0x43 && "nsl_Bytes are not initialized correctly");
}

static void test_to_hex(void) {
    nsl_Arena arena = {0};
    nsl_Bytes b = NSL_BYTES(0x41, 0x42, 0x43);
    nsl_Str s = nsl_bytes_to_hex(b, &arena);
    nsl_Str ec = NSL_STR("414243");
    NSL_ASSERT(ec.data);
    NSL_ASSERT(nsl_str_eq(s, ec) && "nsl_String conversion was not correct");

    nsl_Bytes b2 = NSL_BYTES(0x02, 0xFF, 0xAA, 0xBB, 0x01, 0x02, 0x03);
    nsl_Str s2 = nsl_bytes_to_hex(b2, &arena);
    NSL_ASSERT(nsl_str_eq(s2, NSL_STR("2ffaabb010203")) && "nsl_String conversion was not correct");

    nsl_Bytes b3 = NSL_BYTES(0xaa, 0xbb, 0xcc, 0xdd, 0x41, 0x41, 0x41, 0x41, 0x42, 0x42, 0x42, 0x42);
    nsl_Str s3 = nsl_bytes_to_hex(b3, &arena);
    NSL_ASSERT(nsl_str_eq(s3, NSL_STR("aabbccdd4141414142424242")) && "nsl_String conversion was not correct");

    nsl_arena_free(&arena);
}

static void test_nc_bytes_cmp(void) {
    nsl_Bytes b1 = NSL_BYTES(0xFF);
    NSL_ASSERT(nsl_bytes_eq(b1, NSL_BYTES(0xFF)) == true && "nsl_Bytes not compared correctly");
    NSL_ASSERT(nsl_bytes_eq(b1, NSL_BYTES(0x0F)) == false && "nsl_Bytes not compared correctly");
}

static void test_nc_bytes_slice(void) {
    nsl_Bytes b = NSL_BYTES_STR("ABC");
    nsl_Bytes slice = nsl_bytes_slice(b, 1, 3);
    NSL_ASSERT(nsl_bytes_eq(slice, NSL_BYTES_STR("BC")) && "nsl_Bytes were not sliced correctly");

    nsl_Bytes invalid = nsl_bytes_slice(b, b.size, 2);
    NSL_ASSERT(nsl_bytes_eq(invalid, NSL_BYTES_STR("")) && "nsl_Bytes were not invalidated");

    nsl_Bytes invalid2 = nsl_bytes_slice(b, 0, b.size + 1);
    NSL_ASSERT(nsl_bytes_eq(invalid2, NSL_BYTES_STR("")) && "nsl_Bytes were not invalidated");
}

static void test_nc_bytes_take(void) {
    nsl_Bytes bytes = NSL_BYTES(0x12, 0x34, 0x56, 0x78);
    nsl_Bytes take = nsl_bytes_take(&bytes, 3);
    NSL_ASSERT(nsl_bytes_eq(bytes, NSL_BYTES(0x78)) && "Did not take the correct bytes");
    NSL_ASSERT(nsl_bytes_eq(take, NSL_BYTES(0x12, 0x34, 0x56)) && "Did not take the correct bytes");
    nsl_Bytes rest = nsl_bytes_take(&bytes, 3);
    NSL_ASSERT(nsl_bytes_eq(rest, NSL_BYTES(0x78)) && "Did not take the correct bytes");

    nsl_Bytes invalid = nsl_bytes_take(&bytes, 3);
    NSL_ASSERT(invalid.size == 0 && "Did not take the correct amount of bytes");
}

static void test_nc_bytes_from_hex(void) {
    nsl_Arena arena = {0};
    nsl_Bytes b1 = nsl_bytes_from_hex(NSL_STR("0xA0"), &arena);
    NSL_ASSERT(b1.size == 1 && "conversion was not done correctly");
    NSL_ASSERT(b1.data[0] == 0xa0 && "conversion was not correct");

    nsl_Bytes b2 = nsl_bytes_from_hex(NSL_STR("0xA0B1C2"), &arena);
    NSL_ASSERT(b2.size == 3 && "conversion was not done correctly");
    NSL_ASSERT(b2.data[0] == 0xa0 && "conversion was not correct");
    NSL_ASSERT(b2.data[1] == 0xb1 && "conversion was not correct");
    NSL_ASSERT(b2.data[2] == 0xc2 && "conversion was not correct");

    nsl_Bytes b3 = nsl_bytes_from_hex(NSL_STR("0x101"), &arena);
    NSL_ASSERT(b3.size == 2 && "conversion was not done correctly");
    NSL_ASSERT(b3.data[0] == 0x1 && "conversion was not correct");
    NSL_ASSERT(b3.data[1] == 0x01 && "conversion was not correct");

    nsl_arena_free(&arena);
}

static void test_nc_bytes_hash(void) {
    nsl_Bytes b1 = NSL_BYTES_STR("abc");
    nsl_Bytes b2 = NSL_BYTES_STR("abc");
    nsl_Bytes b3 = NSL_BYTES_STR("bcd");

    NSL_ASSERT(nsl_bytes_hash(b1) == nsl_bytes_hash(b2) && "should be equal");
    NSL_ASSERT(nsl_bytes_hash(b1) != nsl_bytes_hash(b3) && "should not be equal");
}

void run_test_bytes(void);
void run_test_bytes(void) {
    test_bytes();
    test_nc_bytes_str();
    test_to_hex();
    test_nc_bytes_cmp();
    test_nc_bytes_slice();
    test_nc_bytes_take();
    test_nc_bytes_from_hex();
    test_nc_bytes_hash();
}
