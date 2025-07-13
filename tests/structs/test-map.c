#include "nsl.h"

static void test_init(void) {
    nsl_Map map = {.type=NSL_MAP_U64};

    NSL_ASSERT(map.type == NSL_MAP_U64);
    NSL_ASSERT(map.cap == 0);
    NSL_ASSERT(map.del == 0);
    NSL_ASSERT(map.len == 0);
    NSL_ASSERT(map.items == NULL);

    nsl_map_insert_u64(&map, 1, 420);
    nsl_map_insert_u64(&map, 2, 42);
    nsl_map_insert_u64(&map, 3, 69);

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

    nsl_map_insert_i64(&map, 1, -64);
    nsl_map_insert_u64(&map, 2, 64);
    nsl_map_insert_f64(&map, 3, 3.141);
    nsl_map_insert_ptr(&map, 4, &map);

    i64 *i = nsl_map_get_i64(&map, 1);
    NSL_ASSERT(i);
    NSL_ASSERT(*i == -64);

    u64 *u = nsl_map_get_u64(&map, 2);
    NSL_ASSERT(u);
    NSL_ASSERT(*u == 64);

    f64 *f = nsl_map_get_f64(&map, 3);
    NSL_ASSERT(f);
    NSL_ASSERT(3.140 < *f && *f < 3.142);

    void* ptr = nsl_map_get_ptr(&map, 4);
    NSL_ASSERT(ptr);
    NSL_ASSERT(ptr == &map);

    void* n = nsl_map_get_ptr(&map, 5);
    NSL_ASSERT(n == NULL);

    nsl_map_free(&map);
}

static void test_update(void) {
    nsl_Arena arena = {0};
    nsl_Map map1 = {.arena = &arena};

    nsl_map_insert_i64(&map1, 1, -64);
    nsl_map_insert_u64(&map1, 2, 64);
    nsl_map_insert_f64(&map1, 3, 3.141);
    nsl_map_insert_ptr(&map1, 4, &map1);

    nsl_Map map2 = {.arena = &arena};

    nsl_map_update(&map2, &map1);

    i64 *i = nsl_map_get_i64(&map2, 1);
    NSL_ASSERT(i);
    NSL_ASSERT(*i == -64);

    u64 *u = nsl_map_get_u64(&map2, 2);
    NSL_ASSERT(u);
    NSL_ASSERT(*u == 64);

    f64 *f = nsl_map_get_f64(&map2, 3);
    NSL_ASSERT(f);
    NSL_ASSERT(3.140 < *f && *f < 3.142);

    void* ptr = nsl_map_get_ptr(&map2, 4);
    NSL_ASSERT(ptr);
    NSL_ASSERT(ptr == &map1);

    nsl_arena_free(&arena);
}

static void test_remove_entries(void) {
    nsl_Map map = {0};

    nsl_map_insert_u64(&map, 1, 420);

    NSL_ASSERT(nsl_map_remove(&map, 1) == true);
    NSL_ASSERT(nsl_map_remove(&map, 1) == false);

    nsl_map_free(&map);
}

static void test_overwriting(void) {
    nsl_Map map = {0};

    nsl_map_insert_u64(&map, 1, 420);
    nsl_map_insert_u64(&map, 1, 69);

    u64* u = nsl_map_get_u64(&map, 1);
    NSL_ASSERT(u);
    NSL_ASSERT(*u == 69);

    nsl_map_free(&map);
}

static void test_clear(void) {
    nsl_Map map = {0};

    nsl_map_insert_u64(&map, 1, 69);

    nsl_map_clear(&map);
    NSL_ASSERT(map.len == 0);
    NSL_ASSERT(map.del == 0);

    u64* u = nsl_map_get_ptr(&map, 1);
    NSL_ASSERT(u == NULL);

    nsl_map_free(&map);
}

static void test_stress(void) {
    nsl_Map map = {0};

    const usize num_entries = 1000000; 

    for (usize i = 0; i < num_entries; ++i) {
        nsl_map_insert_u64(&map, i, i * 2);
    }

    NSL_ASSERT(map.len == num_entries);

    for (usize i = 0; i < num_entries; ++i) {
        u64 *val = nsl_map_get_u64(&map, i);
        NSL_ASSERT(val && *val == i * 2);
    }

    for (usize i = 0; i < num_entries; i += 2) {
        bool removed = nsl_map_remove(&map, i);
        NSL_ASSERT(removed);
    }

    NSL_ASSERT(map.len == num_entries / 2);

    for (usize i = 0; i < num_entries; ++i) {
        u64 *val = nsl_map_get_u64(&map, i);
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
    test_stress();
}

