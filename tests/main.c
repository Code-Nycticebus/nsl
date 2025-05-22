// tests

#include "core/test-arena.c"
#include "core/test-error.c"
#include "structs/test-list.c"

int main(void) {
    run_test_arena();
    run_test_error();
    run_test_list();
}
