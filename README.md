# Nycticebus Standard Library
nsl is a single-header standard library for C, heavily inspired by [nob.h](https://github.com/tsoding/nob.h).

Its a complete rewrite of [cebus](https://github.com/Code-Nycticebus/cebus).

## Example

```c
// build.c
#define NSL_IMPLEMENTATION
#include "nsl.h"

int main(void) {
    nsl_Arena arena = {0};

    nsl_Path file = NSL_PATH("main.c");
    const char* out = nsl_str_to_cstr(nsl_path_stem(file), &arena);
    if (NSL_CMD("gcc", "-o", out, "-Wall", file.data)) return 1;

    nsl_arena_free(&arena);
    return 0;
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
