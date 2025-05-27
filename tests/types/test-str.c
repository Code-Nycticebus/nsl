#include "nc/types/str.h"

#include "nc/types/char.h"

static void test_str_compare(void) {
    nc_Str s = NC_STR("Hello, World");

    NC_ASSERT(nc_str_eq(s, NC_STR("Hello, World")) == true);
    NC_ASSERT(nc_str_eq_ignorecase(s, NC_STR("hElLo, WoRlD")) == true);
    NC_ASSERT(nc_str_startswith(s, NC_STR("Hello, ")) == true);
    NC_ASSERT(nc_str_endswith(s, NC_STR(", World")) == true);

    NC_ASSERT(nc_str_contains(s, NC_STR("Hell")) == true);
    NC_ASSERT(nc_str_contains(s, NC_STR("TEST")) == false);

    NC_ASSERT(nc_str_contains(s, NC_STR("H")) == true);
    NC_ASSERT(nc_str_contains(s, NC_STR("T")) == false);
    NC_ASSERT(nc_str_contains(s, NC_STR("")) == true);

    NC_ASSERT(nc_str_includes(s, 'H') == true);
    NC_ASSERT(nc_str_includes(s, 'e') == true);
    NC_ASSERT(nc_str_includes(s, 'T') == false);

    NC_ASSERT(nc_str_is_empty(s) == false && "string should not be empty");
    NC_ASSERT(nc_str_is_empty(NC_STR("")) == true && "string should be empty");
}

static void test_str_transform(void) {
    nc_Arena arena = {0};
    nc_Str s = NC_STR("Hello, World");
    nc_Str lower = nc_str_lower(s, &arena);
    nc_Str upper = nc_str_upper(s, &arena);

    NC_ASSERT(nc_str_eq(lower, NC_STR("hello, world")));
    NC_ASSERT(nc_str_eq(upper, NC_STR("HELLO, WORLD")));

    nc_arena_free(&arena);
}

static void test_str_copy(void) {
    nc_Arena arena = {0};

    nc_Str str = NC_STR("Hello, World");
    nc_Str full = nc_str_copy(str, &arena);

    NC_ASSERT(nc_str_eq(full, NC_STR("Hello, World")));
    NC_ASSERT(nc_str_getc(full, 0) == 'H');
    NC_ASSERT(nc_str_getc(full, 1) == 'e');
    NC_ASSERT(nc_str_getc(full, 7) == 'W');
    NC_ASSERT(nc_str_getc(full, full.len) == '\0');

    nc_arena_free(&arena);
}

static void test_str_append(void) {
    nc_Arena arena = {0};
    nc_Str append = nc_str_append(NC_STR("filename"), NC_STR(".c"), &arena);
    NC_ASSERT(nc_str_eq(append, NC_STR("filename.c")) && "did not append correctly");

    nc_Str prepend = nc_str_prepend(NC_STR("filename.c"), NC_STR("used_mark__"), &arena);
    NC_ASSERT(nc_str_eq(prepend, NC_STR("used_mark__filename.c")) && "did not prepend correctly");

    nc_Str wrap = nc_str_wrap(NC_STR("command here"), NC_STR("'"), &arena);
    NC_ASSERT(nc_str_eq(wrap, NC_STR("'command here'")) && "did not wrap correctly");

    nc_arena_free(&arena);
}

static void test_str_trim(void) {
    nc_Str dirty = NC_STR("\t  Hello World  \n");
    nc_Str trim_l = nc_str_trim_left(dirty);
    NC_ASSERT(nc_str_eq(trim_l, NC_STR("Hello World  \n")));

    nc_Str trim_r = nc_str_trim_right(dirty);
    NC_ASSERT(nc_str_eq(trim_r, NC_STR("\t  Hello World")));

    nc_Str trim = nc_str_trim(dirty);
    NC_ASSERT(nc_str_eq(trim, NC_STR("Hello World")));
}

static void test_str_chop(void) {
    nc_Str text = NC_STR("Hello\nThis\nis\n\ntext");
    nc_Str h = nc_str_chop_by_delim(&text, '\n');
    NC_ASSERT(nc_str_eq(h, NC_STR("Hello")));

    nc_Str rest = nc_str_chop_by_predicate(&text, nc_char_is_space);
    NC_ASSERT(nc_str_eq(rest, NC_STR("This")));
    rest = nc_str_chop_by_predicate(&text, nc_char_is_space);
    nc_Str test = NC_STR("is");
    NC_ASSERT(nc_str_eq(rest, test));
    rest = nc_str_chop_by_predicate(&text, nc_char_is_space);
    NC_ASSERT(nc_str_eq(rest, NC_STR("text")));
}

static void test_str_try_chop(void) {
    nc_Str text = NC_STR("Hello\nThis\nis\n\ntext");
    nc_Str h = {0};
    bool t1 = nc_str_try_chop_by_delim(&text, '\n', &h);
    NC_ASSERT(t1 == true);
    NC_ASSERT(nc_str_eq(h, NC_STR("Hello")));

    nc_Str rest = {0};
    bool t2 = nc_str_try_chop_by_predicate(&text, nc_char_is_space, &rest);
    NC_ASSERT(t2 == true);
    NC_ASSERT(nc_str_eq(rest, NC_STR("This")));

    t2 = nc_str_try_chop_by_predicate(&text, nc_char_is_space, &rest);
    NC_ASSERT(t2 == true);
    NC_ASSERT(nc_str_eq(rest, NC_STR("is")));

    t2 = nc_str_try_chop_by_predicate(&text, nc_char_is_space, &rest);
    NC_ASSERT(t2 == true);
    NC_ASSERT(nc_str_eq(rest, NC_STR("text")));

    bool t3 = nc_str_try_chop_by_delim(&text, '\n', &rest);
    NC_ASSERT(t3 == false);
}

static void test_str_chop_right(void) {
    nc_Str text = NC_STR("Hello\nThis is  text");
    nc_Str t = nc_str_chop_right_by_predicate(&text, nc_char_is_space);
    nc_Str rest = nc_str_chop_right_by_delim(&text, '\n');
    nc_Str rest2 = nc_str_chop_right_by_delim(&text, '\n');
    nc_Str rest3 = nc_str_chop_right_by_delim(&text, '\n');

    NC_ASSERT(nc_str_eq(t, NC_STR("text")));
    NC_ASSERT(nc_str_eq(rest, NC_STR("This is")));
    NC_ASSERT(nc_str_eq(rest2, NC_STR("Hello")));
    NC_ASSERT(nc_str_eq(rest3, NC_STR("")));
}

static void test_str_number_converting(void) {
    nc_Arena arena = {0};

    const u32 N = 64;
    nc_Str number = nc_str_format(&arena, "%d", N);
    NC_ASSERT(nc_str_eq(number, NC_STR("64")));

    nc_Str n = nc_str_append(number, NC_STR(" bytes"), &arena);
    NC_ASSERT(nc_str_eq(n, NC_STR("64 bytes")));

    NC_ASSERT(nc_str_u64(n) == 64);
    NC_ASSERT(nc_str_chop_u64(&n) == 64);
    NC_ASSERT(n.len == 6);
    NC_ASSERT(nc_str_eq(n, NC_STR(" bytes")));

    nc_Str f = NC_STR("420.69");
    NC_ASSERT(nc_str_f64(f) == 420.69);

    NC_ASSERT(nc_str_chop_f64(&f) == 420.69);
    NC_ASSERT(f.len == 0);
    NC_ASSERT(nc_str_eq(f, NC_STR("")));

    nc_arena_free(&arena);
}

static void test_str_find(void) {
    nc_Str s = NC_STR("Hello, World");
    NC_ASSERT(nc_str_find(s, NC_STR("Hello")) == 0);
    NC_ASSERT(nc_str_find_last(s, NC_STR("Hello")) == 0);
    NC_ASSERT(nc_str_find(s, NC_STR("World")) == 7);
    NC_ASSERT(nc_str_find_last(s, NC_STR("World")) == 7);
    NC_ASSERT(nc_str_find(s, NC_STR("TEST")) == NC_STR_NOT_FOUND);
}

static void test_str_count(void) {
    nc_Str s = NC_STR("Hello, World");
    usize c = nc_str_count(s, NC_STR("o"));
    NC_ASSERT(c == 2);
    c = nc_str_count(s, NC_STR("TEST"));
    NC_ASSERT(c == 0);

    s = NC_STR("--help");
    c = nc_str_count(s, NC_STR("-"));
    NC_ASSERT(c == 2);
}

static void test_str_replace(void) {
    nc_Arena arena = {0};

    nc_Str s = NC_STR("Hello, World");
    nc_Str goodbye = nc_str_replace(s, NC_STR("Hello"), NC_STR("Goodbye"), &arena);
    nc_Str all = nc_str_replace(s, NC_STR("World"), NC_STR("All!"), &arena);

    NC_ASSERT(nc_str_eq(s, NC_STR("Hello, World")));
    NC_ASSERT(nc_str_eq(goodbye, NC_STR("Goodbye, World")));
    NC_ASSERT(nc_str_eq(all, NC_STR("Hello, All!")));

    nc_Str max_test = NC_STR("test test test");
    nc_Str result = nc_str_replace(max_test, NC_STR("test"), NC_STR("result"), &arena);
    NC_ASSERT(nc_str_eq(result, NC_STR("result result result")));

    nc_Str dash = NC_STR("c-language");
    nc_Str res = nc_str_replace(dash, NC_STR("-"), NC_STR(""), &arena);
    NC_ASSERT(nc_str_eq(res, NC_STR("clanguage")));

    nc_Str edge = NC_STR("-language");
    nc_Str res2 = nc_str_replace(edge, NC_STR("-"), NC_STR(""), &arena);
    NC_ASSERT(nc_str_eq(res2, NC_STR("language")));

    nc_arena_free(&arena);
}

static void test_str_substring(void) {
    nc_Str s = NC_STR("Hello, World");
    nc_Str substring = nc_str_substring(s, 0, 4);
    nc_Str invalid1 = nc_str_substring(s, 4, 2);
    nc_Str invalid2 = nc_str_substring(s, s.len, 2);

    NC_ASSERT(nc_str_eq(substring, NC_STR("Hell")));
    NC_ASSERT(nc_str_eq(invalid1, NC_STR("")));
    NC_ASSERT(nc_str_eq(invalid2, NC_STR("")));
}

static void test_str_join(void) {
    nc_Arena arena = {0};
    nc_Str res =
        nc_str_join(NC_STR(", "), 2, (nc_Str[2]){NC_STR("Hello"), NC_STR("World")}, &arena);
    NC_ASSERT(nc_str_eq(res, NC_STR("Hello, World")));

    nc_Str res2 =
        nc_str_join_suffix(NC_STR(", "), 2, (nc_Str[2]){NC_STR("Hello"), NC_STR("World")}, &arena);
    NC_ASSERT(nc_str_eq(res2, NC_STR("Hello, World, ")));

    nc_Str res3 =
        nc_str_join_prefix(NC_STR(", "), 2, (nc_Str[2]){NC_STR("Hello"), NC_STR("World")}, &arena);
    NC_ASSERT(nc_str_eq(res3, NC_STR(", Hello, World")));

    nc_Str res4 = nc_str_join_wrap(NC_STR(" "), NC_STR("'"), 2,
                                   (nc_Str[2]){NC_STR("One"), NC_STR("Two")}, &arena);
    NC_ASSERT(nc_str_eq(res4, NC_STR("'One' 'Two'")));

    nc_Str res5 = nc_str_join(NC_STR(" "), 0, NULL, &arena);
    NC_ASSERT(nc_str_eq(res5, NC_STR("")));

    nc_arena_free(&arena);
}

static void test_str_justify(void) {
    nc_Arena arena = {0};
    const usize width = 10;
    nc_Str center = nc_str_center(NC_STR("Hello"), width, ' ', &arena);
    NC_ASSERT(nc_str_eq(center, NC_STR("  Hello   ")));
    nc_Str left = nc_str_ljust(NC_STR("Hello"), width, ' ', &arena);
    NC_ASSERT(nc_str_eq(left, NC_STR("Hello     ")));
    nc_Str right = nc_str_rjust(NC_STR("Hello"), width, ' ', &arena);
    NC_ASSERT(nc_str_eq(right, NC_STR("     Hello")));
    nc_arena_free(&arena);
}

static void test_str_repeat(void) {
    nc_Arena arena = {0};
    nc_Str tf_fleet = nc_str_repeat(NC_STR("|-#-| "), 4, &arena);
    NC_ASSERT(nc_str_eq(tf_fleet, NC_STR("|-#-| |-#-| |-#-| |-#-| ")));
    nc_arena_free(&arena);
}

static void test_str_reverse(void) {
    nc_Arena arena = {0};
    nc_Str s = nc_str_reverse(NC_STR("Hello, World"), &arena);
    NC_ASSERT(nc_str_eq(s, NC_STR("dlroW ,olleH")));
    nc_arena_free(&arena);
}

static void test_str_hash(void) {
    const struct {
        nc_Str s;
        u64 hash;
    } tests[] = {
        {NC_STR("Hello"), 0x63f0bfacf2c00f6b},
        {NC_STR("This is a very long string"), 0xf276220f1fa71f6b},
        {NC_STR("Another"), 0x90c0e53673f3e9f4},
        {NC_STR("Hello"), 0x63f0bfacf2c00f6b},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        const u64 hash = nc_str_hash(tests[i].s);
        NC_ASSERT(hash == tests[i].hash);
    }
}

static void test_str_format(void) {
    nc_Arena arena = {0};
    nc_Str s = nc_str_format(&arena, "%d %d", 420, 69);
    NC_ASSERT(nc_str_eq(s, NC_STR("420 69")) && "Should format correctly!");
    nc_arena_free(&arena);
}

static void test_str_take(void) {
    nc_Str s = NC_STR("what a wonderful string");
    NC_ASSERT(nc_str_eq(nc_str_take(&s, 2), NC_STR("wh")));
    NC_ASSERT(nc_str_eq(nc_str_take_right(&s, 2), NC_STR("ng")));

    NC_ASSERT(nc_str_eq(nc_str_take(&s, 8), NC_STR("at a won")));
    NC_ASSERT(nc_str_eq(nc_str_take_right(&s, 5), NC_STR(" stri")));

    NC_ASSERT(nc_str_eq(s, NC_STR("derful")));
}

static void test_str_try_take(void) {
    nc_Str s = NC_STR("what a wonderful string");

    nc_Str chunk = {0};
    NC_ASSERT(nc_str_try_take(&s, 7, &chunk) == true);
    NC_ASSERT(nc_str_eq(chunk, NC_STR("what a ")));

    NC_ASSERT(nc_str_try_take_right(&s, 7, &chunk) == true);
    NC_ASSERT(nc_str_eq(chunk, NC_STR(" string")));

    NC_ASSERT(nc_str_eq(s, NC_STR("wonderful")));
    NC_ASSERT(nc_str_try_take_right(&s, 9, &chunk) == true);

    NC_ASSERT(nc_str_try_take_right(&s, 2, &chunk) == false);
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
