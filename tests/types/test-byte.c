#include "nc/types/byte.h"

#include "nc/types/str.h"

#include <assert.h>

static void test_bytes(void) {
    nc_Arena arena = {0};
    nc_Bytes b = NC_BYTES(0x02, 0xFF, 0xAA, 0xBB, 0x41, 0x41, 0x41);
    assert(b.data[0] == 0x02 && "nc_Bytes are not initialized correctly");
    assert(b.data[1] == 0xff && "nc_Bytes are not initialized correctly");
    assert(b.data[2] == 0xaa && "nc_Bytes are not initialized correctly");
    assert(b.data[3] == 0xbb && "nc_Bytes are not initialized correctly");
    nc_arena_free(&arena);
}

static void test_nc_bytes_str(void) {
    nc_Bytes b = NC_BYTES_STR("ABC");
    assert(b.size == 3 && "Size is not correct");
    assert(b.data[0] == 0x41 && "nc_Bytes are not initialized correctly");
    assert(b.data[1] == 0x42 && "nc_Bytes are not initialized correctly");
    assert(b.data[2] == 0x43 && "nc_Bytes are not initialized correctly");

    nc_Bytes b2 = NC_BYTES_STR("\x41\x42\x43");
    assert(b2.size == 3 && "Size is not correct");
    assert(b2.data[0] == 0x41 && "nc_Bytes are not initialized correctly");
    assert(b2.data[1] == 0x42 && "nc_Bytes are not initialized correctly");
    assert(b2.data[2] == 0x43 && "nc_Bytes are not initialized correctly");
}

static void test_to_hex(void) {
    nc_Arena arena = {0};
    nc_Bytes b = NC_BYTES(0x41, 0x42, 0x43);
    nc_Str s = nc_bytes_to_hex(b, &arena);
    assert(nc_str_eq(s, NC_STR("414243")) && "nc_String conversion was not correct");

    nc_Bytes b2 = NC_BYTES(0x02, 0xFF, 0xAA, 0xBB, 0x01, 0x02, 0x03);
    nc_Str s2 = nc_bytes_to_hex(b2, &arena);
    assert(nc_str_eq(s2, NC_STR("2ffaabb010203")) && "nc_String conversion was not correct");

    nc_Bytes b3 = NC_BYTES(0xaa, 0xbb, 0xcc, 0xdd, 0x41, 0x41, 0x41, 0x41, 0x42, 0x42, 0x42, 0x42);
    nc_Str s3 = nc_bytes_to_hex(b3, &arena);
    assert(nc_str_eq(s3, NC_STR("aabbccdd4141414142424242")) && "nc_String conversion was not correct");

    nc_arena_free(&arena);
}

static void test_nc_bytes_cmp(void) {
    nc_Bytes b1 = NC_BYTES(0xFF);
    assert(nc_bytes_eq(b1, NC_BYTES(0xFF)) == true && "nc_Bytes not compared correctly");
    assert(nc_bytes_eq(b1, NC_BYTES(0x0F)) == false && "nc_Bytes not compared correctly");
}

static void test_nc_bytes_slice(void) {
    nc_Bytes b = NC_BYTES_STR("ABC");
    nc_Bytes slice = nc_bytes_slice(b, 1, 3);
    assert(nc_bytes_eq(slice, NC_BYTES_STR("BC")) && "nc_Bytes were not sliced correctly");

    nc_Bytes invalid = nc_bytes_slice(b, b.size, 2);
    assert(nc_bytes_eq(invalid, NC_BYTES_STR("")) && "nc_Bytes were not invalidated");

    nc_Bytes invalid2 = nc_bytes_slice(b, 0, b.size + 1);
    assert(nc_bytes_eq(invalid2, NC_BYTES_STR("")) && "nc_Bytes were not invalidated");
}

static void test_nc_bytes_take(void) {
    nc_Bytes bytes = NC_BYTES(0x12, 0x34, 0x56, 0x78);
    nc_Bytes take = nc_bytes_take(&bytes, 3);
    assert(nc_bytes_eq(bytes, NC_BYTES(0x78)) && "Did not take the correct bytes");
    assert(nc_bytes_eq(take, NC_BYTES(0x12, 0x34, 0x56)) && "Did not take the correct bytes");
    nc_Bytes rest = nc_bytes_take(&bytes, 3);
    assert(nc_bytes_eq(rest, NC_BYTES(0x78)) && "Did not take the correct bytes");

    nc_Bytes invalid = nc_bytes_take(&bytes, 3);
    assert(invalid.size == 0 && "Did not take the correct amount of bytes");
}

static void test_nc_bytes_from_hex(void) {
    nc_Arena arena = {0};
    nc_Bytes b1 = nc_bytes_from_hex(NC_STR("0xA0"), &arena);
    assert(b1.size == 1 && "conversion was not done correctly");
    assert(b1.data[0] == 0xa0 && "conversion was not correct");

    nc_Bytes b2 = nc_bytes_from_hex(NC_STR("0xA0B1C2"), &arena);
    assert(b2.size == 3 && "conversion was not done correctly");
    assert(b2.data[0] == 0xa0 && "conversion was not correct");
    assert(b2.data[1] == 0xb1 && "conversion was not correct");
    assert(b2.data[2] == 0xc2 && "conversion was not correct");

    nc_Bytes b3 = nc_bytes_from_hex(NC_STR("0x101"), &arena);
    assert(b3.size == 2 && "conversion was not done correctly");
    assert(b3.data[0] == 0x1 && "conversion was not correct");
    assert(b3.data[1] == 0x01 && "conversion was not correct");

    nc_arena_free(&arena);
}

static void test_nc_bytes_hash(void) {
    nc_Bytes b1 = NC_BYTES_STR("abc");
    nc_Bytes b2 = NC_BYTES_STR("abc");
    nc_Bytes b3 = NC_BYTES_STR("bcd");

    assert(nc_bytes_hash(b1) == nc_bytes_hash(b2) && "should be equal");
    assert(nc_bytes_hash(b1) != nc_bytes_hash(b3) && "should not be equal");
}

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
