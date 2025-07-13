#include "../../nsl.h"

static void test_file_open(void) {
    FILE* file = NULL;
    nsl_Error error = nsl_file_open(&file, NSL_PATH(__FILE__), "r");
    NSL_ASSERT(file && error == 0);
    nsl_file_close(file);
}

static void test_file_read_str(void) {
    FILE* file = NULL;
    nsl_Error err = nsl_file_open(&file, NSL_PATH(__FILE__), "r");
    NSL_ASSERT(file && err == 0);

    nsl_StrBuilder sb = {0};

    nsl_Str content = nsl_file_read_sb(file, &sb);
    nsl_Str line = nsl_str_chop_by_delim(&content, '\n');
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("#include \"../../nsl.h\"")));

    NSL_ASSERT(err == 0);

    nsl_file_close(file);
    nsl_list_free(&sb);
}

static void test_file_read_sb(void) {
    FILE* file = NULL;
    nsl_Error error = nsl_file_open(&file, NSL_PATH(__FILE__), "r");
    NSL_ASSERT(file && error == 0);

    nsl_StrBuilder sb = {0};

    nsl_Str content = nsl_file_read_sb(file, &sb);
    nsl_Str line = nsl_str_chop_by_delim(&content, '\n');
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("#include \"../../nsl.h\"")));

    nsl_file_close(file);
    nsl_list_free(&sb);
}

static void test_file_read_line(void) {
    FILE* file = NULL;
    nsl_Error error = nsl_file_open(&file, NSL_PATH(__FILE__), "r");
    NSL_ASSERT(file && error == 0);

    nsl_StrBuilder sb = {0};

    nsl_Str line = nsl_file_read_line(file, &sb);
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("#include \"../../nsl.h\"\n")));

    line = nsl_file_read_line(file, &sb);
    NSL_ASSERT(nsl_str_eq(line, NSL_STR("\n")));

    nsl_file_close(file);
    nsl_list_free(&sb);
}

static void test_file_read_bytes(void) {
    FILE* file = NULL;
    nsl_Error error =  nsl_file_open(&file, NSL_PATH(__FILE__), "r");
    NSL_ASSERT(file && error == 0);

    u8 buffer[10] = {0};
    nsl_Bytes chunk = nsl_file_read_bytes(file, sizeof(buffer), buffer);
    NSL_ASSERT(nsl_bytes_eq(chunk, NSL_BYTES_STR("#include \"")));

    nsl_file_close(file);
}

static void test_file_write_str(void) {
    nsl_Str content = NSL_STR("Hello, World\n");

    FILE* file = NULL;
    nsl_Error error = nsl_file_open(&file, NSL_PATH("build/test.txt"), "w");
    NSL_ASSERT(error == 0);

    nsl_file_write_str(file, content);

    nsl_file_close(file);
}

static void test_file_write_bytes(void) {
    nsl_Bytes content = NSL_BYTES_STR("Hello, World\n");

    FILE* file = NULL;
    nsl_Error error = nsl_file_open(&file, NSL_PATH("build/test.bin"), "w");
    NSL_ASSERT(error == 0);

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
