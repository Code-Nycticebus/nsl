#define CEBUS_IMPLEMENTATION
#include "bootstrap.h"

#define CC "gcc"

void collect_flags(Cmd *cmd) {
    cmd_push(cmd, STR("-Wall"), STR("-Werror"), STR("-Wextra"), STR("-Wpedantic"));
    cmd_push(cmd, STR("-Wmissing-prototypes"), STR("-Wmissing-declarations"));
    cmd_push(cmd, STR("-Wconversion"), STR("-Wsign-conversion"));
    cmd_push(cmd, STR("-Wshadow"), STR("-Wpointer-arith"), STR("-Wstrict-prototypes"));
    cmd_push(cmd, STR("-Wcast-qual"), STR("-Wcast-align"), STR("-Wwrite-strings"));
    cmd_push(cmd, STR("-Wundef"), STR("-Wswitch-enum"), STR("-Wdouble-promotion"));
    cmd_push(cmd, STR("-Wbad-function-cast"), STR("-Wuninitialized"));
    cmd_push(cmd, STR("-Wredundant-decls"), STR("-Wvla"), STR("-Wnested-externs"));
    cmd_push(cmd, STR("-Wstrict-aliasing=2"), STR("-Wno-unused-parameter"));
    cmd_push(cmd, STR("-std=c99"));
    cmd_push(cmd, STR("-Iinclude"));
    cmd_push(cmd, STR("-g"));
    cmd_push(cmd, STR("-O0"));
    cmd_push(cmd, STR("-fsanitize=address,undefined"));
    cmd_push(cmd, STR("-fPIE"));
}

void collect_files(Cmd *cmd) {
    FsIter it = fs_iter_begin(STR("src"), true);
    Cmd cmd_obj = cmd_new(&it.scratch);

    while (fs_iter_next_suffix(&it, STR(".c"))) {
        Path path = it.current.path;
        (void)str_take(&path, 4);
        (void)str_take_right(&path, 2);
        Path obj_path =
            str_format(&it.scratch, "build/obj/" STR_FMT ".o", STR_ARG(path_name(path)));

#if defined(_WIN32)
        if (str_contains(obj_path, STR("_posix"))) continue;
#else
        if (str_contains(obj_path, STR("_windows"))) continue;
#endif

        os_mkdir(path_parent(obj_path));

        cmd_push(&cmd_obj, STR(CC), STR("-c"), STR("-o"), obj_path);
        collect_flags(&cmd_obj);
        cmd_push(&cmd_obj, it.current.path);

        cmd_exec_da(ErrPanic, &cmd_obj);

        da_push(cmd, str_copy(obj_path, cmd->arena));
        da_clear(&cmd_obj);
    }

    fs_iter_end(&it, ErrPanic);
}

void write_compile_commands(void) {
    Arena scratch = {0};
    Cmd cmd = cmd_new(&scratch);
    cmd_push(&cmd, STR(CC), STR("-c"));
#if defined(WINDOWS)
    cmd_push(&cmd, STR("-target"), STR("x86_64-pc-windows-gnu"));
#endif
    collect_flags(&cmd);

    usize cmd_len = cmd.len;

    Path cwd = os_getcwd(&scratch);
#if defined(WINDOWS)
    cwd = str_replace(cwd, STR("\\"), STR("\\\\"), &scratch);
#endif

    FILE *compile_commands = fs_file_open(STR("build/compile_commands.json"), "w", ErrPanic);

    io_write_str(compile_commands, STR("["), ErrPanic);

    FsIter it = fs_iter_begin(STR("src"), true);
    bool first = true;
    while (fs_iter_next_suffix(&it, STR(".c"))) {
        if (first) {
            first = false;
        } else {
            io_write_str(compile_commands, STR(", "), ErrPanic);
        }

        io_write_str(compile_commands, STR("{"), ErrPanic);
        {
            io_write_fmt(compile_commands, "\"directory\": \"" STR_FMT "\",", STR_ARG(cwd));

            Path path = it.current.path;
            io_write_fmt(compile_commands, "\"file\": \"" STR_FMT "\",", STR_ARG(path));

            Path filename = path_stem(path);
            cmd_push(&cmd, STR("-o"),
                     str_format(&it.scratch, "build/obj/" STR_FMT ".o", STR_ARG(filename)));
            Str command = str_join(STR(" "), cmd.len, cmd.items, &it.scratch);
            io_write_fmt(compile_commands, "\"command\": \"" STR_FMT "\"", STR_ARG(command));
        }
        io_write_str(compile_commands, STR("}"), ErrPanic);

        cmd.len = cmd_len;
    }
    fs_iter_end(&it, ErrPanic);

    io_write_str(compile_commands, STR("]"), ErrPanic);

    fs_file_close(compile_commands, ErrPanic);
    arena_free(&scratch);
}

void create_single_header(void) {
    Arena arena = {0};

    Path single_header_file = PATH("nc.h");
    FILE* file = fs_file_open(single_header_file, "w", ErrPanic);

    io_write_str(file, STR("#ifndef _NC_H_\n"), ErrPanic);
    io_write_str(file, STR("#define _NC_H_\n\n"), ErrPanic);
    // headers

    Path defines = PATH("include/nc/defines.h");
    Str content = fs_file_read_str(defines, &arena, ErrPanic);
    for (Str line; str_try_chop_by_delim(&content, '\n', &line);) {
        if (str_contains(line, STR("#include \""))) continue;
        io_write_fmt(file, STR_FMT"\n", STR_ARG(line));
    }
    io_write_str(file, STR("\n"), ErrPanic);

    {
        FsIter it = fs_iter_begin(PATH("include/nc"), true);
        while (fs_iter_next_suffix(&it, STR(".h"))) {
            arena_reset(&arena);
            if (str_eq(it.current.path, defines)) continue;
            io_write_fmt(file, "// "STR_FMT"\n", STR_ARG(it.current.path));
            Str content = fs_file_read_str(it.current.path, &arena, ErrPanic);
            for (Str line; str_try_chop_by_delim(&content, '\n', &line);) {
                if (str_contains(line, STR("#include \""))) continue;
                io_write_fmt(file, STR_FMT"\n", STR_ARG(line));
            }
            io_write_str(file, STR("\n"), ErrPanic);
        }
        fs_iter_end(&it, ErrPanic);
    }
    io_write_str(file, STR("#endif // _NC_H_\n"), ErrPanic);

    io_write_str(file, STR("#ifdef NC_IMPLEMENTATION\n"), ErrPanic);
    // source
    {
        FsIter it = fs_iter_begin(PATH("src/nc"), true);
        while (fs_iter_next_suffix(&it, STR(".c"))) {
            arena_reset(&arena);
            if (str_contains(it.current.path, STR("_windows"))) {
                io_write_fmt(file, "#if defined(_WIN32)\n");
            }
            if (str_contains(it.current.path, STR("_posix"))) {
                io_write_fmt(file, "#if !defined(_WIN32)\n");
            }

            io_write_fmt(file, "// "STR_FMT"\n", STR_ARG(it.current.path));
            Str content = fs_file_read_str(it.current.path, &arena, ErrPanic);
            for (Str line; str_try_chop_by_delim(&content, '\n', &line);) {
                if (str_contains(line, STR("#include \""))) continue;
                io_write_fmt(file, STR_FMT"\n", STR_ARG(line));
            }

            if (str_contains(it.current.path, STR("_windows")) || str_contains(it.current.path, STR("_posix"))) {
                io_write_fmt(file, "#endif\n");
            }
            io_write_str(file, STR("\n"), ErrPanic);
        }
        fs_iter_end(&it, ErrPanic);
    }
    io_write_str(file, STR("#endif // NC_IMPLEMENTATION\n"), ErrPanic);
}

int main(void) {
    os_mkdir(PATH("build"));
    os_mkdir(PATH("build/lib"));
    os_mkdir(PATH("build/obj"));

    fs_file_write_str(PATH("build/.gitignore"), STR("*"), ErrPanic);

    Arena arena = {0};
    Cmd cmd = cmd_new(&arena);

    Str binary = STR("build/lib/libnc.a");
    cmd_push(&cmd, STR("ar"), STR("rcs"), binary);
    collect_files(&cmd);

    cmd_exec_da(ErrPanic, &cmd);

    write_compile_commands();

    da_clear(&cmd);

    cmd_push(&cmd, 
             STR(CC),
             STR("-o"),
             STR("build/test"),
             STR("tests/main.c"),
             STR("-Iinclude"),
             STR("-Lbuild/lib"),
             STR("-lnc")
    );
    collect_flags(&cmd);
    cmd_exec_da(ErrPanic, &cmd);

    create_single_header();
}
