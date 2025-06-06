#include "nc/os/file.h"

#include "nc/core/error.h"
#include "nc/core/arena.h"
#include "nc/structs/list.h"
#include "nc/types/str.h"
#include "nc/types/byte.h"

static void test_file_open(void) {
    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH(__FILE__), "r", &error);
    NC_ASSERT(file && error.code == 0);
    nc_file_close(file);
}

static void test_file_read_str(void) {
    nc_Arena arena = {0};

    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH(__FILE__), "r", &error);
    NC_ASSERT(file && error.code == 0);

    nc_Str content = nc_file_read_str(file, &arena);
    nc_Str line = nc_str_chop_by_delim(&content, '\n');
    NC_ASSERT(nc_str_eq(line, NC_STR("#include \"nc/os/file.h\"")));

    nc_file_check_error(file, &error);
    NC_ASSERT(error.code == 0);

    nc_file_close(file);
    nc_arena_free(&arena);
}

static void test_file_read_sb(void) {
    nc_Arena arena = {0};

    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH(__FILE__), "r", &error);
    NC_ASSERT(file && error.code == 0);

    nc_StrBuilder sb = {0};
    nc_list_init(&sb, &arena);

    nc_Str content = nc_file_read_sb(file, &sb);
    nc_Str line = nc_str_chop_by_delim(&content, '\n');
    NC_ASSERT(nc_str_eq(line, NC_STR("#include \"nc/os/file.h\"")));

    nc_file_check_error(file, &error);
    NC_ASSERT(error.code == 0);

    nc_file_close(file);
    nc_arena_free(&arena);
}

static void test_file_read_line(void) {
    nc_Arena arena = {0};

    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH(__FILE__), "r", &error);
    NC_ASSERT(file && error.code == 0);

    nc_StrBuilder sb = {0};
    nc_list_init(&sb, &arena);

    nc_Str line = nc_file_read_line(file, &sb);
    NC_ASSERT(nc_str_eq(line, NC_STR("#include \"nc/os/file.h\"\n")));

    line = nc_file_read_line(file, &sb);
    NC_ASSERT(nc_str_eq(line, NC_STR("\n")));

    nc_file_check_error(file, &error);
    NC_ASSERT(error.code == 0);

    nc_file_close(file);
    nc_arena_free(&arena);
}

static void test_file_read_bytes(void) {
    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH(__FILE__), "r", &error);
    NC_ASSERT(file && error.code == 0);

    u8 buffer[10] = {0};
    nc_Bytes chunk = nc_file_read_bytes(file, sizeof(buffer), buffer);
    NC_ASSERT(nc_bytes_eq(chunk, NC_BYTES_STR("#include \"")));

    nc_file_check_error(file, &error);
    NC_ASSERT(error.code == 0);

    nc_file_close(file);
}

static void test_file_write_str(void) {
    nc_Str content = NC_STR("Hello, World\n");

    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH("build/test.txt"), "w", &error);
    NC_ASSERT(error.code == 0);

    nc_file_write_str(file, content);

    nc_file_close(file);
}

static void test_file_write_bytes(void) {
    nc_Bytes content = NC_BYTES_STR("Hello, World\n");

    nc_Error error = {0};
    FILE* file = nc_file_open(NC_PATH("build/test.bin"), "w", &error);
    NC_ASSERT(error.code == 0);

    nc_file_write_bytes(file, content);

    nc_file_close(file);
}

void run_test_file(void);
void run_test_file(void) {
    test_file_open();
    test_file_read_str();
    test_file_read_sb();
    test_file_read_line();
    test_file_read_bytes();
    test_file_write_str();
}
