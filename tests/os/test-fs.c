#include "nc/os/fs.h"

#include "nc/core/arena.h"
#include "nc/core/error.h"

#include "nc/types/str.h"

#include "nc/structs/list.h"

static void test_fs_iteration(void) {
    bool is_empty = true;

    nc_Error error = {0};
    nc_FsIter it = nc_fs_begin(NC_PATH("tests/"), true, &error);
    for (nc_FsEntry *e; (e = nc_fs_next(&it));) {
        is_empty = false;
        break;
    }
    nc_fs_end(&it);
    NC_ERROR_HANDLE(&error, NC_ASSERT(false && "Should not throw an error"));
    NC_ASSERT(is_empty == false && "the directory should not be empty");
}

#if 0

static void test_fs_read_str(void) {
    nc_Arena arena = {0};

    nc_Error error = {0};
    FILE* file = nc_fs_open(NC_PATH(__FILE__), "r", &error);
    NC_ERROR_PROPAGATE(&error, { goto defer; });
    usize file_size = nc_fs_size(file);

    nc_StrBuilder builder = {0};
    nc_list_init(&builder, &arena);

    nc_Str content = nc_fs_read_str(file, &builder);
    NC_ASSERT(content.len == file_size);
    NC_ASSERT(builder.len == file_size);

defer:
    if (file) nc_fs_close(file);
    nc_arena_free(&arena);
}

#elif 0

static void test_fs_read_line(void) {
    nc_Arena arena = {0};

    nc_Error error = {0};
    FILE* file = nc_fs_open(NC_PATH(__FILE__), "r", &error);
    NC_ERROR_PROPAGATE(&error, { goto defer; });

    nc_StrBuilder builder = {0};
    nc_list_init(&builder, &arena);

    nc_Str line = nc_fs_read_line(file, &builder);
    NC_ASSERT(nc_str_eq(NC_STR("#include \"nc/os/fs.h\"\n"), line));

defer:
    if (file) nc_fs_close(file);
    nc_arena_free(&arena);
}

#elif 0

static void test_fs_read_bytes(void) {
    nc_Arena arena = {0};

    nc_Error error = {0};
    FILE* file = nc_fs_open(NC_PATH(__FILE__), "r", &error);
    NC_ERROR_PROPAGATE(&error, { goto defer; });

    u8 buffer[512] = {0};
    nc_Bytes chunk = nc_fs_read_bytes(file, NC_ARRAY_LEN(buffer), buffer);
    for (; chunk.size; chunk = nc_fs_read_bytes(file, NC_ARRAY_LEN(buffer), buffer)) {
        nc_fs_check_error(file, &error);
        NC_ERROR_PROPAGATE(&error, { goto defer; });

        // process chunk
    }

defer:
    if (file) nc_fs_close(file);
    nc_arena_free(&arena);
}
#endif

void run_test_fs(void);
void run_test_fs(void) {
    test_fs_iteration();
    // test_fs_read_str();
    // test_fs_read_line();
    // test_fs_read_bytes();
}
