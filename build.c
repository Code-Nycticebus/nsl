#define NSL_IMPLEMENTATION
#include "nsl.h"

#define CC NSL_COMPILER_NAME
typedef nsl_List(nsl_Path) Files;

static void collect_flags(nsl_Cmd *cmd) {
    nsl_cmd_push(cmd, "-Wall", "-Werror", "-Wextra", "-Wpedantic");
    nsl_cmd_push(cmd, "-Wmissing-prototypes", "-Wmissing-declarations");
    nsl_cmd_push(cmd, "-Wconversion", "-Wsign-conversion");
    nsl_cmd_push(cmd, "-Wshadow", "-Wpointer-arith", "-Wstrict-prototypes");
    nsl_cmd_push(cmd, "-Wcast-qual", "-Wcast-align", "-Wwrite-strings");
    nsl_cmd_push(cmd, "-Wundef", "-Wswitch-enum", "-Wdouble-promotion");
    nsl_cmd_push(cmd, "-Wbad-function-cast", "-Wuninitialized");
    nsl_cmd_push(cmd, "-Wredundant-decls", "-Wvla", "-Wnested-externs");
    nsl_cmd_push(cmd, "-Wstrict-aliasing=2", "-Wno-unused-parameter");
    nsl_cmd_push(cmd, "-std=c99");
    nsl_cmd_push(cmd, "-Iinclude");
    nsl_cmd_push(cmd, "-g");
    nsl_cmd_push(cmd, "-O0");
    nsl_cmd_push(cmd, "-fsanitize=address,undefined");
    nsl_cmd_push(cmd, "-fPIE");
    nsl_cmd_push(cmd, "-DFUCK");
}

static void collect_files(Files* files, nsl_Path path, nsl_Error* error) {
    nsl_FsIter it = nsl_fs_begin(path, true, error);
    for (nsl_FsEntry* file = NULL; (file = nsl_fs_next(&it));) {
        if (file->is_dir) continue;
        nsl_list_push(files, nsl_str_copy(file->path, files->arena));
    }
    nsl_fs_end(&it);
}

static bool compile_commands(void) {
    nsl_os_mkdir(NSL_PATH("build"), NULL, (nsl_OsDirConfig){.exists_ok=true});

    bool result = true;
    bool first = true;
    FILE* bc = nsl_file_open(NSL_PATH("build/compile_commands.json"), "w", NULL);
    nsl_Arena arena = {0};
    nsl_Cmd cmd = {0};
    nsl_list_init(&cmd, &arena);

    nsl_Path cwd = nsl_os_cwd(&arena, NULL);

    nsl_file_write_fmt(bc, "[");
    nsl_FsIter it = nsl_fs_begin(NSL_PATH("src/nsl"), true, NULL);
    for (nsl_FsEntry* file = NULL; (file = nsl_fs_next(&it));) {
        if (file->is_dir) continue;
        #if defined(_WIN32)
        if (nsl_str_contains(file->path, NSL_STR("posix"))) continue;
        #else
        if (nsl_str_contains(file->path, NSL_STR("windows"))) continue;
        #endif

        nsl_cmd_push(&cmd, CC, "-c", "-o", "test.o", file->path.data);
        collect_flags(&cmd);

        if (nsl_cmd_exec_list(&cmd) == 0) {
            if (!first) nsl_file_write_fmt(bc, ", ");
            nsl_file_write_fmt(bc, "{");
            nsl_file_write_fmt(bc, "\"file\": \""NSL_STR_FMT"\", ", NSL_STR_ARG(file->path));
            nsl_file_write_fmt(bc, "\"directory\": \""NSL_STR_FMT"\", ", NSL_STR_ARG(cwd));

            nsl_file_write_fmt(bc, "\"arguments\": [");
            nsl_list_for_each(const char**, c, &cmd) {
                if (c != cmd.items) nsl_file_write_fmt(bc, ", ");
                nsl_file_write_fmt(bc, "\"%s\"", *c);
            }
            nsl_file_write_fmt(bc, "]");

            nsl_file_write_fmt(bc, "}");
        } else {
            result = false;
        }

        nsl_list_clear(&cmd);
    }
    nsl_fs_end(&it);
    nsl_file_write_fmt(bc, "]");

    nsl_fs_remove(NSL_PATH("test.o"));

    nsl_file_close(bc);
    nsl_arena_free(&arena);
    return result;
}

static i32 path_compare(const void *s1, const void *s2) {
    const nsl_Path *p1 = s1;
    const nsl_Path *p2 = s2;

    if (p1->len > p2->len) return strncmp(p2->data, p1->data, p2->len);
    else                   return strncmp(p1->data, p2->data, p1->len);
}

static void write_to_file(FILE* out, Files* files) {
    nsl_Arena scratch = {0};

    nsl_list_for_each(nsl_Path*, path, files) {
        nsl_file_write_fmt(out, "// "NSL_STR_FMT"\n", NSL_STR_ARG(*path));

        if (nsl_str_contains(*path, NSL_STR("windows"))) {
            nsl_file_write_fmt(out, "#if defined(_WIN32)\n");
        }

        if (nsl_str_contains(*path, NSL_STR("posix"))) {
            nsl_file_write_fmt(out, "#if !defined(_WIN32)\n");
        }

        FILE* file = nsl_file_open(*path, "r", NULL);

        nsl_Str content = nsl_file_read_str(file, &scratch);
        for (nsl_Str line; nsl_str_try_chop_by_delim(&content, '\n', &line);) {
            if (nsl_str_contains(line, NSL_STR("_H_"))) continue;
            if (nsl_str_contains(line, NSL_STR("#include \"nsl/"))) continue;
            nsl_file_write_fmt(out, NSL_STR_FMT"\n", NSL_STR_ARG(line));
        } 

        nsl_file_close(file);


        if (nsl_str_contains(*path, NSL_STR("windows")) || nsl_str_contains(*path, NSL_STR("posix"))) {
            nsl_file_write_fmt(out, "#endif\n");
        }

        nsl_arena_reset(&scratch);
    }

    nsl_arena_free(&scratch);
}

static bool build_header_file(void) {
    nsl_Arena arena = {0};

    FILE* nsl = nsl_file_open(NSL_PATH("nsl.h"), "w", NULL);

    FILE* r = nsl_file_open(NSL_PATH("README.md"), "r", NULL);
    nsl_Str readme = nsl_file_read_str(r, &arena);
    nsl_file_write_fmt(nsl, "/*\n"NSL_STR_FMT"*/\n\n", NSL_STR_ARG(readme));
    nsl_file_close(r);

    Files headers = {0};
    nsl_list_init(&headers, &arena);
    collect_files(&headers, NSL_PATH("include/nsl/"), NULL);
    nsl_list_sort(&headers, path_compare);

    nsl_file_write_fmt(nsl, "#ifndef _NSL_H_\n");
    nsl_file_write_fmt(nsl, "#define _NSL_H_\n\n");
    write_to_file(nsl, &headers);
    nsl_file_write_fmt(nsl, "#endif // _NSL_H_\n\n");

    Files src = {0};
    nsl_list_init(&src, &arena);
    collect_files(&src, NSL_PATH("src/nsl/"), NULL);
    nsl_list_sort(&src, path_compare);

    nsl_file_write_fmt(nsl, "#ifdef NSL_IMPLEMENTATION\n");
    write_to_file(nsl, &src);
    nsl_file_write_fmt(nsl, "#endif // NSL_IMPLEMENTATION\n");

    nsl_file_close(nsl);


    nsl_Cmd cmd = {0};
    nsl_list_init(&cmd, &arena);

    nsl_cmd_push(&cmd, CC, "-c", "-o", "build/nsl.o", "-DNSL_IMPLEMENTATION", "nsl.h");
    collect_flags(&cmd);
    if (nsl_cmd_exec_list(&cmd) != 0) return false;

    nsl_arena_free(&arena);
    return true;
}

static bool build_tests(void) {
    nsl_Arena arena = {0};
    nsl_Cmd cmd = {0};
    nsl_list_init(&cmd, &arena);

    nsl_cmd_push(&cmd, CC, "-o", "build/test", "-I.", "tests/main.c");

    nsl_cmd_exec_list(&cmd);

    nsl_arena_free(&arena);
    return true;
}

int main(int argc, const char** argv) {
    if (!compile_commands()) return 1;

    if (argc < 2 || strcmp("build", argv[1]) == 0 || strcmp("test", argv[1]) == 0) {
        if (!build_header_file()) return 2;
    }

    if (argc >= 2 && strcmp("test", argv[1]) == 0) {
        if (!build_tests()) return 3;
    }

    return 0;
}
