#include "nc/structs/set.h"

#include <NC_ASSERT.h>

static void test_set_init(void) {
    nc_Arena arena = {0};
    nc_Set set = {0};
    nc_set_init(&set, &arena);

    nc_set_add(&set, 1);
    nc_set_add(&set, 2);
    nc_set_add(&set, 3);
    NC_ASSERT(nc_set_has(&set, 1) == true);
    NC_ASSERT(nc_set_has(&set, 2) == true);
    NC_ASSERT(nc_set_has(&set, 3) == true);

    NC_ASSERT(set._cap == 8);
    NC_ASSERT(set._del == 0);
    NC_ASSERT(set.len == 3);
    NC_ASSERT(set.items);

    nc_set_remove(&set, 2);
    NC_ASSERT(set._cap == 8);
    NC_ASSERT(set._del == 1);
    NC_ASSERT(set.len == 2);
    NC_ASSERT(set.items);

    nc_set_resize(&set, 10);
    NC_ASSERT(set._cap == 16);
    NC_ASSERT(set._del == 0);
    NC_ASSERT(set.len == 2);

    nc_arena_free(&arena);
}

static void test_set_update(void) {
    nc_Arena arena = {0};
    nc_Set set1 = {0};
    nc_set_init(&set1, &arena);

    nc_set_add(&set1, 1);
    nc_set_add(&set1, 2);
    nc_set_add(&set1, 3);

    nc_Set set2 = {0};
    nc_set_init(&set2, &arena);

    nc_set_update(&set2, &set1);
    NC_ASSERT(nc_set_has(&set2, 1) == true);
    NC_ASSERT(nc_set_has(&set2, 2) == true);
    NC_ASSERT(nc_set_has(&set2, 3) == true);


    NC_ASSERT(nc_set_eq(&set1, &set2) == true);

    nc_arena_free(&arena);
}

static void test_set_subset(void) {
    nc_Arena arena = {0};

    const u64 n1[] = {1, 2, 3};
    nc_Set set1 = {0};
    nc_set_init(&set1, &arena);
    nc_set_extend(&set1, 3, n1);

    const u64 n2[] = {2, 3, 4};
    nc_Set set2 = {0};
    nc_set_init(&set2, &arena);
    nc_set_extend(&set2, 3, n2);

    const u64 n3[] = {1, 2, 3, 4};
    nc_Set set3 = {0};
    nc_set_init(&set3, &arena);
    nc_set_extend(&set3, 4, n3);

    NC_ASSERT(nc_set_subset(&set1, &set2) == false && "'set1' should not be a subset of 'set2'");
    NC_ASSERT(nc_set_subset(&set1, &set1) == true && "'set1' should be a subset of 'set1'");
    NC_ASSERT(nc_set_subset(&set1, &set3) == true && "'set1' should be a subset of 'set3'");
    NC_ASSERT(nc_set_subset(&set2, &set3) == true && "'set2' should be a subset of 'set3'");
    nc_arena_free(&arena);
}

static void test_set_disjoint(void) {
    nc_Arena arena = {0};

    const u64 test_numbers[] = {2, 3, 7, 8};
    const usize count = NC_ARRAY_LEN(test_numbers);
    nc_Set set1 = {0};
    nc_set_init(&set1, &arena);
    nc_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {2, 8, 9};
    const usize count2 = NC_ARRAY_LEN(test_numbers2);
    nc_Set set2 = {0};
    nc_set_init(&set2, &arena);
    nc_set_extend(&set2, count2, test_numbers2);

    const u64 test_numbers3[] = {4, 5, 6};
    const usize count3 = NC_ARRAY_LEN(test_numbers3);
    nc_Set set3 = {0};
    nc_set_init(&set3, &arena);
    nc_set_extend(&set3, count3, test_numbers3);

    NC_ASSERT(nc_set_disjoint(&set1, &set2) == false && "Should not be disjoint");
    NC_ASSERT(nc_set_disjoint(&set2, &set1) == false && "Should not be disjoint");
    NC_ASSERT(nc_set_disjoint(&set3, &set2) == true && "Should be disjoint");
    NC_ASSERT(nc_set_disjoint(&set3, &set1) == true && "Should be disjoint");

    nc_arena_free(&arena);
}

static void test_set_intersection(void) {
    nc_Arena arena = {0};

    const u64 test_numbers[] = {2, 3, 7, 8};
    const usize count = sizeof(test_numbers) / sizeof(test_numbers[0]);

    nc_Set set1 = {0};
    nc_set_init(&set1, &arena);
    nc_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {2, 3, 7, 8, 9};
    const usize count2 = sizeof(test_numbers2) / sizeof(test_numbers2[0]);

    nc_Set set2 = {0};
    nc_set_init(&set2, &arena);
    nc_set_extend(&set2, count2, test_numbers2);

    nc_Set inter = {0};
    nc_set_init(&inter, &arena);

    nc_set_intersection(&set1, &set2, &inter);

    NC_ASSERT(nc_set_subset(&inter, &set2) == true && "inter should be a subset");

    nc_arena_free(&arena);
}

static void test_set_difference(void) {
    nc_Arena arena = {0};

    const u64 test_numbers[] = {1, 2, 3};
    const usize count = sizeof(test_numbers) / sizeof(test_numbers[0]);

    nc_Set set1 = {0};
    nc_set_init(&set1, &arena);
    nc_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {3, 4, 5};
    const usize count2 = sizeof(test_numbers2) / sizeof(test_numbers2[0]);

    nc_Set set2 = {0};
    nc_set_init(&set2, &arena);
    nc_set_extend(&set2, count2, test_numbers2);

    nc_Set diff = {0}; 
    nc_set_init(&diff, &arena);
    nc_set_difference(&set1, &set2, &diff);

    NC_ASSERT(nc_set_has(&diff, 1) == true && "should be different");
    NC_ASSERT(nc_set_has(&diff, 2) == true && "should be different");
    NC_ASSERT(nc_set_has(&diff, 3) == false && "should not be different");
    NC_ASSERT(nc_set_has(&diff, 4) == false && "should not be symetrical");
    NC_ASSERT(nc_set_has(&diff, 5) == false && "should not be symetrical");

    nc_arena_free(&arena);
}

static void test_nc_set_union(void) {
    nc_Arena arena = {0};

    const u64 test_numbers[] = {1, 2, 3};
    const usize count = NC_ARRAY_LEN(test_numbers);
    nc_Set set1 = {0};
    nc_set_init(&set1, &arena);
    nc_set_extend(&set1, count, test_numbers);

    const u64 test_numbers2[] = {3, 4, 5};
    const usize count2 = NC_ARRAY_LEN(test_numbers);
    nc_Set set2 = {0};
    nc_set_init(&set2, &arena);
    nc_set_extend(&set2, count2, test_numbers2);

    nc_Set _unique = {0};
    nc_set_init(&_unique, &arena);
    nc_set_union(&set1, &set2, &_unique);

    NC_ASSERT(nc_set_has(&_unique, 1) == true && "nc_Set should contain 1");
    NC_ASSERT(nc_set_has(&_unique, 2) == true && "nc_Set should contain 2");
    NC_ASSERT(nc_set_has(&_unique, 3) == false && "nc_Set should not contain 3");
    NC_ASSERT(nc_set_has(&_unique, 4) == true && "nc_Set should contain 4");
    NC_ASSERT(nc_set_has(&_unique, 5) == true && "nc_Set should contain 6");

    nc_arena_free(&arena);
}

void run_test_set(void) {
    test_set_init();
    test_set_update();
    test_set_subset();
    test_set_disjoint();
    test_set_intersection();
    test_set_difference();
    test_nc_set_union();
}
