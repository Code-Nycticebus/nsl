// tests

#define NSL_IMPLEMENTATION
#include "nsl.h"
#undef NSL_IMPLEMENTATION

#include "core/test-arena.c"
#include "core/test-error.c"

#include "os/test-fs.c"
#include "os/test-file.c"
#include "os/test-cmd.c"

#include "structs/test-list.c"
#include "structs/test-map.c"
#include "structs/test-set.c"

#include "types/test-char.c"
#include "types/test-byte.c"
#include "types/test-int.c"
#include "types/test-str.c"
#include "types/test-path.c"

int main(void) {
    // core
    run_test_arena();
    run_test_error();

    // os
    run_test_fs();
    run_test_file();
    run_test_cmd();

    // structs
    run_test_list();
    run_test_map();
    run_test_set();

    // types
    run_test_char();
    run_test_bytes();
    run_test_int();
    run_test_str();
    run_test_path();
}
