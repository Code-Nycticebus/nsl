#include "nsl.h"

#include "nsl.h"
#include "nsl.h"
#include "nsl.h"
#include "nsl.h"
#include "nsl.h"

static void test_file_open(void) {
    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH(__FILE__), "r", &error);
    NSL_ASSERT(file && error.code == 0);
    nsl_file_close(file);
}

static void test_file_read_str(void) {
    nsl_Arena arena = {0};

    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH(__FILE__), "r", &error);
    NSL_ASSERT(file && error.code == 0);

    nsl_StrBuilder sb = {0};
    nsl_list_init(&sb, &arena);

    nsl_Str content = nsl_file_read_sb(file, &sb);
    nsl_Str line = nsl_str_chop_by_delim(&content, '\n');
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("#include \"nc/os/file.h\"")));

    nsl_file_check_error(file, &error);
    NSL_ASSERT(error.code == 0);

    nsl_file_close(file);
    nsl_arena_free(&arena);
}

static void test_file_read_sb(void) {
    nsl_Arena arena = {0};

    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH(__FILE__), "r", &error);
    NSL_ASSERT(file && error.code == 0);

    nsl_StrBuilder sb = {0};
    nsl_list_init(&sb, &arena);

    nsl_Str content = nsl_file_read_sb(file, &sb);
    nsl_Str line = nsl_str_chop_by_delim(&content, '\n');
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("#include \"nc/os/file.h\"")));

    nsl_file_check_error(file, &error);
    NSL_ASSERT(error.code == 0);

    nsl_file_close(file);
    nsl_arena_free(&arena);
}

static void test_file_read_line(void) {
    nsl_Arena arena = {0};

    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH(__FILE__), "r", &error);
    NSL_ASSERT(file && error.code == 0);

    nsl_StrBuilder sb = {0};
    nsl_list_init(&sb, &arena);

    nsl_Str line = nsl_file_read_line(file, &sb);
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("#include \"nc/os/file.h\"\n")));

    line = nsl_file_read_line(file, &sb);
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("\n")));

    nsl_file_check_error(file, &error);
    NSL_ASSERT(error.code == 0);

    nsl_file_close(file);
    nsl_arena_free(&arena);
}

static void test_file_read_bytes(void) {
    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH(__FILE__), "r", &error);
    NSL_ASSERT(file && error.code == 0);

    u8 buffer[10] = {0};
    nsl_Bytes chunk = nsl_file_read_bytes(file, sizeof(buffer), buffer);
    NSL_ASSERT(nsl_bytes_eq(chunk, NSL_BYTES_STR("#include \"")));

    nsl_file_check_error(file, &error);
    NSL_ASSERT(error.code == 0);

    nsl_file_close(file);
}

static void test_file_write_str(void) {
    nsl_Str content = NSL_STR("Hello, World\n");

    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH("build/test.txt"), "w", &error);
    NSL_ASSERT(error.code == 0);

    nsl_file_write_str(file, content);

    nsl_file_close(file);
}

static void test_file_write_bytes(void) {
    nsl_Bytes content = NSL_BYTES_STR("Hello, World\n");

    nsl_Error error = {0};
    FILE* file = nsl_file_open(NSL_PATH("build/test.bin"), "w", &error);
    NSL_ASSERT(error.code == 0);

    nsl_file_write_bytes(file, content);

    nsl_file_close(file);
}

void run_test_file(void);
void run_test_file(void) {
    test_file_open();
    test_file_read_str();
    test_file_read_sb();
    test_file_read_line();
    test_file_read_bytes();
    test_file_write_str();
    test_file_write_bytes();
}
