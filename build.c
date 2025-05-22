#define CEBUS_IMPLEMENTATION
#include "bootstrap.h"

#define CC CEBUS_COMPILER

void collect_flags(Cmd *cmd) {
  cmd_push(cmd, STR("-Wall"), STR("-Werror"), STR("-Wextra"), STR("-Wpedantic"));
  cmd_push(cmd, STR("-std=c99"));
  cmd_push(cmd, STR("-Ilibs"));
  cmd_push(cmd, STR("-Iinclude"));
}

void collect_files(Cmd *cmd) {
  FsIter it = fs_iter_begin(STR("src"), true);
  Cmd cmd_obj = cmd_new(&it.scratch);

  while (fs_iter_next_suffix(&it, STR(".c"))) {
    Path path = it.current.path;
    (void)str_take(&path, 4);
    (void)str_take_right(&path, 2);
    Path obj_path = str_format(&it.scratch, "build/obj/" STR_FMT ".o", STR_ARG(path));

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
      cmd_push(&cmd, STR("-o"), str_format(&it.scratch, "build/obj/" STR_FMT ".o", STR_ARG(filename)));
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
  cmd_push(&cmd, STR(CC), STR("-o"), STR("build/test"), STR("tests/main.c"),
           STR("-Iinclude"), STR("-Lbuild/lib"), STR("-lnc"));
  cmd_exec_da(ErrPanic, &cmd);
}
