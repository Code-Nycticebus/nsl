#include "nsl.h"

static void test_string_builder(void) {
    nsl_StrBuilder sb = {0};
    nsl_sb_push_fmt(&sb, "%s", "what");

    nsl_sb_push_c(&sb, '\0');
    printf("%s\n", sb.items); fflush(stdout);

    nsl_list_free(&sb);
}



void run_test_builder(void);
void run_test_builder(void) {
    test_string_builder();
}
