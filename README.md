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

## TODO
Here's whats left to do:
- [x] windows implementation for `os.h`
- [ ] more cmd functions like: async, run and reset, CMD macro usw
- [ ] scratch arena in some way
- [ ] dll loading

