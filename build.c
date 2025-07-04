#define NC_IMPLEMENTATION
#include "nc.h"

#define CC NC_COMPILER_NAME
typedef nc_List(nc_Path) Files;

static void collect_flags(nc_Cmd *cmd) {
    nc_cmd_push(cmd, "-Wall", "-Werror", "-Wextra", "-Wpedantic");
    nc_cmd_push(cmd, "-Wmissing-prototypes", "-Wmissing-declarations");
    nc_cmd_push(cmd, "-Wconversion", "-Wsign-conversion");
    nc_cmd_push(cmd, "-Wshadow", "-Wpointer-arith", "-Wstrict-prototypes");
    nc_cmd_push(cmd, "-Wcast-qual", "-Wcast-align", "-Wwrite-strings");
    nc_cmd_push(cmd, "-Wundef", "-Wswitch-enum", "-Wdouble-promotion");
    nc_cmd_push(cmd, "-Wbad-function-cast", "-Wuninitialized");
    nc_cmd_push(cmd, "-Wredundant-decls", "-Wvla", "-Wnested-externs");
    nc_cmd_push(cmd, "-Wstrict-aliasing=2", "-Wno-unused-parameter");
    nc_cmd_push(cmd, "-std=c99");
    nc_cmd_push(cmd, "-Iinclude");
    nc_cmd_push(cmd, "-g");
    nc_cmd_push(cmd, "-O0");
    nc_cmd_push(cmd, "-fsanitize=address,undefined");
    nc_cmd_push(cmd, "-fPIE");
    nc_cmd_push(cmd, "-DFUCK");
}

static void collect_files(Files* files, nc_Path path, nc_Error* error) {
    nc_FsIter it = nc_fs_begin(path, true, error);
    for (nc_FsEntry* file = NULL; (file = nc_fs_next(&it));) {
        if (file->is_dir) continue;
        nc_list_push(files, nc_str_copy(file->path, files->arena));
    }
    nc_fs_end(&it);
}

static bool compile_commands(void) {
    nc_os_mkdir(NC_PATH("build"), NULL, (nc_OsDirConfig){.exists_ok=true});

    bool result = true;
    bool first = true;
    FILE* bc = nc_file_open(NC_PATH("build/compile_commands.json"), "w", NULL);
    nc_Arena arena = {0};
    nc_Cmd cmd = {0};
    nc_list_init(&cmd, &arena);

    nc_Path cwd = nc_os_cwd(&arena, NULL);

    nc_file_write_fmt(bc, "[");
    nc_FsIter it = nc_fs_begin(NC_PATH("src/"), true, NULL);
    for (nc_FsEntry* file = NULL; (file = nc_fs_next(&it));) {
        if (file->is_dir) continue;
        #if defined(_WIN32)
        if (nc_str_contains(file->path, NC_STR("posix"))) continue;
        #else
        if (nc_str_contains(file->path, NC_STR("windows"))) continue;
        #endif

        nc_cmd_push(&cmd, CC, "-c", "-o", "test.o", file->path.data);
        collect_flags(&cmd);

        if (nc_cmd_exec_list(&cmd) == 0) {
            if (!first) nc_file_write_fmt(bc, ", ");
            nc_file_write_fmt(bc, "{");
            nc_file_write_fmt(bc, "\"file\": \""NC_STR_FMT"\", ", NC_STR_ARG(file->path));
            nc_file_write_fmt(bc, "\"directory\": \""NC_STR_FMT"\", ", NC_STR_ARG(cwd));

            nc_file_write_fmt(bc, "\"arguments\": [");
            nc_list_for_each(const char**, c, &cmd) {
                if (c != cmd.items) nc_file_write_fmt(bc, ", ");
                nc_file_write_fmt(bc, "\"%s\"", *c);
            }
            nc_file_write_fmt(bc, "]");

            nc_file_write_fmt(bc, "}");
        } else {
            result = false;
        }

        nc_list_clear(&cmd);
    }
    nc_fs_end(&it);
    nc_file_write_fmt(bc, "]");

    nc_fs_remove(NC_PATH("test.o"));

    nc_file_close(bc);
    nc_arena_free(&arena);
    return result;
}

static i32 path_compare(const void *s1, const void *s2) {
    const nc_Path *p1 = s1;
    const nc_Path *p2 = s2;

    if (p1->len > p2->len) return strncmp(p2->data, p1->data, p2->len);
    else                   return strncmp(p1->data, p2->data, p1->len);
}

static void write_to_file(FILE* out, Files* files) {
    nc_Arena scratch = {0};

    nc_list_for_each(nc_Path*, path, files) {
        nc_file_write_fmt(out, "// "NC_STR_FMT"\n", NC_STR_ARG(*path));

        if (nc_str_contains(*path, NC_STR("windows"))) {
            nc_file_write_fmt(out, "#if defined(_WIN32)\n");
        }

        if (nc_str_contains(*path, NC_STR("posix"))) {
            nc_file_write_fmt(out, "#if !defined(_WIN32)\n");
        }

        FILE* file = nc_file_open(*path, "r", NULL);

        nc_Str content = nc_file_read_str(file, &scratch);
        for (nc_Str line; nc_str_try_chop_by_delim(&content, '\n', &line);) {
            if (nc_str_contains(line, NC_STR("_H_"))) continue;
            if (nc_str_contains(line, NC_STR("#include \"nc/"))) continue;
            nc_file_write_fmt(out, NC_STR_FMT"\n", NC_STR_ARG(line));
        } 

        nc_file_close(file);


        if (nc_str_contains(*path, NC_STR("windows")) || nc_str_contains(*path, NC_STR("posix"))) {
            nc_file_write_fmt(out, "#endif\n");
        }

        nc_arena_reset(&scratch);
    }

    nc_arena_free(&scratch);
}

static bool build_header_file(void) {
    nc_Arena arena = {0};

    FILE* nc = nc_file_open(NC_PATH("nc.h"), "w", NULL);

    nc_file_write_fmt(nc, "/* nycticebus standard library */\n\n");

    Files headers = {0};
    nc_list_init(&headers, &arena);
    collect_files(&headers, NC_PATH("include/nc/"), NULL);
    nc_list_sort(&headers, path_compare);

    nc_file_write_fmt(nc, "#ifndef _NC_H_\n");
    write_to_file(nc, &headers);
    nc_file_write_fmt(nc, "#endif // _NC_H_\n\n");

    Files src = {0};
    nc_list_init(&src, &arena);
    collect_files(&src, NC_PATH("src/nc/"), NULL);
    nc_list_sort(&src, path_compare);

    nc_file_write_fmt(nc, "#ifdef NC_IMPLEMENTATION\n");
    write_to_file(nc, &src);
    nc_file_write_fmt(nc, "#endif // NC_IMPLEMENTATION\n");

    nc_file_close(nc);


    nc_Cmd cmd = {0};
    nc_list_init(&cmd, &arena);

    nc_cmd_push(&cmd, CC, "-c", "-o", "build/nc.o", "-DNC_IMPLEMENTATION", "nc.h");
    collect_flags(&cmd);
    if (nc_cmd_exec_list(&cmd) != 0) return false;

    nc_arena_free(&arena);
    return true;
}

int main(int argc, const char** argv) {
    if (!compile_commands()) return 1;

    if (argc < 2 || strcmp("build", argv[1]) == 0 || strcmp("test", argv[1])) {
        if (!build_header_file()) return 2;
    }

    if (argc >= 2 && strcmp("test", argv[1]) == 0) {
        NC_NOT_IMPLEMENTED("tests");
    }

    return 0;
}
