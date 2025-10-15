#include "nsl.h"

static void test_string_builder(void) {
    nsl_StrBuilder sb = {0};

    nsl_sb_push_fmt(&sb, "%s", "what");
    nsl_Str s = nsl_sb_to_str(&sb);
    NSL_ASSERT(nsl_str_eq(s, NSL_STR("what")));

    nsl_list_free(&sb);
}

static void test_bytes_builder(void) {
    nsl_ByteBuffer bb = {0};

    int data[4] = {1, 2, 3, 4};
    nsl_bb_push_bytes(&bb, sizeof(data), data);

    nsl_Bytes bytes = nsl_bb_to_bytes(&bb);
    NSL_ASSERT(nsl_bytes_eq(bytes, nsl_bytes_from_parts(sizeof(data), data)));

    nsl_list_free(&bb);
}


void run_test_builder(void);
void run_test_builder(void) {
    test_string_builder();
    test_bytes_builder();
}
