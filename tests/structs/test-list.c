#include "nsl.h"

#include <stdio.h>

static void test_vec(void) {
    const usize n = 10;
    nsl_List(usize) list = {0};
    for (usize i = 0; i < n; ++i) {
        nsl_list_push(&list, i + 1);
    }

    for (usize i = 0; i < list.len; ++i) {
        NSL_ASSERT(list.items[i] == i + 1 &&
               "Numbers were not pushed on the stack correctly");
    }
    nsl_list_clear(&list);
    NSL_ASSERT(list.len == 0 && "Clearing did not reset list.len");

    nsl_list_free(&list);
}

static void test_list_init(void) {
    nsl_List(usize) list = {0};
    nsl_list_extend_static(&list, (usize[]){1, 2, 3, 4, 5});

    NSL_ASSERT(list.len == 5 && "Did not set len correctly");
    NSL_ASSERT(list.cap == 5 && "Did not set cap correctly");

    NSL_ASSERT(list.items[0] == 1 && "Did not init correctly");
    NSL_ASSERT(list.items[2] == 3 && "Did not init correctly");
    NSL_ASSERT(list.items[4] == 5 && "Did not init correctly");

    nsl_list_free(&list);
}

static usize times_two(usize v) { return v * 2; }

static void test_map(void) {
    const usize n = 10;
    nsl_List(usize) list = {0};

    for (usize i = 0; i < n; ++i) {
        nsl_list_push(&list, i);
    }

    nsl_list_map(&list, &list, times_two);

    for (usize i = 0; i < list.len; ++i) {
        NSL_ASSERT(list.items[i] == i * 2 && "Mapping did not multiply by two");
    }

    nsl_list_free(&list);
}

static i32 compare(const void *a, const void *b) {
    return (i32)(*(const usize *)a - *(const usize *)b);
}

static void test_sort(void) {
    nsl_List(usize) list = {0};

    nsl_list_extend_static(&list, (usize[]){0, 3, 2, 5, 1, 4, 8, 7, 9, 6});

    nsl_list_sort(&list, compare);

    for (usize i = 0; i < list.len; ++i) {
        NSL_ASSERT(list.items[i] == i && "sorting did not work correctly");
    }

    nsl_list_free(&list);
}

static void test_last(void) {
    nsl_List(i32) list = {0};

    NSL_ASSERT(nsl_list_is_empty(&list) && "List should be initialized empty");
    nsl_list_push(&list, 10);
    nsl_list_push(&list, 20);
    int last = nsl_list_last(&list);
    NSL_ASSERT(last == 20 && "Last is not the correct number");
    int first = nsl_list_first(&list);
    NSL_ASSERT(first == 10 && "First is not the correct number");

    nsl_list_free(&list);
}

static void test_extend(void) {
    nsl_List(i32) list = {0};

    nsl_list_extend_static(&list, ((int[]){1, 2, 3}));
    NSL_ASSERT(list.len == 3 && "List did not extend correctly");
    NSL_ASSERT(list.items[0] == 1 && list.items[1] == 2 && list.items[2] == 3 &&
           "List did not extend correctly");

    i32 array[] = {1, 2, 3};
    nsl_list_extend_static(&list, array);
    NSL_ASSERT(list.items[3] == 1 && list.items[4] == 2 && list.items[5] == 3 &&
           "List did not extend correctly");

    nsl_list_extend_list(&list, &list);
    NSL_ASSERT(list.items[6] == 1 && list.items[7] == 2 && list.items[8] == 3 &&
           "List did not extend correctly");

    nsl_list_free(&list);
}

static void test_reserve(void) {
    nsl_List(i32) list = {0};

    nsl_list_resize(&list, 20);
    NSL_ASSERT(list.cap == 20 && "Capacity was not increased");
    nsl_list_reserve(&list, 50);
    NSL_ASSERT(list.cap == 80 && "Capacity was not increased");

    nsl_list_free(&list);
}

static void test_reverse(void) {
    nsl_List(usize) list = {0};

    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nsl_list_push(&list, i + 1);
    }
    nsl_list_reverse(&list);
    for (usize i = 0; i < n; i++) {
        NSL_ASSERT(list.items[i] == n - i && "List was not reversed correctly");
    }

    nsl_list_free(&list);
}

static bool is_odd(usize i) { return i % 2 == 0; }

static void test_filter(void) {
    nsl_List(usize) list = {0};

    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nsl_list_push(&list, i);
    }

    nsl_list_filter(&list, &list, is_odd);

    NSL_ASSERT(list.items[1] == 2 && "list was not filtered correctly");
    NSL_ASSERT(list.items[2] == 4 && "list was not filtered correctly");
    NSL_ASSERT(list.items[3] == 6 && "list was not filtered correctly");

    nsl_list_free(&list);
}

typedef struct {
    usize a;
} Ctx;
static bool filter_with_context(Ctx *ctx, usize a) { return ctx->a < a; }

static void test_filter_ctx(void) {
    nsl_List(usize) list = {0};

    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nsl_list_push(&list, i);
    }

    Ctx ctx = {.a = 4};
    nsl_list_filter_ctx(&list, &list, filter_with_context, &ctx);

    NSL_ASSERT(list.items[0] == 5 && "list was not filtered correctly");
    NSL_ASSERT(list.items[1] == 6 && "list was not filtered correctly");
    NSL_ASSERT(list.items[2] == 7 && "list was not filtered correctly");

    nsl_list_free(&list);
}

static void test_copy(void) {
    nsl_Arena arena = {0};
    nsl_List(usize) l1 = {.arena = &arena};

    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nsl_list_push(&l1, i + 1);
    }

    nsl_List(usize) l2 = {.arena = &arena};

    nsl_list_copy(&l1, &l2);
    NSL_ASSERT(l1.len == l2.len && "list was not copied correctly");
    for (usize i = 0; i < l2.len; i++) {
        NSL_ASSERT(l1.items[i] == l2.items[i] && "list was not copied correctly");
    }

    nsl_arena_free(&arena);
}

static void test_pop(void) {
    nsl_List(usize) list = {0};
    const usize n = 10;
    for (usize i = 0; i < n; i++) {
        nsl_list_push(&list, i + 1);
    }

    for (usize i = list.len; 0 < i; i--) {
        NSL_ASSERT(nsl_list_pop(&list) == i && "Poping not correctly");
    }

    NSL_ASSERT(nsl_list_is_empty(&list) == true && "After all that not empty");

    nsl_list_free(&list);
}

static void test_insert(void) {
    nsl_List(usize) list = {0};

    nsl_list_push(&list, 1);
    nsl_list_push(&list, 4);

    nsl_list_insert(&list, 2, 1);
    nsl_list_insert(&list, 3, 2);

    NSL_ASSERT(list.len == 4 && "");
    NSL_ASSERT(list.items[0] == 1);
    NSL_ASSERT(list.items[1] == 2);
    NSL_ASSERT(list.items[2] == 3);
    NSL_ASSERT(list.items[3] == 4);

    nsl_list_free(&list);
}

static void test_remove(void) {
    nsl_List(usize) list = {0};

    nsl_list_push(&list, 1);
    nsl_list_push(&list, 2);
    nsl_list_push(&list, 3);
    nsl_list_push(&list, 4);

    nsl_list_remove(&list, 1);
    nsl_list_remove(&list, 1);

    NSL_ASSERT(list.len == 2 && "");
    NSL_ASSERT(list.items[0] == 1 && "");
    NSL_ASSERT(list.items[1] == 4 && "");

    nsl_list_free(&list);
}

static void test_for_each(void) {
    nsl_List(usize) list = {0};

    usize el[] = {1, 2, 3, 4};
    nsl_list_extend_static(&list, el);

    usize i = 0;
    nsl_list_for_each(usize *, element, &list) {
        NSL_ASSERT(*element == el[i] && "");
        i++;
    }
    NSL_ASSERT(i == list.len && "");

    nsl_list_free(&list);
}

void run_test_list(void);
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
