# Nycticebus Standard Library (nsl)
A single-header standard library for C, heavily inspired by [nob.h](https://github.com/tsoding/nob.h).

It aims to make programming in C better suited to my preferences, 
providing a common interface between Windows and Linux,
and some extra tools like arena allocators, string utilities, and commonly used data structures.

Its a complete rewrite of [cebus](https://github.com/Code-Nycticebus/cebus).

## Example

```c
// build.c
#define NSL_IMPLEMENTATION
#include "nsl.h"

int main(void) {
    int result = 0;
    nsl_Arena arena = {0};

    nsl_Path file = NSL_PATH("main.c");
    const char* out = nsl_str_to_cstr(nsl_path_stem(file), &arena);
    if (NSL_CMD("gcc", "-o", out, "-Wall", file.data)) NSL_DEFER(-1);

defer:
    nsl_arena_free(&arena);
    return result;
}
```

## Errors
Functions that can fail return a `0` or `NSL_NO_ERROR` on success and a non-zero (truthy) value on failure.
This mirrors the Unix convention where a non-zero exit code indicates an error.

```c
nsl_Error error = nsl_os_mkdir(NSL_PATH("path/to/directory"));
if (error) {
    // handle error or crash
}
```

## Data Structures
The `nsl_List`, `nsl_Map`, and `nsl_Set` data structures are valid when zero-initialized and can be used without any setup:
```c
nsl_Map map = {0};
// map operations
nsl_map_free(&map);
```

Or, you can allocate all internal memory from a `nsl_Arena`.
```c
nsl_Arena arena = {0};
nsl_Map map = {.arena = &arena};
// map operations
nsl_arena_free(&arena);
```
