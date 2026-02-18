#define NSL_IMPLEMENTATION
#include "nsl.h"

#ifdef WINDOWS
#    define _WIN32
#    define CC "x86_64-w64-mingw32-gcc"
#endif

#ifndef CC
#   define CC NSL_COMPILER_NAME
#endif

static nsl_Error build_header(void) {
    nsl_Error result = NSL_NO_ERROR;
    nsl_Cmd cmd = {0};

    nsl_cmd_push(&cmd, CC, "-c", "-o", "nsl.o", "-DNSL_IMPLEMENTATION", "nsl.h");
    nsl_cmd_push(&cmd, "-Wall", "-Werror", "-Wextra", "-Wpedantic");
    nsl_cmd_push(&cmd, "-std=c99");
    nsl_cmd_push(&cmd, "-Isrc");

    if (nsl_cmd_exec(&cmd) != 0) NSL_DEFER(NSL_ERROR);

defer:
    nsl_os_remove(NSL_PATH("nsl.o"));
    return result;
}

static bool build_tests(void) {
    nsl_Error result = NSL_NO_ERROR;

    if (NSL_CMD(CC, "-o", "test", "-DNSL_IMPLEMENTATION", "-I.", "tests/main.c") != 0) NSL_DEFER(NSL_ERROR);
    if (NSL_CMD("test") != 0) NSL_DEFER(NSL_ERROR);

defer:
    nsl_os_remove(NSL_PATH("test"));
    return result;
}

int main(int argc, const char **argv) {
    if (build_header()) return 1;
    if (build_tests())  return 2;

    return 0;
}

