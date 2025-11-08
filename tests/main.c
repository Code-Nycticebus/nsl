// tests
#include "core/test-arena.c"

#include "os/test-dir.c"
#include "os/test-file.c"
#include "os/test-cmd.c"

#include "structs/test-list.c"
#include "structs/test-map.c"
#include "structs/test-builder.c"

#include "types/test-char.c"
#include "types/test-byte.c"
#include "types/test-int.c"
#include "types/test-str.c"
#include "types/test-path.c"

int main(void) {
    printf("test core\n"); fflush(stdout);
    run_test_arena();

    printf("os\n"); fflush(stdout);
    run_test_dir();
    run_test_file();
    run_test_cmd();

    printf("structs\n"); fflush(stdout);
    run_test_list();
    run_test_map();
    run_test_builder();

    printf("types\n"); fflush(stdout);
    run_test_char();
    run_test_bytes();
    run_test_int();
    run_test_str();
    run_test_path();
}
