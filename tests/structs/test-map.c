#include "nsl.h"

static void test_init(void) {
    nsl_Map map = {0};

    NSL_ASSERT(nsl_map_insert(&map, 1, 67) == true);
    NSL_ASSERT(nsl_map_insert(&map, 1, 420) == false);
    nsl_map_insert(&map, 2, 42);
    nsl_map_insert(&map, 3, 69);

    NSL_ASSERT(map.cap == 8);
    NSL_ASSERT(map.del == 0);
    NSL_ASSERT(map.len == 3);
    NSL_ASSERT(map.items);

    nsl_map_remove(&map, 2);
    NSL_ASSERT(map.cap == 8);
    NSL_ASSERT(map.del == 1);
    NSL_ASSERT(map.len == 2);
    NSL_ASSERT(map.items);

    nsl_map_resize(&map, 10);
    NSL_ASSERT(map.cap == 16);
    NSL_ASSERT(map.del == 0);
    NSL_ASSERT(map.len == 2);

    nsl_map_free(&map);
}

static void test_access(void) {
    nsl_Map map = {0};

    nsl_map_insert(&map, 2, 64);

    u64 *u = nsl_map_get(&map, 2);
    NSL_ASSERT(u);
    NSL_ASSERT(*u == 64);

    u64* n = nsl_map_get(&map, 5);
    NSL_ASSERT(n == NULL);

    nsl_map_free(&map);
}

static void test_update(void) {
    nsl_Arena arena = {0};
    nsl_Map map1 = {.arena = &arena};

    nsl_map_insert(&map1, 2, 64);

    nsl_Map map2 = {.arena = &arena};

    nsl_map_update(&map2, &map1);

    u64 *u = nsl_map_get(&map2, 2);
    NSL_ASSERT(u);
    NSL_ASSERT(*u == 64);

    nsl_arena_free(&arena);
}

static void test_remove_entries(void) {
    nsl_Map map = {0};

    nsl_map_insert(&map, 1, 420);

    NSL_ASSERT(nsl_map_remove(&map, 1) == true);
    NSL_ASSERT(nsl_map_remove(&map, 1) == false);

    nsl_map_free(&map);
}

static void test_overwriting(void) {
    nsl_Map map = {0};

    nsl_map_insert(&map, 1, 420);
    nsl_map_insert(&map, 1, 69);

    u64* u = nsl_map_get(&map, 1);
    NSL_ASSERT(u);
    NSL_ASSERT(*u == 69);

    nsl_map_free(&map);
}

static void test_clear(void) {
    nsl_Map map = {0};

    nsl_map_insert(&map, 1, 69);

    nsl_map_clear(&map);
    NSL_ASSERT(map.len == 0);
    NSL_ASSERT(map.del == 0);

    u64* u = nsl_map_get(&map, 1);
    NSL_ASSERT(u == NULL);

    nsl_map_free(&map);
}

static void test_map_subset(void) {
    nsl_Arena arena = {0};

    const nsl_MapItem n1[] = {{1, 1}, {2, 1}, {3, 1}};
    nsl_Map map1 = {.arena = &arena};
    nsl_map_extend(&map1, 3, n1);

    const nsl_MapItem n2[] = {{2, 1}, {3, 1}, {4, 1}};
    nsl_Map map2 = {.arena = &arena};
    nsl_map_extend(&map2, 3, n2);

    const nsl_MapItem n3[] = {{1, 1}, {2, 1}, {3, 1}, {4, 1}};
    nsl_Map map3 = {.arena = &arena};
    nsl_map_extend(&map3, 4, n3);

    NSL_ASSERT(nsl_map_subset(&map1, &map2) == false && "'map1' should not be a subset of 'map2'");
    NSL_ASSERT(nsl_map_subset(&map1, &map1) == true && "'map1' should be a subset of 'map1'");
    NSL_ASSERT(nsl_map_subset(&map1, &map3) == true && "'map1' should be a subset of 'map3'");
    NSL_ASSERT(nsl_map_subset(&map2, &map3) == true && "'map2' should be a subset of 'map3'");

    nsl_arena_free(&arena);
}

static void test_stress(void) {
    nsl_Map map = {0};

    const usize num_entries = 1000000; 

    for (usize i = 0; i < num_entries; ++i) {
        nsl_map_insert(&map, i, i * 2);
    }

    NSL_ASSERT(map.len == num_entries);

    for (usize i = 0; i < num_entries; ++i) {
        u64 *val = nsl_map_get(&map, i);
        NSL_ASSERT(val && *val == i * 2);
    }

    for (usize i = 0; i < num_entries; i += 2) {
        bool removed = nsl_map_remove(&map, i);
        NSL_ASSERT(removed);
    }

    NSL_ASSERT(map.len == num_entries / 2);

    for (usize i = 0; i < num_entries; ++i) {
        u64 *val = nsl_map_get(&map, i);
        if (i % 2 == 0) {
            NSL_ASSERT(val == NULL);
        } else {
            NSL_ASSERT(val && *val == i * 2);
        }
    }

    nsl_map_free(&map);
}

void run_test_map(void);
void run_test_map(void) {
    test_init();
    test_access();
    test_update();
    test_clear();
    test_remove_entries();
    test_overwriting();
    test_map_subset();
    test_stress();
}

