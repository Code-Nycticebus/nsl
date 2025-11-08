#define NSL_IMPLEMENTATION
#include "nsl.h"

#define NSL_VERSION "v0.9.2 - dev"

#ifdef WINDOWS
#    define _WIN32
#    define CC "x86_64-w64-mingw32-gcc"
#endif

#ifndef CC
#   define CC NSL_COMPILER_NAME
#endif

typedef nsl_List(nsl_Path) Files;

static void collect_flags(nsl_Cmd *cmd) {
    nsl_cmd_push(cmd, "-Wall", "-Werror", "-Wextra", "-Wpedantic");
    nsl_cmd_push(cmd, "-std=c99");
    nsl_cmd_push(cmd, "-Isrc");
}

static i32 path_compare(const void *s1, const void *s2) {
    const nsl_Path *p1 = s1;
    const nsl_Path *p2 = s2;
    if (nsl_str_contains(*p1, NSL_STR("defines.h"))) return -1;
    if (nsl_str_contains(*p2, NSL_STR("defines.h"))) return  1;
    return strncmp(p1->data, p2->data, nsl_usize_min(p1->len, p2->len));
}

static void copy_to_file(FILE *out, nsl_Path path) {
    nsl_Arena scratch = {0};

    if (nsl_str_contains(path, NSL_STR("windows"))) {
        nsl_file_write_fmt(out, "#if defined(_WIN32)\n");
    } else if (nsl_str_contains(path, NSL_STR("posix"))) {
        nsl_file_write_fmt(out, "#if !defined(_WIN32)\n");
    }

    FILE* file = NULL;
    if (nsl_file_open(&file, path, "r")) NSL_PANIC("this file should open!!!");

    nsl_Str content = nsl_file_read_str(file, &scratch);
    for (nsl_Str line; nsl_str_try_chop_by_delim(&content, '\n', &line);) {
        if (nsl_str_contains(line, NSL_STR("_H_"))) continue;
        if (nsl_str_contains(line, NSL_STR("#include \"nsl/"))) continue;
        nsl_file_write_fmt(out, NSL_STR_FMT "\n", NSL_STR_ARG(line));
    }

    nsl_file_close(file);

    if (nsl_str_contains(path, NSL_STR("windows"))) {
        nsl_file_write_fmt(out, "#endif // _WIN32\n\n");
    } else if (nsl_str_contains(path, NSL_STR("posix"))) {
        nsl_file_write_fmt(out, "#endif // !_WIN32\n\n");
    }

    nsl_arena_free(&scratch);
}

static bool build_header_file(void) {
    bool result = false;
    nsl_Arena arena = {0};

    FILE *nsl = NULL;
    if (nsl_file_open(&nsl, NSL_PATH("nsl.h"), "w")) NSL_DEFER(true);

    nsl_file_write_fmt(nsl, "/*\n");
    nsl_file_write_fmt(nsl, "nsl.h - " NSL_VERSION " - MIT license - https://github.com/Code-Nycticebus/nsl\n\n");

    FILE *r = NULL;
    if (nsl_file_open(&r, NSL_PATH("README.md"), "r")) NSL_DEFER(true);
    nsl_Str readme = nsl_file_read_str(r, &arena);
    nsl_file_write_fmt(nsl, NSL_STR_FMT "\n", NSL_STR_ARG(readme));
    nsl_file_close(r);

    nsl_file_write_fmt(nsl, "*/\n\n");

    Files files = {.arena = &arena};
    nsl_dir_walk(file, NSL_PATH("src/nsl/"), true) {
        if (file->is_dir) continue;
        if (!nsl_str_endswith(file->path, NSL_STR(".h"))) continue;
        nsl_list_push(&files, nsl_str_copy(file->path, &arena));
    }

    nsl_list_sort(&files, path_compare);

    nsl_file_write_fmt(nsl, "#ifndef _NSL_H_\n");
    nsl_file_write_fmt(nsl, "#define _NSL_H_\n\n");
    nsl_list_for_each(nsl_Path *, path, &files) {
        copy_to_file(nsl, *path);
    }
    nsl_file_write_fmt(nsl, "#endif // _NSL_H_\n\n");
    files.len = 0;

    nsl_dir_walk(file, NSL_PATH("src/nsl/"), true) {
        if (file->is_dir) continue;
        if (!nsl_str_endswith(file->path, NSL_STR(".c"))) continue;
        nsl_list_push(&files, nsl_str_copy(file->path, &arena));
    }
    nsl_list_sort(&files, path_compare);

    nsl_file_write_fmt(nsl, "#if defined(NSL_IMPLEMENTATION) && !defined(_NSL_IMPLEMENTED)\n");
    nsl_file_write_fmt(nsl, "#define _NSL_IMPLEMENTED\n");
    nsl_list_for_each(nsl_Path *, path, &files) {
        copy_to_file(nsl, *path);
    }
    nsl_file_write_fmt(nsl, "\n#endif // NSL_IMPLEMENTATION\n");
    files.len = 0;

    nsl_file_close(nsl);
    nsl = NULL;

    nsl_Cmd cmd = {.arena = &arena};

    nsl_cmd_push(&cmd, CC, "-c", "-o", "build/nsl.o", "-DNSL_IMPLEMENTATION", "nsl.h");
    collect_flags(&cmd);
    if (nsl_cmd_exec_list(&cmd) != 0) NSL_DEFER(true);

defer:
    if (nsl) nsl_file_close(nsl);
    nsl_arena_free(&arena);
    return result;
}

static bool build_tests(void) {
    if (NSL_CMD(CC, "-o", "build/test", "-DNSL_IMPLEMENTATION", "-I.", "tests/main.c") != 0) return true;
    if (NSL_CMD("./build/test") != 0) return true;
    return false;
}

int main(int argc, const char **argv) {
    if (argc < 2 || strcmp("build", argv[1]) == 0 || strcmp("test", argv[1]) == 0) {
        if (build_header_file()) return 2;
    }

    if (argc >= 2 && strcmp("test", argv[1]) == 0) {
        if (build_tests()) return 3;
    }

    return 0;
}

