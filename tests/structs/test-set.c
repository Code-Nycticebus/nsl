#include "nsl.h"

#include <assert.h>

static void test_set_init(void) {
    nsl_Set set = {0};

    nsl_set_add(&set, 1);
    nsl_set_add(&set, 2);
    nsl_set_add(&set, 3);
    NSL_ASSERT(nsl_set_has(&set, 1) == true);
    NSL_ASSERT(nsl_set_has(&set, 2) == true);
    NSL_ASSERT(nsl_set_has(&set, 3) == true);

    NSL_ASSERT(set._cap == 8);
    NSL_ASSERT(set._del == 0);
    NSL_ASSERT(set.len == 3);
    NSL_ASSERT(set.items);

    nsl_set_remove(&set, 2);
    NSL_ASSERT(set._cap == 8);
    NSL_ASSERT(set._del == 1);
    NSL_ASSERT(set.len == 2);
    NSL_ASSERT(set.items);

    nsl_set_resize(&set, 10);
    NSL_ASSERT(set._cap == 16);
    NSL_ASSERT(set._del == 0);
    NSL_ASSERT(set.len == 2);

    nsl_set_free(&set);
}

static void test_set_update(void) {
    nsl_Arena arena = {0};
    nsl_Set set1 = {.arena = &arena};

    nsl_set_add(&set1, 1);
    nsl_set_add(&set1, 2);
    nsl_set_add(&set1, 3);

    nsl_Set set2 = {.arena = &arena};

    nsl_set_update(&set2, &set1);
    NSL_ASSERT(nsl_set_has(&set2, 1) == true);
    NSL_ASSERT(nsl_set_has(&set2, 2) == true);
    NSL_ASSERT(nsl_set_has(&set2, 3) == true);


    NSL_ASSERT(nsl_set_eq(&set1, &set2) == true);

    nsl_arena_free(&arena);
}

static void test_set_subset(void) {
    nsl_Arena arena = {0};

    const u64 n1[] = {1, 2, 3};
    nsl_Set set1 = {.arena = &arena};
    nsl_set_extend(&set1, 3, n1);

    const u64 n2[] = {2, 3, 4};
    nsl_Set set2 = {.arena = &arena};
    nsl_set_extend(&set2, 3, n2);

    const u64 n3[] = {1, 2, 3, 4};
    nsl_Set set3 = {.arena = &arena};
    nsl_set_extend(&set3, 4, n3);

    NSL_ASSERT(nsl_set_subset(&set1, &set2) == false && "'set1' should not be a subset of 'set2'");
    NSL_ASSERT(nsl_set_subset(&set1, &set1) == true && "'set1' should be a subset of 'set1'");
    NSL_ASSERT(nsl_set_subset(&set1, &set3) == true && "'set1' should be a subset of 'set3'");
    NSL_ASSERT(nsl_set_subset(&set2, &set3) == true && "'set2' should be a subset of 'set3'");

    nsl_arena_free(&arena);
}

static void test_set_disjoint(void) {
    nsl_Arena arena = {0};

    const u64 test_numbers[] = {2, 3, 7, 8};
    const usize count = NSL_ARRAY_LEN(test_numbers);
    nsl_Set set1 = {.arena = &arena};
    nsl_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {2, 8, 9};
    const usize count2 = NSL_ARRAY_LEN(test_numbers2);
    nsl_Set set2 = {.arena = &arena};
    nsl_set_extend(&set2, count2, test_numbers2);

    const u64 test_numbers3[] = {4, 5, 6};
    const usize count3 = NSL_ARRAY_LEN(test_numbers3);
    nsl_Set set3 = {.arena = &arena};
    nsl_set_extend(&set3, count3, test_numbers3);

    NSL_ASSERT(nsl_set_disjoint(&set1, &set2) == false && "Should not be disjoint");
    NSL_ASSERT(nsl_set_disjoint(&set2, &set1) == false && "Should not be disjoint");
    NSL_ASSERT(nsl_set_disjoint(&set3, &set2) == true && "Should be disjoint");
    NSL_ASSERT(nsl_set_disjoint(&set3, &set1) == true && "Should be disjoint");

    nsl_arena_free(&arena);
}

static void test_set_intersection(void) {
    nsl_Arena arena = {0};

    const u64 test_numbers[] = {2, 3, 7, 8};
    const usize count = sizeof(test_numbers) / sizeof(test_numbers[0]);

    nsl_Set set1 = {.arena = &arena};
    nsl_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {2, 3, 7, 8, 9};
    const usize count2 = sizeof(test_numbers2) / sizeof(test_numbers2[0]);

    nsl_Set set2 = {.arena = &arena};
    nsl_set_extend(&set2, count2, test_numbers2);

    nsl_Set inter = {.arena = &arena};

    nsl_set_intersection(&set1, &set2, &inter);

    NSL_ASSERT(nsl_set_subset(&inter, &set2) == true && "inter should be a subset");

    nsl_arena_free(&arena);
}

static void test_set_difference(void) {
    nsl_Arena arena = {0};

    const u64 test_numbers[] = {1, 2, 3};
    const usize count = sizeof(test_numbers) / sizeof(test_numbers[0]);

    nsl_Set set1 = {.arena = &arena};
    nsl_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {3, 4, 5};
    const usize count2 = sizeof(test_numbers2) / sizeof(test_numbers2[0]);

    nsl_Set set2 = {.arena = &arena};
    nsl_set_extend(&set2, count2, test_numbers2);

    nsl_Set diff = {.arena = &arena}; 
    nsl_set_difference(&set1, &set2, &diff);

    NSL_ASSERT(nsl_set_has(&diff, 1) == true && "should be different");
    NSL_ASSERT(nsl_set_has(&diff, 2) == true && "should be different");
    NSL_ASSERT(nsl_set_has(&diff, 3) == false && "should not be different");
    NSL_ASSERT(nsl_set_has(&diff, 4) == false && "should not be symetrical");
    NSL_ASSERT(nsl_set_has(&diff, 5) == false && "should not be symetrical");

    nsl_arena_free(&arena);
}

static void test_nc_set_union(void) {
    nsl_Arena arena = {0};

    const u64 test_numbers[] = {1, 2, 3};
    const usize count = NSL_ARRAY_LEN(test_numbers);
    nsl_Set set1 = {.arena = &arena};
    nsl_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {3, 4, 5};
    const usize count2 = NSL_ARRAY_LEN(test_numbers);
    nsl_Set set2 = {.arena = &arena};
    nsl_set_extend(&set2, count2, test_numbers2);

    nsl_Set _unique = {.arena = &arena};
    nsl_set_union(&set1, &set2, &_unique);

    NSL_ASSERT(nsl_set_has(&_unique, 1) == true && "nsl_Set should contain 1");
    NSL_ASSERT(nsl_set_has(&_unique, 2) == true && "nsl_Set should contain 2");
    NSL_ASSERT(nsl_set_has(&_unique, 3) == false && "nsl_Set should not contain 3");
    NSL_ASSERT(nsl_set_has(&_unique, 4) == true && "nsl_Set should contain 4");
    NSL_ASSERT(nsl_set_has(&_unique, 5) == true && "nsl_Set should contain 6");

    nsl_arena_free(&arena);
}

void run_test_set(void);
void run_test_set(void) {
    test_set_init();
    test_set_update();
    test_set_subset();
    test_set_disjoint();
    test_set_intersection();
    test_set_difference();
    test_nc_set_union();
}
