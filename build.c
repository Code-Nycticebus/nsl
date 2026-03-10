#define NSL_IMPLEMENTATION
#include "nsl.h"

static void build_push_flags(nsl_Cmd *cmd) {
    nsl_cmd_push(cmd, "-Wall", "-Werror", "-Wextra", "-Wpedantic");
    nsl_cmd_push(cmd, "-std=c99");
}

static nsl_Error build_header_mingw32(nsl_Cmd *cmd) {
    cmd->len = 0;
    nsl_cmd_push(cmd, "x86_64-w64-mingw32-gcc", "-c", "-o", "build/nsl.o", "-DNSL_IMPLEMENTATION", "nsl.h");
    build_push_flags(cmd);

    if (nsl_cmd_exec(cmd)) return NSL_ERROR;

    return NSL_NO_ERROR;
}

static nsl_Error build_header_gcc(nsl_Cmd *cmd) {
    cmd->len = 0;
    nsl_cmd_push(cmd, "gcc", "-c", "-o", "build/nsl.o", "-DNSL_IMPLEMENTATION", "nsl.h");
    build_push_flags(cmd);

    if (nsl_cmd_exec(cmd)) return NSL_ERROR;

    return NSL_NO_ERROR;
}

static nsl_Error build_tests_mingw32(nsl_Cmd *cmd) {
    cmd->len = 0;
    nsl_cmd_push(cmd, "x86_64-w64-mingw32-gcc", "-o", "build/test.exe", "-DNSL_IMPLEMENTATION", "-I.", "tests/main.c");
    build_push_flags(cmd);
    if (nsl_cmd_exec(cmd)) return NSL_ERROR;

    if (NSL_CMD("wine", "build/test.exe")) return NSL_ERROR;
    return NSL_NO_ERROR;
}

static nsl_Error build_tests_gcc(nsl_Cmd *cmd) {
    nsl_Error result = NSL_NO_ERROR;
    nsl_Arena arena = {0};
    nsl_dir_walk(e, NSL_PATH("tests"), true) {
        if (e->is_dir) continue;

        nsl_arena_reset(&arena);
        cmd->len = 0;

        nsl_Path stem = nsl_path_stem(e->path);
        const char *bin_dir = nsl_str_format(&arena, "build/"NSL_STR_FMT, NSL_STR_ARG(stem)).data;
        nsl_cmd_push(cmd, "gcc", "-o", bin_dir, "-DNSL_IMPLEMENTATION", "-I.", e->path.data);
        build_push_flags(cmd);
        if (nsl_cmd_exec(cmd)) NSL_DEFER(NSL_ERROR);

        printf("running: "NSL_STR_FMT"\n", NSL_STR_ARG(stem));
        switch (NSL_CMD(bin_dir)) {
        case NSL_ERROR_SIGSEGV:
            printf("test segfaults!\n");
            break;
        }
    }

defer:
    nsl_arena_free(&arena);
    return result;
}

int main(int argc, const char **argv) {
    int result = 0;
    nsl_Cmd cmd = {0};

    if (nsl_os_mkdir(NSL_PATH("build"), .exists_ok=true)) NSL_DEFER(1);

    if (build_header_gcc(&cmd)) NSL_DEFER(2);
    if (build_tests_gcc(&cmd))  NSL_DEFER(3);

defer:
    nsl_list_free(&cmd);
    return result;
}

