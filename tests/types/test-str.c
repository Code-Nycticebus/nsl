#include "nsl.h"

static void test_str_compare(void) {
    nsl_Str s = NSL_STR("Hello, World");

    NSL_ASSERT(nsl_str_eq(s, NSL_STR("Hello, World")) == true);
    NSL_ASSERT(nsl_str_eq_ignorecase(s, NSL_STR("hElLo, WoRlD")) == true);
    NSL_ASSERT(nsl_str_startswith(s, NSL_STR("Hello, ")) == true);
    NSL_ASSERT(nsl_str_endswith(s, NSL_STR(", World")) == true);

    NSL_ASSERT(nsl_str_contains(s, NSL_STR("Hell")) == true);
    NSL_ASSERT(nsl_str_contains(s, NSL_STR("TEST")) == false);

    NSL_ASSERT(nsl_str_contains(s, NSL_STR("H")) == true);
    NSL_ASSERT(nsl_str_contains(s, NSL_STR("T")) == false);
    NSL_ASSERT(nsl_str_contains(s, NSL_STR("")) == true);

    NSL_ASSERT(nsl_str_includes(s, 'H') == true);
    NSL_ASSERT(nsl_str_includes(s, 'e') == true);
    NSL_ASSERT(nsl_str_includes(s, 'T') == false);

    NSL_ASSERT(nsl_str_is_empty(s) == false && "string should not be empty");
    NSL_ASSERT(nsl_str_is_empty(NSL_STR("")) == true && "string should be empty");
}

static void test_str_transform(void) {
    nsl_Arena arena = {0};
    nsl_Str s = NSL_STR("Hello, World");
    nsl_Str lower = nsl_str_lower(s, &arena);
    nsl_Str upper = nsl_str_upper(s, &arena);

    NSL_ASSERT(nsl_str_eq(lower, NSL_STR("hello, world")));
    NSL_ASSERT(nsl_str_eq(upper, NSL_STR("HELLO, WORLD")));

    nsl_arena_free(&arena);
}

static void test_str_copy(void) {
    nsl_Arena arena = {0};

    nsl_Str str = NSL_STR("Hello, World");
    nsl_Str full = nsl_str_copy(str, &arena);

    NSL_ASSERT(nsl_str_eq(full, NSL_STR("Hello, World")));
    NSL_ASSERT(nsl_str_getc(full, 0) == 'H');
    NSL_ASSERT(nsl_str_getc(full, 1) == 'e');
    NSL_ASSERT(nsl_str_getc(full, 7) == 'W');
    NSL_ASSERT(nsl_str_getc(full, full.len) == '\0');

    nsl_arena_free(&arena);
}

static void test_str_append(void) {
    nsl_Arena arena = {0};
    nsl_Str append = nsl_str_append(NSL_STR("filename"), NSL_STR(".c"), &arena);
    NSL_ASSERT(nsl_str_eq(append, NSL_STR("filename.c")) && "did not append correctly");

    nsl_Str prepend = nsl_str_prepend(NSL_STR("filename.c"), NSL_STR("used_mark__"), &arena);
    NSL_ASSERT(nsl_str_eq(prepend, NSL_STR("used_mark__filename.c")) && "did not prepend correctly");

    nsl_Str wrap = nsl_str_wrap(NSL_STR("command here"), NSL_STR("'"), &arena);
    NSL_ASSERT(nsl_str_eq(wrap, NSL_STR("'command here'")) && "did not wrap correctly");

    nsl_arena_free(&arena);
}

static void test_str_trim(void) {
    nsl_Str dirty = NSL_STR("\t  Hello World  \n");
    nsl_Str trim_l = nsl_str_trim_left(dirty);
    NSL_ASSERT(nsl_str_eq(trim_l, NSL_STR("Hello World  \n")));

    nsl_Str trim_r = nsl_str_trim_right(dirty);
    NSL_ASSERT(nsl_str_eq(trim_r, NSL_STR("\t  Hello World")));

    nsl_Str trim = nsl_str_trim(dirty);
    NSL_ASSERT(nsl_str_eq(trim, NSL_STR("Hello World")));
}

static void test_str_chop(void) {
    nsl_Str text = NSL_STR("Hello\nThis\nis\n\ntext");
    nsl_Str h = nsl_str_chop_by_delim(&text, '\n');
    NSL_ASSERT(nsl_str_eq(h, NSL_STR("Hello")));

    nsl_Str rest = nsl_str_chop_by_predicate(&text, nsl_char_is_space);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("This")));
    rest = nsl_str_chop_by_predicate(&text, nsl_char_is_space);
    nsl_Str test = NSL_STR("is");

    NSL_ASSERT(nsl_str_eq(rest, test));

    rest = nsl_str_chop_by_predicate(&text, nsl_char_is_space);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("")));

    rest = nsl_str_chop_by_predicate(&text, nsl_char_is_space);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("text")));
}

static void test_str_try_chop(void) {
    nsl_Str text = NSL_STR("Hello\nThis\nis\n\ntext");
    nsl_Str h = {0};
    bool t1 = nsl_str_try_chop_by_delim(&text, '\n', &h);
    NSL_ASSERT(t1 == true);
    NSL_ASSERT(nsl_str_eq(h, NSL_STR("Hello")));

    nsl_Str rest = {0};
    bool t2 = nsl_str_try_chop_by_delim(&text, '\n', &rest);
    NSL_ASSERT(t2 == true);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("This")));

    t2 = nsl_str_try_chop_by_predicate(&text, nsl_char_is_space, &rest);
    NSL_ASSERT(t2 == true);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("is")));

    t2 = nsl_str_try_chop_by_predicate(&text, nsl_char_is_space, &rest);
    NSL_ASSERT(t2 == true);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("")));

    t2 = nsl_str_try_chop_by_predicate(&text, nsl_char_is_space, &rest);
    NSL_ASSERT(t2 == true);
    NSL_ASSERT(nsl_str_eq(rest, NSL_STR("text")));

    bool t3 = nsl_str_try_chop_by_delim(&text, '\n', &rest);
    NSL_ASSERT(t3 == false);
}

static void test_str_chop_right(void) {
    nsl_Str text = NSL_STR("Hello\n\nThis is  text");
    nsl_Str rest[] = {
        nsl_str_chop_right_by_delim(&text, '\n'),
        nsl_str_chop_right_by_delim(&text, '\n'),
        nsl_str_chop_right_by_delim(&text, '\n'),
        nsl_str_chop_right_by_delim(&text, '\n'),
        nsl_str_chop_right_by_delim(&text, '\n'),
    };

    NSL_ASSERT(nsl_str_eq(rest[0], NSL_STR("This is  text")));
    NSL_ASSERT(nsl_str_eq(rest[1], NSL_STR("")));
    NSL_ASSERT(nsl_str_eq(rest[2], NSL_STR("Hello")));
    NSL_ASSERT(nsl_str_eq(rest[3], NSL_STR("")));
}

static void test_str_number_converting(void) {
    nsl_Arena arena = {0};

    const u32 N = 64;
    nsl_Str number = nsl_str_format(&arena, "%d", N);
    NSL_ASSERT(nsl_str_eq(number, NSL_STR("64")));

    nsl_Str n = nsl_str_append(number, NSL_STR(" bytes"), &arena);
    NSL_ASSERT(nsl_str_eq(n, NSL_STR("64 bytes")));

    u64 u = 0;
    NSL_ASSERT(nsl_str_u64(n, &u) == NSL_ERROR_PARSE);
    NSL_ASSERT(nsl_str_chop_u64(&n, &u) == NSL_NO_ERROR);
    NSL_ASSERT(u == 64);
    NSL_ASSERT(n.len == 6);
    NSL_ASSERT(nsl_str_eq(n, NSL_STR(" bytes")));

    nsl_Str e = NSL_STR("420");
    NSL_ASSERT(nsl_str_u64(e, &u) == NSL_NO_ERROR);
    NSL_ASSERT(u == 420);

    nsl_Str i = NSL_STR("-420");
    i64 ii = 0;
    NSL_ASSERT(nsl_str_i64(i, &ii) == NSL_NO_ERROR);
    NSL_ASSERT(ii == -420);
    NSL_ASSERT(nsl_str_chop_i64(&i, &ii) == NSL_NO_ERROR);
    NSL_ASSERT(ii == -420);
    NSL_ASSERT(i.len == 0);

    nsl_Str f = NSL_STR("420.69");

    f64 ff;
    NSL_ASSERT(nsl_str_f64(f, &ff) == NSL_NO_ERROR);
    NSL_ASSERT(ff == 420.69);

    NSL_ASSERT(nsl_str_chop_f64(&f, &ff) == NSL_NO_ERROR);
    NSL_ASSERT(ff == 420.69);
    NSL_ASSERT(f.len == 0);

    nsl_arena_free(&arena);
}

static void test_str_find(void) {
    nsl_Str s = NSL_STR("Hello, World");
    NSL_ASSERT(nsl_str_find(s, NSL_STR("Hello")) == 0);
    NSL_ASSERT(nsl_str_find_last(s, NSL_STR("Hello")) == 0);
    NSL_ASSERT(nsl_str_find(s, NSL_STR("World")) == 7);
    NSL_ASSERT(nsl_str_find_last(s, NSL_STR("World")) == 7);
    NSL_ASSERT(nsl_str_find(s, NSL_STR("TEST")) == NSL_STR_NOT_FOUND);
}

static void test_str_count(void) {
    nsl_Str s = NSL_STR("Hello, World");
    usize c = nsl_str_count(s, NSL_STR("o"));
    NSL_ASSERT(c == 2);
    c = nsl_str_count(s, NSL_STR("TEST"));
    NSL_ASSERT(c == 0);

    s = NSL_STR("--help");
    c = nsl_str_count(s, NSL_STR("-"));
    NSL_ASSERT(c == 2);
}

static void test_str_replace(void) {
    nsl_Arena arena = {0};

    nsl_Str s = NSL_STR("Hello, World");
    nsl_Str goodbye = nsl_str_replace(s, NSL_STR("Hello"), NSL_STR("Goodbye"), &arena);
    nsl_Str all = nsl_str_replace(s, NSL_STR("World"), NSL_STR("All!"), &arena);

    NSL_ASSERT(nsl_str_eq(s, NSL_STR("Hello, World")));
    NSL_ASSERT(nsl_str_eq(goodbye, NSL_STR("Goodbye, World")));
    NSL_ASSERT(nsl_str_eq(all, NSL_STR("Hello, All!")));

    nsl_Str max_test = NSL_STR("test test test");
    nsl_Str result = nsl_str_replace(max_test, NSL_STR("test"), NSL_STR("result"), &arena);
    NSL_ASSERT(nsl_str_eq(result, NSL_STR("result result result")));

    nsl_Str dash = NSL_STR("c-language");
    nsl_Str res = nsl_str_replace(dash, NSL_STR("-"), NSL_STR(""), &arena);
    NSL_ASSERT(nsl_str_eq(res, NSL_STR("clanguage")));

    nsl_Str edge = NSL_STR("-language");
    nsl_Str res2 = nsl_str_replace(edge, NSL_STR("-"), NSL_STR(""), &arena);
    NSL_ASSERT(nsl_str_eq(res2, NSL_STR("language")));

    nsl_arena_free(&arena);
}

static void test_str_substring(void) {
    nsl_Str s = NSL_STR("Hello, World");
    nsl_Str substring = nsl_str_substring(s, 0, 4);
    nsl_Str invalid1 = nsl_str_substring(s, 4, 2);
    nsl_Str invalid2 = nsl_str_substring(s, s.len, 2);

    NSL_ASSERT(nsl_str_eq(substring, NSL_STR("Hell")));
    NSL_ASSERT(nsl_str_eq(invalid1, NSL_STR("")));
    NSL_ASSERT(nsl_str_eq(invalid2, NSL_STR("")));
}

static void test_str_join(void) {
    nsl_Arena arena = {0};
    nsl_Str res =
        nsl_str_join(NSL_STR(", "), 2, (nsl_Str[2]){NSL_STR("Hello"), NSL_STR("World")}, &arena);
    NSL_ASSERT(nsl_str_eq(res, NSL_STR("Hello, World")));

    nsl_Str res2 =
        nsl_str_join_suffix(NSL_STR(", "), 2, (nsl_Str[2]){NSL_STR("Hello"), NSL_STR("World")}, &arena);
    NSL_ASSERT(nsl_str_eq(res2, NSL_STR("Hello, World, ")));

    nsl_Str res3 =
        nsl_str_join_prefix(NSL_STR(", "), 2, (nsl_Str[2]){NSL_STR("Hello"), NSL_STR("World")}, &arena);
    NSL_ASSERT(nsl_str_eq(res3, NSL_STR(", Hello, World")));

    nsl_Str res4 = nsl_str_join_wrap(NSL_STR(" "), NSL_STR("'"), 2,
                                   (nsl_Str[2]){NSL_STR("One"), NSL_STR("Two")}, &arena);
    NSL_ASSERT(nsl_str_eq(res4, NSL_STR("'One' 'Two'")));

    nsl_Str res5 = nsl_str_join(NSL_STR(" "), 0, NULL, &arena);
    NSL_ASSERT(nsl_str_eq(res5, NSL_STR("")));

    nsl_arena_free(&arena);
}

static void test_str_justify(void) {
    nsl_Arena arena = {0};
    const usize width = 10;
    nsl_Str center = nsl_str_center(NSL_STR("Hello"), width, ' ', &arena);
    NSL_ASSERT(nsl_str_eq(center, NSL_STR("  Hello   ")));
    nsl_Str left = nsl_str_ljust(NSL_STR("Hello"), width, ' ', &arena);
    NSL_ASSERT(nsl_str_eq(left, NSL_STR("Hello     ")));
    nsl_Str right = nsl_str_rjust(NSL_STR("Hello"), width, ' ', &arena);
    NSL_ASSERT(nsl_str_eq(right, NSL_STR("     Hello")));
    nsl_arena_free(&arena);
}

static void test_str_repeat(void) {
    nsl_Arena arena = {0};
    nsl_Str tf_fleet = nsl_str_repeat(NSL_STR("|-#-| "), 4, &arena);
    NSL_ASSERT(nsl_str_eq(tf_fleet, NSL_STR("|-#-| |-#-| |-#-| |-#-| ")));
    nsl_arena_free(&arena);
}

static void test_str_reverse(void) {
    nsl_Arena arena = {0};
    nsl_Str s = nsl_str_reverse(NSL_STR("Hello, World"), &arena);
    NSL_ASSERT(nsl_str_eq(s, NSL_STR("dlroW ,olleH")));
    nsl_arena_free(&arena);
}

static void test_str_hash(void) {
    const struct {
        nsl_Str s;
        u64 hash;
    } tests[] = {
        {NSL_STR("Hello"), 0x63f0bfacf2c00f6b},
        {NSL_STR("This is a very long string"), 0xf276220f1fa71f6b},
        {NSL_STR("Another"), 0x90c0e53673f3e9f4},
        {NSL_STR("Hello"), 0x63f0bfacf2c00f6b},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        const u64 hash = nsl_str_hash(tests[i].s);
        NSL_ASSERT(hash == tests[i].hash);
    }
}

static void test_str_format(void) {
    nsl_Arena arena = {0};
    nsl_Str s = nsl_str_format(&arena, "%d %d", 420, 69);
    NSL_ASSERT(nsl_str_eq(s, NSL_STR("420 69")) && "Should format correctly!");
    nsl_arena_free(&arena);
}

static void test_str_take(void) {
    nsl_Str s = NSL_STR("what a wonderful string");
    NSL_ASSERT(nsl_str_eq(nsl_str_take(&s, 2), NSL_STR("wh")));
    NSL_ASSERT(nsl_str_eq(nsl_str_take_right(&s, 2), NSL_STR("ng")));

    NSL_ASSERT(nsl_str_eq(nsl_str_take(&s, 8), NSL_STR("at a won")));
    NSL_ASSERT(nsl_str_eq(nsl_str_take_right(&s, 5), NSL_STR(" stri")));

    NSL_ASSERT(nsl_str_eq(s, NSL_STR("derful")));
}

static void test_str_try_take(void) {
    nsl_Str s = NSL_STR("what a wonderful string");

    nsl_Str chunk = {0};
    NSL_ASSERT(nsl_str_try_take(&s, 7, &chunk) == true);
    NSL_ASSERT(nsl_str_eq(chunk, NSL_STR("what a ")));

    NSL_ASSERT(nsl_str_try_take_right(&s, 7, &chunk) == true);
    NSL_ASSERT(nsl_str_eq(chunk, NSL_STR(" string")));

    NSL_ASSERT(nsl_str_eq(s, NSL_STR("wonderful")));
    NSL_ASSERT(nsl_str_try_take_right(&s, 9, &chunk) == true);

    NSL_ASSERT(nsl_str_try_take_right(&s, 2, &chunk) == false);
}

void run_test_str(void);
void run_test_str(void) {
    test_str_compare();
    test_str_transform();
    test_str_copy();
    test_str_append();
    test_str_trim();
    test_str_chop();
    test_str_try_chop();
    test_str_chop_right();
    test_str_number_converting();
    test_str_find();
    test_str_count();
    test_str_replace();
    test_str_substring();
    test_str_join();
    test_str_justify();
    test_str_repeat();
    test_str_reverse();
    test_str_hash();
    test_str_format();
    test_str_take();
    test_str_try_take();
}
