#define NSL_IMPLEMENTATION
#include "nsl.h"

#ifdef WINDOWS
#    define _WIN32
#    define CC "x86_64-w64-mingw32-gcc"
#endif

#ifndef CC
#   define CC NSL_COMPILER_NAME
#endif

static void cmd_push_flags(nsl_Cmd *cmd) {
    nsl_cmd_push(cmd, "-Wall", "-Werror", "-Wextra", "-Wpedantic");
    nsl_cmd_push(cmd, "-std=c99");
}

static nsl_Error build_header(nsl_Cmd *cmd) {
    cmd->len = 0;

    nsl_cmd_push(cmd, CC, "-c", "-o", "build/nsl.o", "-DNSL_IMPLEMENTATION", "nsl.h");
    cmd_push_flags(cmd);

    if (nsl_cmd_exec(cmd)) return NSL_ERROR;

    return NSL_NO_ERROR;
}

static nsl_Error build_tests(nsl_Cmd *cmd) {
    cmd->len = 0;

    nsl_cmd_push(cmd, CC, "-o", "build/test", "-DNSL_IMPLEMENTATION", "-I.", "tests/main.c");
    cmd_push_flags(cmd);
    if (nsl_cmd_exec(cmd)) return NSL_ERROR;

    if (NSL_CMD("test")) return NSL_ERROR;

    return NSL_NO_ERROR;
}

static nsl_Error build_examples(nsl_Cmd *cmd) {
    cmd->len = 0;
    NSL_UNUSED(cmd);
    return NSL_NO_ERROR;
}

int main(int argc, const char **argv) {
    int result = 0;
    nsl_Cmd cmd = {0};

    if (nsl_os_mkdir(NSL_PATH("build"), NSL_DEFAULT)) NSL_DEFER(1);

    if (build_header(&cmd))    NSL_DEFER(2);
    if (build_tests(&cmd))     NSL_DEFER(3);
    if (build_examples(&cmd))  NSL_DEFER(4);

defer:
    nsl_list_free(&cmd);
    return 0;
}

