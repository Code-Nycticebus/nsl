// tests

#include "core/test-arena.c"
#include "core/test-error.c"

#include "structs/test-list.c"

#include "types/test-char.c"
#include "types/test-byte.c"
#include "types/test-int.c"
#include "types/test-str.c"

int main(void) {
    // core
    run_test_arena();
    run_test_error();

    // structs
    run_test_list();

    // types
    run_test_char();
    run_test_bytes();
    run_test_int();
    run_test_str();
}
