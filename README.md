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
    if (NSL_CMD("gcc", "-o", out, "-Werror", "-Wall", "-Wextra", file.data)) return 1;

    nsl_arena_free(&arena);
    return 0;
}
```

## Errors
I've decided that functions which can fail should return an error code.
Since a non-zero error code is considered truthy, I'm using the same logic for boolean-returning functions. They return true on failure and false on success.
This mirrors the Unix convention where a non-zero exit code indicates an error.

## Data Structures

The `nsl_List`, `nsl_Map`, and `nsl_Set` types are provided as data structures. All of them are valid when zero-initialized and can be used without any setup:
```c
nsl_Map map = {0};
nsl_map_insert_u64(hash, 69);
nsl_map_free(&map);
```

Alternatively, you can allocate all internal memory from a `nsl_Arena`.
```c
nsl_Map map = {.arena = &arena};
nsl_map_insert_u64(hash, 69);
nsl_arena_free(&arena); // frees everything in the arena, including the map
```

Or, if you want to free just the map itself (even if it's arena-backed):
```c
nsl_Map map = {.arena = &arena};
nsl_map_insert_u64(hash, 69);
nsl_map_free(&arena); // only removes the map, arena remains intact
```


## TODO
Here's whats left to do:
- [ ] windows implementation for `os.h` and `fs.h`
- [x] make `nsl_arena_allocate_chunk()` allocate with malloc when `nsl_Arena*` is `NULL`
- [x] make a zero initialized datastructure also valid (`Map`, `Set`, `List`)
- [x] depricate ds init functions
- [ ] more cmd functions like: async, run and reset, CMD macro usw
- [ ] scratch arena by just allocating a chunk that i can reset? and allocate a certain size that i will assert if i overdo it??? 
- [ ] dll loading
- [ ] overthink the `fs.h` api

