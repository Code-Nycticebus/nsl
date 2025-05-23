#include "nc/structs/list.h"

#include <stdio.h>

static void test_vec(void) {
    nc_Arena arena = {0};
    const usize n = 10;
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);
    for (usize i = 0; i < n; ++i) {
        nc_list_push(&list, i + 1);
    }

    for (usize i = 0; i < list.len; ++i) {
        NC_ASSERT(list.items[i] == i + 1 &&
               "Numbers were not pushed on the stack correctly");
    }
    nc_list_clear(&list);
    NC_ASSERT(list.len == 0 && "Clearing did not reset list.len");

    nc_arena_free(&arena);
}

static void test_list_init(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_from_static(&list, &arena, (usize[]){1, 2, 3, 4, 5});

    NC_ASSERT(list.len == 5 && "Did not set len correctly");
    NC_ASSERT(list.cap == 5 && "Did not set cap correctly");

    NC_ASSERT(list.items[0] == 1 && "Did not init correctly");
    NC_ASSERT(list.items[2] == 3 && "Did not init correctly");
    NC_ASSERT(list.items[4] == 5 && "Did not init correctly");

    nc_arena_free(&arena);
}

static usize times_two(usize v) { return v * 2; }

static void test_map(void) {
    nc_Arena arena = {0};
    const usize n = 10;
    nc_List(usize) list = nc_list_new(&arena);
    for (usize i = 0; i < n; ++i) {
        nc_list_push(&list, i);
    }

    nc_list_map(&list, &list, times_two);

    for (usize i = 0; i < list.len; ++i) {
        NC_ASSERT(list.items[i] == i * 2 && "Mapping did not multiply by two");
    }
    nc_arena_free(&arena);
}

static i32 compare(const void *a, const void *b) {
    return *(const usize *)a - *(const usize *)b;
}

static void test_sort(void) {
    nc_Arena arena = {0};

    nc_List(usize) list = {0};
    nc_list_from_static(&list, &arena, (usize[]){0, 3, 2, 5, 1, 4, 8, 7, 9, 6});

    nc_list_sort(&list, compare);

    for (usize i = 0; i < list.len; ++i) {
        NC_ASSERT(list.items[i] == i && "sorting did not work correctly");
    }

    nc_arena_free(&arena);
}

static void test_last(void) {
    nc_Arena arena = {0};

    nc_List(i32) list = {0};
    nc_list_init(&list, &arena);
    NC_ASSERT(nc_list_is_empty(&list) && "List should be initialized empty");
    nc_list_push(&list, 10);
    nc_list_push(&list, 20);
    int last = nc_list_last(&list);
    NC_ASSERT(last == 20 && "Last is not the correct number");
    int first = nc_list_first(&list);
    NC_ASSERT(first == 10 && "First is not the correct number");

    nc_arena_free(&arena);
}

static void test_extend(void) {
    nc_Arena arena = {0};
    nc_List(i32) list = {0};
    nc_list_init(&list, &arena);

    nc_list_extend(&list, 3, ((int[]){1, 2, 3}));
    NC_ASSERT(list.len == 3 && "List did not extend correctly");
    NC_ASSERT(list.items[0] == 1 && list.items[1] == 2 && list.items[2] == 3 &&
           "List did not extend correctly");

    i32 array[] = {1, 2, 3};
    nc_list_extend(&list, 3, array);
    NC_ASSERT(list.items[3] == 1 && list.items[4] == 2 && list.items[5] == 3 &&
           "List did not extend correctly");

    nc_list_extend_da(&list, &list);
    NC_ASSERT(list.items[6] == 1 && list.items[7] == 2 && list.items[8] == 3 &&
           "List did not extend correctly");

    nc_arena_free(&arena);
}

static void test_reserve(void) {
    nc_Arena arena = {0};
    nc_List(i32) list = {0};
    nc_list_init(&list, &arena);
    nc_list_resize(&list, 20);
    NC_ASSERT(list.cap == 20 && "Capacity was not increased");
    nc_list_reserve(&list, 50);
    NC_ASSERT(list.cap == 80 && "Capacity was not increased");
    nc_arena_free(&arena);
}

static void test_reverse(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);
    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nc_list_push(&list, i + 1);
    }
    nc_list_reverse(&list);
    for (usize i = 0; i < n; i++) {
        NC_ASSERT(list.items[i] == n - i && "List was not reversed correctly");
    }

    nc_arena_free(&arena);
}

static bool is_odd(usize i) { return i % 2 == 0; }

static void test_filter(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);
    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nc_list_push(&list, i);
    }

    nc_list_filter(&list, &list, is_odd);

    NC_ASSERT(list.items[1] == 2 && "list was not filtered correctly");
    NC_ASSERT(list.items[2] == 4 && "list was not filtered correctly");
    NC_ASSERT(list.items[3] == 6 && "list was not filtered correctly");

    nc_arena_free(&arena);
}

typedef struct {
    usize a;
} Ctx;
static bool filter_with_context(Ctx *ctx, usize a) { return ctx->a < a; }

static void test_filter_ctx(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);
    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nc_list_push(&list, i);
    }

    Ctx ctx = {.a = 4};
    nc_list_filter_ctx(&list, &list, filter_with_context, &ctx);

    NC_ASSERT(list.items[0] == 5 && "list was not filtered correctly");
    NC_ASSERT(list.items[1] == 6 && "list was not filtered correctly");
    NC_ASSERT(list.items[2] == 7 && "list was not filtered correctly");

    nc_arena_free(&arena);
}

static void test_copy(void) {
    nc_Arena arena = {0};
    nc_List(usize) l1 = nc_list_new(&arena);
    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nc_list_push(&l1, i + 1);
    }

    nc_List(usize) l2 = nc_list_new(&arena);
    nc_list_copy(&l1, &l2);
    NC_ASSERT(l1.len == l2.len && "list was not copied correctly");
    for (usize i = 0; i < l2.len; i++) {
        NC_ASSERT(l1.items[i] == l2.items[i] && "list was not copied correctly");
    }

    nc_arena_free(&arena);
}

static void test_pop(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);
    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nc_list_push(&list, i + 1);
    }

    for (usize i = list.len; 0 < i; i--) {
        NC_ASSERT(nc_list_pop(&list) == i && "Poping not correctly");
    }

    NC_ASSERT(nc_list_is_empty(&list) == true && "After all that not empty");

    nc_arena_free(&arena);
}

static void test_insert(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);

    nc_list_push(&list, 1);
    nc_list_push(&list, 4);

    nc_list_insert(&list, 2, 1);
    nc_list_insert(&list, 3, 2);

    NC_ASSERT(list.len == 4 && "");
    NC_ASSERT(list.items[0] == 1);
    NC_ASSERT(list.items[1] == 2);
    NC_ASSERT(list.items[2] == 3);
    NC_ASSERT(list.items[3] == 4);

    nc_arena_free(&arena);
}

static void test_remove(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};
    nc_list_init(&list, &arena);

    nc_list_push(&list, 1);
    nc_list_push(&list, 2);
    nc_list_push(&list, 3);
    nc_list_push(&list, 4);

    nc_list_remove(&list, 1);
    nc_list_remove(&list, 1);

    NC_ASSERT(list.len == 2 && "");
    NC_ASSERT(list.items[0] == 1 && "");
    NC_ASSERT(list.items[1] == 4 && "");

    nc_arena_free(&arena);
}

static void test_for_each(void) {
    nc_Arena arena = {0};
    nc_List(usize) list = {0};

    usize el[] = {1, 2, 3, 4};
    nc_list_from_static(&list, &arena, el);

    usize i = 0;
    nc_list_for_each(usize *, element, &list) {
        NC_ASSERT(*element == el[i] && "");
        i++;
    }
    NC_ASSERT(i == list.len && "");
}

void run_test_list(void) {
    test_vec();
    test_list_init();
    test_map();
    test_extend();
    test_reserve();
    test_reverse();
    test_sort();
    test_last();
    test_filter();
    test_filter_ctx();
    test_copy();
    test_pop();
    test_insert();
    test_remove();
    test_for_each();
}
