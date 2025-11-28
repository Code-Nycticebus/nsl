# Nycticebus Standard Library (nsl)
A single-header standard library for C, heavily inspired by [nob.h](https://github.com/tsoding/nob.h).

It aims to make programming in C better suited to my preferences, 
providing a common interface between Windows and Linux
and some extra tools like arena allocators, string utilities, and commonly used data structures.

Its a complete rewrite of [cebus](https://github.com/Code-Nycticebus/cebus).

## Usage
Download the `nsl.h` file and include it with `NSL_IMPLEMENTATION` defined.

```c
#define NSL_IMPLEMENTATION
#include "nsl.h"
```

## Example
You can create a simple build script with it.

```c
// build.c
#define NSL_IMPLEMENTATION
#include "nsl.h"

int main(void) {
    if (NSL_CMD("gcc", "-o", "main", "-Wall", "main.c")) return 1;
}
```

Or a more complicated one:
```c
// build.c
#define NSL_IMPLEMENTATION
#include "nsl.h"

int main(void) {
    nsl_Cmd cmd = {0};
    nsl_cmd_push(&cmd, "gcc");
    nsl_cmd_push(&cmd, "-o", "main");
    nsl_cmd_push(&cmd, "-Wall", "-Wextra");
    nsl_cmd_push(&cmd, "main.c");
    if (nsl_cmd_exec_list(&cmd)) return 1;
}
```


## Errors
Functions that can fail return a `0` or `NSL_NO_ERROR` on success and a non-zero (truthy) value on failure.
This mirrors the Unix convention where a non-zero exit code indicates an error. `nsl_Error` values range 1-255 are reserved for process return codes returned by `nsl_cmd_exec`.

```c
nsl_Error error = nsl_os_mkdir(NSL_PATH("path/to/directory"), NSL_DEFAULT);
if (error) {
    // handle error or crash
}
```
## Data Structures
### Memory
The `nsl_List` and `nsl_Map` data structures are valid when zero-initialized and can be used without any setup:
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

### Maps
So the `nsl_Map` is weird. Its supposed to be only a way to lookup `u64` (mostly representing an index) via a hash.
