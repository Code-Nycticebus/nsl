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

    nsl_Cmd cmd = {0};
    nsl_list_init(&cmd, &arena);

    nsl_cmd_push(&cmd, "cc", "-o", "main", "main.c")

    if (nsl_cmd_exec_list(&cmd) != 0) return 1;

    nsl_arena_free(&arena);
    return 0;
}
```

