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
    cmd->len = 0;
    nsl_cmd_push(cmd, "gcc", "-o", "build/test", "-DNSL_IMPLEMENTATION", "-I.", "tests/main.c");
    build_push_flags(cmd);
    if (nsl_cmd_exec(cmd)) return NSL_ERROR;

    if (NSL_CMD("build/test")) return NSL_ERROR;

    return NSL_NO_ERROR;
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

