# Nycticebus Standard Library
nsl is a single-header standard library for C, heavily inspired by [nob.h](https://github.com/tsoding/nob.h).

Its a complete rewrite of [cebus](https://github.com/Code-Nycticebus/cebus).

## Example

```c
// build.c
#define NSL_IMPLEMENTATION
#include "nsl.h"

int main(int argc, const char** argv) {
    nsl_Arena arena {0};

    const char* name = "nsl";
    nsl_Str s = nsl_str_format(&arena, "Hello, %s!", name);
    printf(NSL_STR_FMT"\n", NSL_STR_ARG(s));

    nsl_arena_free(&arena);
    return 0;
}
```

## Errors
I've decided that functions which can fail should return an error code.
Since a non-zero error code is considered truthy, I'm using the same logic for boolean-returning functions. They return true on failure and false on success. This mirrors the Unix convention where a non-zero exit code indicates an error.

## TODO
Here's whats left to do:
- [ ] windows implementation for `os.h` and `fs.h`
- [ ] make `nsl_arena_allocate_chunk()` allocate with malloc when `nsl_Arena*` is NULL
- [ ] make a zero initialized datastructure also valid (`Map`, `Set`, `List`)
- [ ] more cmd functions like: async, run and reset, CMD macro usw
- [ ] scratch arena in some way
- [ ] dll loading

