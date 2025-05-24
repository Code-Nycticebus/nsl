#include "nc/structs/map.h"

#include "nc/types/str.h"

static void test_init(void) {
    nc_Arena arena = {0};
    nc_Map map = {0};
    nc_map_init(&map, &arena);

    map.type = NC_MAP_U64;
    assert(map._cap == 0);
    assert(map._del == 0);
    assert(map.len == 0);
    assert(map._nodes == NULL);

    nc_map_insert_u64(&map, 1, 420);
    nc_map_insert_u64(&map, 2, 42);
    nc_map_insert_u64(&map, 3, 69);

    assert(map._cap == 8);
    assert(map._del == 0);
    assert(map.len == 3);
    assert(map._nodes);

    nc_map_remove(&map, 2);
    assert(map._cap == 8);
    assert(map._del == 1);
    assert(map.len == 2);
    assert(map._nodes);

    nc_map_resize(&map, 10);
    assert(map._cap == 16);
    assert(map._del == 0);
    assert(map.len == 2);

    nc_arena_free(&arena);
}

static void test_access(void) {
    nc_Arena arena = {0};
    nc_Map map = {0};
    nc_map_init(&map, &arena);

    nc_map_insert_i64(&map, 1, -64);
    nc_map_insert_u64(&map, 2, 64);
    nc_map_insert_f64(&map, 3, 3.141);
    nc_map_insert_ptr(&map, 4, &map);

    i64 *i = nc_map_get_i64(&map, 1);
    assert(i);
    assert(*i == -64);

    u64 *u = nc_map_get_u64(&map, 2);
    assert(u);
    assert(*u == 64);

    f64 *f = nc_map_get_f64(&map, 3);
    assert(f);
    assert(3.140 < *f && *f < 3.142);

    void* ptr = nc_map_get_ptr(&map, 4);
    assert(ptr);
    assert(ptr == &map);

    void* n = nc_map_get_ptr(&map, 5);
    assert(n == NULL);

    nc_arena_free(&arena);
}

static void test_update(void) {
    nc_Arena arena = {0};
    nc_Map map1 = {0};
    nc_map_init(&map1, &arena);

    nc_map_insert_i64(&map1, 1, -64);
    nc_map_insert_u64(&map1, 2, 64);
    nc_map_insert_f64(&map1, 3, 3.141);
    nc_map_insert_ptr(&map1, 4, &map1);

    nc_Map map2 = {0};
    nc_map_init(&map2, &arena);

    nc_map_update(&map2, &map1);

    i64 *i = nc_map_get_i64(&map2, 1);
    assert(i);
    assert(*i == -64);

    u64 *u = nc_map_get_u64(&map2, 2);
    assert(u);
    assert(*u == 64);

    f64 *f = nc_map_get_f64(&map2, 3);
    assert(f);
    assert(3.140 < *f && *f < 3.142);

    void* ptr = nc_map_get_ptr(&map2, 4);
    assert(ptr);
    assert(ptr == &map1);

    nc_arena_free(&arena);
}

static void test_remove_entries(void) {
    nc_Arena arena = {0};
    nc_Map map = {0};
    nc_map_init(&map, &arena);

    nc_map_insert_u64(&map, 1, 420);

    assert(nc_map_remove(&map, 1) == true);
    assert(nc_map_remove(&map, 1) == false);

    nc_arena_free(&arena);
}

static void test_overwriting(void) {
    nc_Arena arena = {0};
    nc_Map map = {0};
    nc_map_init(&map, &arena);

    nc_map_insert_u64(&map, 1, 420);
    nc_map_insert_u64(&map, 1, 69);

    u64* u = nc_map_get_u64(&map, 1);
    assert(u);
    assert(*u == 69);

    nc_arena_free(&arena);
}

static void test_clear(void) {
    nc_Arena arena = {0};
    nc_Map map = {0};
    nc_map_init(&map, &arena);

    nc_map_insert_u64(&map, 1, 69);

    nc_map_clear(&map);
    assert(map.len == 0);
    assert(map._del == 0);

    u64* u = nc_map_get_ptr(&map, 1);
    assert(u == NULL);

    nc_arena_free(&arena);
}

static void test_stress(void) {
    nc_Arena arena = {0};
    nc_Map map = {0};
    nc_map_init(&map, &arena);
    map.type = NC_MAP_U64;

    const usize num_entries = 1000000; 

    for (usize i = 0; i < num_entries; ++i) {
        nc_map_insert_u64(&map, i, i * 2);
    }

    assert(map.len == num_entries);

    for (usize i = 0; i < num_entries; ++i) {
        u64 *val = nc_map_get_u64(&map, i);
        assert(val && *val == i * 2);
    }

    for (usize i = 0; i < num_entries; i += 2) {
        bool removed = nc_map_remove(&map, i);
        assert(removed);
    }

    assert(map.len == num_entries / 2);

    for (usize i = 0; i < num_entries; ++i) {
        u64 *val = nc_map_get_u64(&map, i);
        if (i % 2 == 0) {
            assert(val == NULL);
        } else {
            assert(val && *val == i * 2);
        }
    }


    nc_arena_free(&arena);
}

void run_test_map(void) {
    test_init();
    test_access();
    test_update();
    test_remove();
    test_overwriting();
    test_stress();
}
