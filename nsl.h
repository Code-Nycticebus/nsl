/*
# Nycticebus Standard Library
nsl is a single-header standard library for C, heavily inspired by [nob.h](https://github.com/tsoding/nob.h).

Its a complete rewrite of [cebus](https://github.com/Code-Nycticebus/cebus).

## Example

```c
// build.c
#define NSL_IMPLEMENTATION
#include "nsl.h"

int main(int argc, const char** argv) {
    nsl_Arena arena {0};

    const char* name = "nsl";
    nsl_Str s = nsl_str_format(&arena, "Hello, %s!", name);
    printf(NSL_STR_FMT"\n", NSL_STR_ARG(s));

    nsl_arena_free(&arena);
    return 0;
}
```

## TODO
Here's whats left to do:
- [ ] windows implementation for `os.h`
- [ ] more cmd functions like: async, run and reset, CMD macro usw
- [ ] scratch arena in some way
- [ ] dll loading

*/

#ifndef _NSL_H_
#define _NSL_H_

// include/nsl/defines.h

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef NSL_NO_INT_TYPEDEFS
    #include <stdint.h>
    #include <stddef.h>
    typedef uint8_t   u8;
    typedef int8_t    i8;
    typedef uint16_t  u16;
    typedef int16_t   i16;
    typedef uint32_t  u32;
    typedef int32_t   i32;
    typedef uint64_t  u64;
    typedef int64_t   i64;
    typedef size_t    usize;
    typedef ptrdiff_t isize;
    typedef float     f32;
    typedef double    f64;
#endif

#ifndef NSL_ASSERT
#    include <assert.h>
#    define NSL_ASSERT assert
#endif

#define NSL_BOOL_FMT "%s"
#define NSL_BOOL_ARG(b) (b ? "true" : "false")

#define NSL_ARRAY_LEN(...) (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))

#define NSL_STR(str) ((nsl_Str){.len = sizeof(str) - 1, .data = (str)})
#define NSL_STR_STATIC(str) { .len = sizeof(str) - 1, .data = (str) }

#define NSL_STR_FMT "%.*s"
#define NSL_STR_REPR "'%.*s'"
#define NSL_STR_ARG(str) (i32)(str).len, (str).data

typedef struct {
    usize len;
    const char *data;
} nsl_Str;

#define NSL_BYTES(...)                                                                             \
    (nsl_Bytes) {                                                                                  \
        .size = sizeof((const u8[]){__VA_ARGS__}), .data = (const u8[]){__VA_ARGS__},              \
    }

#define NSL_BYTES_STR(s)                                                                           \
    (nsl_Bytes) {                                                                                  \
        .size = sizeof(s) - 1, .data = (const u8 *)(s),                                            \
    }

typedef struct {
    usize size;
    const u8 *data;
} nsl_Bytes;

typedef nsl_Str nsl_Path;
#define NSL_PATH(cstr) ((nsl_Path){.len = sizeof(cstr) - 1, .data = (cstr)})

typedef struct nsl_Chunk nsl_Chunk;

typedef struct {
    nsl_Chunk *begin, *end;
} nsl_Arena;

typedef struct {
    i64 code;            // 0 = no error
    const char *file;    // __FILE__ where error occured
    i32 line;            // __LINE__ where error occured
    const char *func;    // __func__ where error occured
    const char *message; // human-readable message
} nsl_Error;

#define nsl_List(T)                                                                                \
    struct {                                                                                       \
        usize cap;                                                                                 \
        usize len;                                                                                 \
        nsl_Arena *arena;                                                                          \
        T *items;                                                                                  \
    }

typedef nsl_List(char) nsl_StrBuilder;
typedef nsl_List(u8) nsl_ByteBuffer;

#define NSL_UNUSED(v) (void)(v)
#define NSL_PASS ((void)(0))

#define NSL_DEFER(v) do { result = (v); goto defer; } while(0)

#define NSL_PANIC(msg)                                                                             \
    do {                                                                                           \
        fprintf(stderr, "Panic: %s:%d: %s\n", __FILE__, __LINE__, msg);                            \
        abort();                                                                                   \
    } while (0)

#define NSL_UNREACHABLE(msg)                                                                       \
    do {                                                                                           \
        fprintf(stderr, "Unreachable: %s:%d: %s\n", __FILE__, __LINE__, msg);                      \
        abort();                                                                                   \
    } while (0)

#define NSL_TODO(msg)                                                                              \
    do {                                                                                           \
        fprintf(stderr, "TODO: %s:%d: %s\n", __FILE__, __LINE__, msg);                             \
        abort();                                                                                   \
    } while (0)

#define NSL_NOT_IMPLEMENTED(msg)                                                                   \
    do {                                                                                           \
        fprintf(stderr, "Not Implemented: %s:%d: %s\n", __FILE__, __LINE__, msg);                  \
        abort();                                                                                   \
    } while (0)


#if defined(__clang__)
#    define NSL_COMPILER_CLANG
#    define NSL_COMPILER_NAME "clang"
#elif defined(__GNUC__) && !defined(__clang__)
#    define NSL_COMPILER_GCC
#    define NSL_COMPILER_NAME "gcc"
#elif defined(__TINYC__)
#    define NSL_COMPILER_TCC
#    define NSL_COMPILER_NAME "tcc"
#elif defined(_MSC_VER)
#    define NSL_COMPILER_MSVC
#    define NSL_COMPILER_NAME "cl"
#elif defined(__MINGW32__)
#    define NSL_COMPILER_MINGW32
#    define NSL_COMPILER_NAME "mingw32"
#elif defined(__MINGW64__)
#    define NSL_COMPILER_MINGW64
#    define NSL_COMPILER_NAME "mingw64"
#else
#    define NSL_COMPILER_UNKNOWN
#    define NSL_COMPILER_NAME "unknown"
#endif

#if defined(NSL_COMPILER_GCC) || defined(NSL_COMPILER_CLANG)
#    define NSL_EXPORT        __attribute__((used))
#    define NSL_NORETURN      __attribute__((noreturn))
#    define NSL_PURE_FN       __attribute__((pure)) __attribute__((warn_unused_result))
#    define NSL_CONST_FN      __attribute__((const)) __attribute__((warn_unused_result))
#    define NSL_LIKELY(exp)   __builtin_expect(!!(exp), 1)
#    define NSL_UNLIKELY(exp) __builtin_expect(!!(exp), 0)
#    define NSL_FMT(fmt_idx)  __attribute__((format(printf, fmt_idx, fmt_idx + 1)))
#elif defined(NSL_COMPILER_MSVC)
#    include <sal.h>
#    define NSL_EXPORT        __declspec(dllexport)
#    define NSL_NORETURN      __declspec(noreturn)
#    define NSL_PURE_FN       _Check_return_
#    define NSL_CONST_FN      _Check_return_
#    define NSL_LIKELY(exp)   (exp)
#    define NSL_UNLIKELY(exp) (exp)
#    define NSL_FMT(fmt_idx)
#else
#    define NSL_EXPORT
#    define NSL_NORETURN
#    define NSL_UNUSED
#    define NSL_PURE_FN
#    define NSL_CONST_FN
#    define NSL_LIKELY(exp)   (exp)
#    define NSL_UNLIKELY(exp) (exp)
#    define NSL_FMT(fmt_idx)
#endif

#ifdef NSL_STATIC
#    define NSL_API static
#elif defined(_WIN32)
#    if defined(NSL_DLL_BUILD)
#        define NSL_API __declspec(dllexport)
#    elif defined(NSL_DLL_LOAD)
#        define NSL_API __declspec(dllimport)
#    endif
#else
#    define NSL_API __attribute__((visibility("default")))
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
#    define NSL_ENDIAN_LITTLE __ORDER_LITTLE_ENDIAN__
#    define NSL_ENDIAN_BIG    __ORDER_BIG_ENDIAN__
#    define NSL_BYTE_ORDER    __BYTE_ORDER__
#elif defined(_MSC_VER)
#    define NSL_ENDIAN_LITTLE 1234
#    define NSL_ENDIAN_BIG    4321
#    define NSL_BYTE_ORDER    NSL_ENDIAN_LITTLE
#else
#    error "No Byte Order detected!"
#endif

// include/nsl/types/str.h


///////////////////////////////////////////////////////////////////////////////

#define NSL_STR_NOT_FOUND SIZE_MAX

#define NSL_STR_LETTERS NSL_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define NSL_STR_UPPERCASE NSL_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define NSL_STR_LOWERCASE NSL_STR("abcdefghijklmnopqrstuvwxyz")
#define NSL_STR_DIGITS NSL_STR("0123456789")
#define NSL_STR_HEXDIGITS NSL_STR("0123456789abcdefABCDEF")
#define NSL_STR_PUNCTUATION NSL_STR("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~")
#define NSL_STR_WHITESPACE NSL_STR(" \t\n\r\x0b\x0c")

NSL_API nsl_Str nsl_str_from_parts(usize size, const char *cstr);
NSL_API nsl_Str nsl_str_from_bytes(nsl_Bytes bytes);
NSL_API nsl_Bytes nsl_str_to_bytes(nsl_Str s);
NSL_API nsl_Str nsl_str_from_cstr(const char *cstr);
NSL_API const char* nsl_str_to_cstr(nsl_Str str, nsl_Arena* arena);
NSL_API NSL_FMT(2) nsl_Str nsl_str_format(nsl_Arena *arena, const char *fmt, ...);

NSL_API nsl_Str nsl_str_copy(nsl_Str s, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_append(nsl_Str s1, nsl_Str suffix, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_prepend(nsl_Str s1, nsl_Str prefix, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_wrap(nsl_Str s, nsl_Str wrap, nsl_Arena *arena);

// Inserts sep in between elements
NSL_API nsl_Str nsl_str_join(nsl_Str sep, usize count, nsl_Str *s, nsl_Arena *arena);
// Appends suffix to every element, even the last one
NSL_API nsl_Str nsl_str_join_suffix(nsl_Str suffix, usize count, nsl_Str *s, nsl_Arena *arena);
// Prepends prefix to every element, even the last one
NSL_API nsl_Str nsl_str_join_prefix(nsl_Str prefix, usize count, nsl_Str *s, nsl_Arena *arena);

NSL_API nsl_Str nsl_str_join_wrap(nsl_Str sep, nsl_Str wrap, usize count, nsl_Str *s, nsl_Arena *arena);

NSL_API nsl_Str nsl_str_upper(nsl_Str s, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_lower(nsl_Str s, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_replace(nsl_Str s, nsl_Str old, nsl_Str new, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_center(nsl_Str s, usize width, char fillchar, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_ljust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_rjust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_repeat(nsl_Str s, usize count, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_reverse(nsl_Str s, nsl_Arena *arena);

NSL_API bool nsl_str_eq(nsl_Str s1, nsl_Str s2);
NSL_API bool nsl_str_eq_ignorecase(nsl_Str s1, nsl_Str s2);
NSL_API bool nsl_str_startswith(nsl_Str s1, nsl_Str prefix);
NSL_API bool nsl_str_endswith(nsl_Str s1, nsl_Str suffix);
NSL_API bool nsl_str_endswith_predicate(nsl_Str s1, bool (*predicate)(char));

NSL_API bool nsl_str_contains(nsl_Str haystack, nsl_Str needle);
NSL_API bool nsl_str_includes(nsl_Str haystack, char needle);
NSL_API bool nsl_str_is_empty(nsl_Str s);

NSL_API nsl_Str nsl_str_trim_left(nsl_Str s);
NSL_API nsl_Str nsl_str_trim_left_by_delim(nsl_Str s, char delim);
NSL_API nsl_Str nsl_str_trim_left_by_predicate(nsl_Str s, bool (*predicate)(char));

NSL_API nsl_Str nsl_str_trim_right(nsl_Str s);
NSL_API nsl_Str nsl_str_trim_right_by_delim(nsl_Str s, char delim);
NSL_API nsl_Str nsl_str_trim_right_by_predicate(nsl_Str s, bool (*predicate)(char));

NSL_API nsl_Str nsl_str_trim(nsl_Str s);
NSL_API nsl_Str nsl_str_trim_by_delim(nsl_Str s, char delim);
NSL_API nsl_Str nsl_str_trim_by_predicate(nsl_Str s, bool (*predicate)(char));

NSL_API bool nsl_str_try_chop_by_delim(nsl_Str *s, char delim, nsl_Str *chunk);
NSL_API nsl_Str nsl_str_chop_by_delim(nsl_Str *s, char delim);
NSL_API bool nsl_str_try_chop_by_predicate(nsl_Str *s, bool (*predicate)(char), nsl_Str *chunk);
NSL_API nsl_Str nsl_str_chop_by_predicate(nsl_Str *s, bool (*predicate)(char));
NSL_API nsl_Str nsl_str_chop_right_by_delim(nsl_Str *s, char delim);
NSL_API nsl_Str nsl_str_chop_right_by_predicate(nsl_Str *s, bool (*predicate)(char));
NSL_API nsl_Str nsl_str_take(nsl_Str *s, usize count);
NSL_API bool nsl_str_try_take(nsl_Str *s, usize count, nsl_Str *chunk);
NSL_API nsl_Str nsl_str_take_right(nsl_Str *s, usize count);
NSL_API bool nsl_str_try_take_right(nsl_Str *s, usize count, nsl_Str *chunk);

NSL_API nsl_Str nsl_str_substring(nsl_Str s, usize start, usize end);

NSL_API u64 nsl_str_u64(nsl_Str s);
NSL_API u64 nsl_str_chop_u64(nsl_Str *s);

NSL_API i64 nsl_str_i64(nsl_Str s);
NSL_API i64 nsl_str_chop_i64(nsl_Str *s);

NSL_API f64 nsl_str_f64(nsl_Str s);
NSL_API f64 nsl_str_chop_f64(nsl_Str *s);

// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NSL_API usize nsl_str_find(nsl_Str haystack, nsl_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NSL_API usize nsl_str_find_by_predicate(nsl_Str haystack, bool (*predicate)(char));
// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NSL_API usize nsl_str_find_last(nsl_Str haystack, nsl_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NSL_API usize nsl_str_find_last_by_predicate(nsl_Str haystack, bool (*predicate)(char));

NSL_API usize nsl_str_count(nsl_Str haystack, nsl_Str needle);
// Returns '\0' if the index is out of bounds.
NSL_API char nsl_str_getc(nsl_Str s, usize idx);

// Basic FNV hash.
NSL_API u64 nsl_str_hash(nsl_Str s);

// include/nsl/types/path.h


NSL_API nsl_Path nsl_path_join(usize len, const nsl_Path* parts, nsl_Arena* arena);
NSL_API nsl_Path nsl_path_normalize(nsl_Path path, nsl_Arena* arena);

NSL_API bool nsl_path_eq(nsl_Path p1, nsl_Path p2);
NSL_API bool nsl_path_is_absolute(nsl_Path path);
NSL_API bool nsl_path_is_root(nsl_Path path);

NSL_API nsl_Str nsl_path_name(nsl_Path path);
NSL_API nsl_Str nsl_path_suffix(nsl_Path path);
NSL_API nsl_Str nsl_path_stem(nsl_Path path);
NSL_API nsl_Path nsl_path_parent(nsl_Path path);

// include/nsl/types/int.h


#define INTEGER_DECL(T)                                                                            \
    NSL_API NSL_CONST_FN T nsl_##T##_reverse_bits(T value);                                           \
    NSL_API NSL_CONST_FN usize nsl_##T##_leading_ones(T value);                                       \
    NSL_API NSL_CONST_FN usize nsl_##T##_trailing_ones(T value);                                      \
    NSL_API NSL_CONST_FN usize nsl_##T##_leading_zeros(T value);                                      \
    NSL_API NSL_CONST_FN usize nsl_##T##_trailing_zeros(T value);                                     \
    NSL_API NSL_CONST_FN usize nsl_##T##_count_zeros(T value);                                        \
    NSL_API NSL_CONST_FN usize nsl_##T##_count_ones(T value);                                         \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_swap_bytes(T value);                                             \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_to_be(T value);                                                  \
    NSL_API NSL_CONST_FN T nsl_##T##_from_be(T value);                                                \
    NSL_API T nsl_##T##_from_be_bytes(nsl_Bytes bytes);                                               \
    NSL_API nsl_Bytes nsl_##T##_to_be_bytes(T value, nsl_Arena *arena);                                \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_to_le(T value);                                                  \
    NSL_API NSL_CONST_FN T nsl_##T##_from_le(T value);                                                \
    NSL_API T nsl_##T##_from_le_bytes(nsl_Bytes bytes);                                               \
    NSL_API nsl_Bytes nsl_##T##_to_le_bytes(T value, nsl_Arena *arena);                                \
                                                                                                   \
    NSL_API T nsl_##T##_from_ne_bytes(nsl_Bytes bytes);                                               \
    NSL_API nsl_Bytes nsl_##T##_to_ne_bytes(T value, nsl_Arena *arena);                                \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_max(T a, T b);                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_min(T a, T b);                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_clamp(T min, T max, T value);                                    \
                                                                                                   \
    NSL_API NSL_CONST_FN u64 nsl_##T##_hash(T value);                                                 \
    NSL_API void nsl_##T##_swap(T *v1, T *v2);                                                       \
                                                                                                   \
    NSL_API NSL_CONST_FN T nsl_##T##_next_pow2(T n);

INTEGER_DECL(u8)
INTEGER_DECL(i8)
INTEGER_DECL(u16)
INTEGER_DECL(i16)
INTEGER_DECL(u32)
INTEGER_DECL(i32)
INTEGER_DECL(u64)
INTEGER_DECL(i64)
INTEGER_DECL(usize)

#undef INTEGER_DECL

// include/nsl/types/char.h


NSL_API NSL_CONST_FN bool nsl_char_is_alnum(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_alpha(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_lower(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_upper(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_space(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_cntrl(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_print(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_graph(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_blank(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_punct(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_digit(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_xdigit(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_path_delimiter(char c);

NSL_API NSL_CONST_FN char nsl_char_to_lower(char c);
NSL_API NSL_CONST_FN char nsl_char_to_upper(char c);

NSL_API NSL_CONST_FN u8 nsl_char_to_u8(char c);
NSL_API NSL_CONST_FN u8 nsl_char_hex_to_u8(char c);

NSL_API NSL_CONST_FN char nsl_char_from_u8(u8 d);
NSL_API NSL_CONST_FN char nsl_char_hex_from_u8(u8 d);
NSL_API NSL_CONST_FN char nsl_char_HEX_from_u8(u8 d);

// include/nsl/types/byte.h


NSL_API nsl_Bytes nsl_bytes_from_parts(usize size, const void *data);

NSL_API nsl_Bytes nsl_bytes_copy(nsl_Bytes bytes, nsl_Arena *arena);

NSL_API nsl_Bytes nsl_bytes_slice(nsl_Bytes bytes, usize idx1, size_t idx2);
NSL_API nsl_Bytes nsl_bytes_take(nsl_Bytes *bytes, usize count);

NSL_API bool nsl_bytes_eq(nsl_Bytes b1, nsl_Bytes b2);
NSL_API u64 nsl_bytes_hash(nsl_Bytes bytes);

NSL_API nsl_Str nsl_bytes_to_hex(nsl_Bytes bytes, nsl_Arena *arena);
NSL_API nsl_Bytes nsl_bytes_from_hex(nsl_Str s, nsl_Arena *arena);

// include/nsl/structs/set.h


typedef struct {
    usize len;
    usize _cap;
    usize _del;
    nsl_Arena *arena;
    u64 *items;
} nsl_Set;

#define NSL_SET_DEFAULT_SIZE 8
#define NSL_SET_DELETED ((u64)0xdeaddeaddeaddead)

NSL_API void nsl_set_init(nsl_Set* set, nsl_Arena *arena);

NSL_API void nsl_set_resize(nsl_Set *set, usize size);
NSL_API void nsl_set_reserve(nsl_Set *set, usize size);

NSL_API bool nsl_set_remove(nsl_Set* set, u64 hash);

NSL_API bool nsl_set_add(nsl_Set *set, u64 hash);
NSL_API bool nsl_set_has(const nsl_Set *set, u64 hash);

NSL_API void nsl_set_update(nsl_Set* set, const nsl_Set* other);
NSL_API void nsl_set_extend(nsl_Set* set, usize count, const u64* hashes);

NSL_API bool nsl_set_eq(const nsl_Set* set, const nsl_Set* other);
NSL_API bool nsl_set_subset(const nsl_Set* set, const nsl_Set* other);
NSL_API bool nsl_set_disjoint(const nsl_Set* set, const nsl_Set* other);

NSL_API void nsl_set_intersection(const nsl_Set *set, const nsl_Set *other, nsl_Set* out);
NSL_API void nsl_set_difference(const nsl_Set *set, const nsl_Set *other, nsl_Set* out);
NSL_API void nsl_set_union(const nsl_Set *set, const nsl_Set *other, nsl_Set* out);

// include/nsl/structs/map.h


typedef enum {
    NSL_MAP_DYNAMIC,
    NSL_MAP_I64,
    NSL_MAP_U64,
    NSL_MAP_F64,
    NSL_MAP_PTR,
} nsl_MapType;

typedef union {
    i64 i64;
    u64 u64;
    f64 f64;
    void* ptr;
    const void* const_ptr;
} nsl_MapValue;

typedef struct {
    u64 hash;
    nsl_MapValue value;
} nsl_MapItem;

typedef struct nsl_Map {
    nsl_MapType type;
    usize len;
    usize cap;
    usize del;
    nsl_Arena *arena;
    nsl_MapItem *items;
} nsl_Map;

#define NSL_MAP_DEFAULT_SIZE 8
#define NSL_MAP_DELETED ((u64)0xdeaddeaddeaddead)

NSL_API void nsl_map_init(nsl_Map *map, nsl_MapType type, nsl_Arena *arena);

NSL_API void nsl_map_update(nsl_Map *map, nsl_Map *other);
NSL_API void nsl_map_extend(nsl_Map *map, usize count, nsl_MapItem *items);

NSL_API void nsl_map_clear(nsl_Map *map);

NSL_API void nsl_map_resize(nsl_Map *map, usize size);
NSL_API void nsl_map_reserve(nsl_Map *map, usize size);

NSL_API bool nsl_map_remove(nsl_Map *map, u64 hash);

NSL_API void nsl_map_insert_i64(nsl_Map *map, u64 hash, i64 value);
NSL_API void nsl_map_insert_u64(nsl_Map *map, u64 hash, u64 value);
NSL_API void nsl_map_insert_f64(nsl_Map *map, u64 hash, f64 value);
NSL_API void nsl_map_insert_ptr(nsl_Map *map, u64 hash, void *value);

NSL_API i64 *nsl_map_get_i64(nsl_Map *map, u64 hash);
NSL_API u64 *nsl_map_get_u64(nsl_Map *map, u64 hash);
NSL_API f64 *nsl_map_get_f64(nsl_Map *map, u64 hash);
NSL_API void *nsl_map_get_ptr(nsl_Map *map, u64 hash);

NSL_API const i64 *nsl_map_get_i64_const(const nsl_Map *map, u64 hash);
NSL_API const u64 *nsl_map_get_u64_const(const nsl_Map *map, u64 hash);
NSL_API const f64 *nsl_map_get_f64_const(const nsl_Map *map, u64 hash);
NSL_API const void *nsl_map_get_ptr_const(const nsl_Map *map, u64 hash);

// include/nsl/structs/list.h


#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////

#define nsl_list_first(list) (list)->items[0]
#define nsl_list_last(list) (list)->items[(list)->len - 1]
#define nsl_list_pop(list) (list)->items[--(list)->len]
#define nsl_list_is_empty(list) (!(list)->len)

#define nsl_list_clear(list) ((list)->len = 0)

///////////////////////////////////////////////////////////////////////////////

#define nsl_list_init(list, _arena)                                                                 \
    do {                                                                                           \
        (list)->len = 0;                                                                           \
        (list)->cap = 0;                                                                           \
        (list)->arena = _arena;                                                                    \
        (list)->items = NULL;                                                                      \
    } while (0)

#define nsl_list_copy(src, dest)                                                                    \
    do {                                                                                           \
        nsl_list_resize((dest), (src)->len);                                                        \
        for (usize __c_i = 0; __c_i < (src)->len; __c_i++) {                                       \
            (dest)->items[__c_i] = (src)->items[__c_i];                                            \
        }                                                                                          \
        (dest)->len = (src)->len;                                                                  \
    } while (0)

///////////////////////////////////////////////////////////////////////////////

#define nsl_list_resize(list, size)                                                                 \
    do {                                                                                           \
        if (size < (list)->cap) {                                                                  \
            break;                                                                                 \
        }                                                                                          \
        (list)->cap = size;                                                                        \
        (list)->items = nsl_arena_realloc_chunk((list)->arena, (list)->items,                       \
                                               (list)->cap * sizeof(*(list)->items));              \
    } while (0)

#define nsl_list_reserve(list, size)                                                                \
    do {                                                                                           \
        const usize __rs = (list)->len + size;                                                     \
        if (__rs < (list)->cap) {                                                                  \
            break;                                                                                 \
        }                                                                                          \
        usize __ns = (list)->cap == 0 ? 5 : (list)->cap;                                           \
        while (__ns < __rs) {                                                                      \
            __ns *= 2;                                                                             \
        }                                                                                          \
        nsl_list_resize(list, __ns);                                                                \
    } while (0)

///////////////////////////////////////////////////////////////////////////////

#define nsl_list_push(list, ...)                                                                    \
    do {                                                                                           \
        nsl_list_reserve((list), 1);                                                                \
        (list)->items[(list)->len++] = (__VA_ARGS__);                                              \
    } while (0)

#define nsl_list_extend(list, count, ...)                                                           \
    do {                                                                                           \
        nsl_list_reserve((list), (count));                                                          \
        for (usize __e_i = 0; __e_i < (count); __e_i++) {                                          \
            (list)->items[(list)->len + __e_i] = (__VA_ARGS__)[__e_i];                             \
        }                                                                                          \
        (list)->len += count;                                                                      \
    } while (0)

#define nsl_list_extend_static(list, ...)                                                           \
    do {                                                                                           \
        nsl_list_extend(list, NSL_ARRAY_LEN(__VA_ARGS__), __VA_ARGS__);                              \
    } while (0)

#define nsl_list_extend_list(list, other)                                                             \
    do {                                                                                           \
        nsl_list_reserve((list), (other)->len);                                                     \
        for (usize __e_i = 0; __e_i < (other)->len; __e_i++) {                                     \
            (list)->items[(list)->len + __e_i] = (other)->items[__e_i];                            \
        }                                                                                          \
        (list)->len += (other)->len;                                                               \
    } while (0)

#define nsl_list_insert(list, value, idx)                                                           \
    do {                                                                                           \
        nsl_list_reserve(list, 1);                                                                  \
        for (usize __r_i = idx + 1; __r_i < (list)->len + 1; __r_i++) {                            \
            (list)->items[__r_i] = (list)->items[__r_i - 1];                                       \
        }                                                                                          \
        (list)->items[idx] = value;                                                                \
        (list)->len++;                                                                             \
    } while (0)

#define nsl_list_remove(list, idx)                                                                  \
    do {                                                                                           \
        for (usize __r_i = idx + 1; __r_i < (list)->len; __r_i++) {                                \
            (list)->items[__r_i - 1] = (list)->items[__r_i];                                       \
        }                                                                                          \
        (list)->len--;                                                                             \
    } while (0)

///////////////////////////////////////////////////////////////////////////////

#define nsl_list_map(src, dest, map)                                                                \
    do {                                                                                           \
        nsl_list_reserve((dest), (src)->len);                                                       \
        for (usize __m_i = 0; __m_i < (src)->len; __m_i++) {                                       \
            (dest)->items[__m_i] = map((src)->items[__m_i]);                                       \
        }                                                                                          \
        (dest)->len = (src)->len;                                                                  \
    } while (0)

#define nsl_list_map_ctx(src, dest, map, ctx)                                                       \
    do {                                                                                           \
        nsl_list_reserve((dest), (src)->len);                                                       \
        for (usize __m_i = 0; __m_i < (src)->len; __m_i++) {                                       \
            (dest)->items[__m_i] = map(ctx, (src)->items[__m_i]);                                  \
        }                                                                                          \
        (dest)->len = (src)->len;                                                                  \
    } while (0)

#define nsl_list_filter(src, dest, filter)                                                          \
    do {                                                                                           \
        nsl_list_reserve((dest), (src)->len);                                                       \
        usize __f_count = 0;                                                                       \
        for (usize __f_i = 0; __f_i < (src)->len; __f_i++) {                                       \
            if (filter((src)->items[__f_i])) {                                                     \
                (dest)->items[__f_count++] = (src)->items[__f_i];                                  \
            }                                                                                      \
        }                                                                                          \
        (dest)->len = __f_count;                                                                   \
    } while (0)

#define nsl_list_filter_ctx(src, dest, filter, ctx)                                                 \
    do {                                                                                           \
        nsl_list_reserve((dest), (src)->len);                                                       \
        usize __f_count = 0;                                                                       \
        for (usize __f_i = 0; __f_i < (src)->len; __f_i++) {                                       \
            if (filter((ctx), (src)->items[__f_i])) {                                              \
                (dest)->items[__f_count++] = (src)->items[__f_i];                                  \
            }                                                                                      \
        }                                                                                          \
        (dest)->len = __f_count;                                                                   \
    } while (0)

#define nsl_list_sort(src, sort) qsort((src)->items, (src)->len, sizeof((src)->items[0]), sort)

#define nsl_list_reverse(list)                                                                      \
    do {                                                                                           \
        nsl_list_reserve((list), 1);                                                                \
        for (usize __r_i = 0; __r_i < (list)->len - __r_i - 1; __r_i++) {                          \
            usize __last_idx = (list)->len - __r_i - 1;                                            \
            (list)->items[(list)->len] = (list)->items[__r_i];                                     \
            (list)->items[__r_i] = (list)->items[__last_idx];                                      \
            (list)->items[__last_idx] = (list)->items[(list)->len];                                \
        }                                                                                          \
    } while (0)

#define nsl_list_for_each(T, iter, da)                                                              \
    if ((da)->len) for (T iter = &nsl_list_first(da); iter <= &nsl_list_last(da); iter++)

// include/nsl/os/os.h


typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // error when the directory exists
    bool parents;   // create parent paths
} nsl_OsDirConfig;
void nsl_os_mkdir(nsl_Path path, nsl_Error *error, nsl_OsDirConfig config);

NSL_API void nsl_os_chdir(nsl_Path path, nsl_Error* error);
NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena, nsl_Error *error);
NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Error *error);

// include/nsl/os/fs.h


typedef struct {
    nsl_Path path;
    bool is_dir;
    usize size;
    u64 mtime;
} nsl_FsEntry;

typedef struct {
    nsl_Arena scratch; // per file scratch buffer
    bool recursive;   // recursive
    nsl_Error *error;  // Error
    void *_handle;    // platform specific handle
} nsl_FsIter;

NSL_API nsl_FsIter nsl_fs_begin(nsl_Path directory, bool recursive, nsl_Error *error);
NSL_API void nsl_fs_end(nsl_FsIter *it);

NSL_API nsl_FsEntry *nsl_fs_next(nsl_FsIter *it);

NSL_API bool nsl_fs_exists(nsl_Path path);
NSL_API bool nsl_fs_is_dir(nsl_Path path);
NSL_API bool nsl_fs_remove(nsl_Path path);

// include/nsl/os/file.h


NSL_API FILE *nsl_file_open(nsl_Path path, const char *mode, nsl_Error *error);
NSL_API void nsl_file_close(FILE *file);

NSL_API void nsl_file_check_error(FILE* file, nsl_Error* error);

NSL_API usize nsl_file_size(FILE *file);

NSL_API nsl_Str nsl_file_read_str(FILE *file, nsl_Arena *arena);
NSL_API nsl_Str nsl_file_read_sb(FILE *file, nsl_StrBuilder *sb);
NSL_API nsl_Str nsl_file_read_line(FILE *file, nsl_StrBuilder *sb);

NSL_API nsl_Bytes nsl_file_read_bytes(FILE *file, usize size, u8 *buffer);

NSL_API NSL_FMT(2) void nsl_file_write_fmt(FILE* file, const char* fmt, ...);
NSL_API void nsl_file_write_str(FILE *file, nsl_Str content);
NSL_API void nsl_file_write_bytes(FILE *file, nsl_Bytes content);

// include/nsl/os/cmd.h



typedef enum {
    NSL_CMD_OK = 0,
    // the command return code 1-255
    NSL_CMD_NOT_FOUND = 256,
} nsl_CmdError;

typedef nsl_List(const char*) nsl_Cmd;

#define nsl_cmd_push(cmd, ...)                                                                     \
    nsl_list_extend(cmd,                                                                           \
        NSL_ARRAY_LEN((const char *[]){__VA_ARGS__}),                                              \
        (const char *[]){__VA_ARGS__}                                                              \
    )

#define NSL_CMD(...)                                                                               \
    nsl_cmd_exec(NSL_ARRAY_LEN((const char *[]){__VA_ARGS__}), (const char *[]){__VA_ARGS__})

NSL_API nsl_CmdError nsl_cmd_exec(usize argc, const char** argv);
NSL_API nsl_CmdError nsl_cmd_exec_list(const nsl_Cmd* args);

// include/nsl/core/error.h


#include <stdlib.h>
#include <stdio.h>

#define NSL_ERROR_FMT "%s:%d (%s): %s"
#define NSL_ERROR_ARG(E) (E)->file, (E)->line, (E)->func, (E)->message

#define NSL_ERROR_EMIT(E, error_code, error_message)
#define NSL_ERROR_HANDLE(E, ...)
#define NSL_ERROR_EXCEPT_CASE(E, CODE, ...)
#define NSL_ERROR_PROPAGATE(E, ...)
#define NSL_ERROR_CLEAR(E)
#define NSL_ERROR_RAISE(E)
#define NSL_ERROR_LOG(E)

// Implemetation
#undef NSL_ERROR_EMIT
#define NSL_ERROR_EMIT(E, error_code, error_message)                            \
    do {                                                                       \
        if ((E) == NULL) {                                                            \
            nsl_Error _panic_error = {                                          \
                error_code, __FILE__, __LINE__, __func__, error_message,       \
            };                                                                 \
            NSL_ERROR_RAISE(&_panic_error);                                     \
        } else {                                                               \
            (E)->code = error_code;                                            \
            (E)->file = __FILE__;                                              \
            (E)->line = __LINE__;                                              \
            (E)->func = __func__;                                              \
            (E)->message = error_message;                                      \
        }                                                                      \
    } while (0)

#undef NSL_ERROR_HANDLE
#define NSL_ERROR_HANDLE(E, ...)                                                \
    do {                                                                       \
        if ((E) != NULL && (E)->code) {                                                \
            { __VA_ARGS__; }                                                   \
            if ((E)->code) {                                                   \
                NSL_ERROR_RAISE(E);                                             \
            }                                                                  \
        }                                                                      \
    } while (0)

#undef NSL_ERROR_EXCEPT_CASE
#define NSL_ERROR_EXCEPT_CASE(E, CODE, ...)                                     \
    case CODE: {                                                               \
        __VA_ARGS__;                                                           \
        NSL_ERROR_CLEAR(E);                                                     \
        break;                                                                 \
    }

#undef NSL_ERROR_PROPAGATE
#define NSL_ERROR_PROPAGATE(E, ...)                                             \
    do {                                                                       \
        if ((E) != NULL && (E)->code) {                                                \
            { __VA_ARGS__; }                                                   \
        }                                                                      \
    } while (0)

#undef NSL_ERROR_CLEAR
#define NSL_ERROR_CLEAR(E)                                                      \
    do {                                                                       \
        *(E) = (nsl_Error){0};                                                  \
    } while (0)

#undef NSL_ERROR_RAISE
#define NSL_ERROR_RAISE(E)                                                      \
    do {                                                                       \
        fprintf(stderr, NSL_ERROR_FMT "\n", NSL_ERROR_ARG(E));                   \
        abort();                                                               \
    } while (0)

#undef NSL_ERROR_LOG
#define NSL_ERROR_LOG(E)                                                        \
    do {                                                                       \
        fprintf(stderr, NSL_ERROR_FMT "\n", NSL_ERROR_ARG(E));                   \
    } while (0)

// include/nsl/core/arena.h


NSL_API void nsl_arena_free(nsl_Arena *arena);

NSL_API void *nsl_arena_alloc(nsl_Arena *arena, usize size);
NSL_API void *nsl_arena_calloc(nsl_Arena *arena, usize size);
NSL_API void nsl_arena_reset(nsl_Arena *arena);

NSL_API usize nsl_arena_size(nsl_Arena *arena);
NSL_API usize nsl_arena_real_size(nsl_Arena *arena);

////////////////////////////////////////////////////////////////////////////

NSL_API void *nsl_arena_alloc_chunk(nsl_Arena *arena, usize size);
NSL_API void *nsl_arena_calloc_chunk(nsl_Arena *arena, usize size);
NSL_API void *nsl_arena_realloc_chunk(nsl_Arena *arena, void *ptr, usize size);
NSL_API void nsl_arena_free_chunk(nsl_Arena *arena, void *ptr);

#endif // _NSL_H_

#ifdef NSL_IMPLEMENTATION
// src/nsl/types/str.c


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Str nsl_str_from_parts(usize size, const char *cstr) {
    return (nsl_Str){.len = size, .data = cstr};
}

NSL_API nsl_Str nsl_str_from_bytes(nsl_Bytes bytes) {
    return nsl_str_from_parts(bytes.size, (const char *)bytes.data);
}

NSL_API nsl_Bytes nsl_str_to_bytes(nsl_Str s) {
    return nsl_bytes_from_parts(s.len, s.data);
}

NSL_API nsl_Str nsl_str_from_cstr(const char *cstr) {
    return (nsl_Str){.len = strlen(cstr), .data = cstr};
}

NSL_API const char* nsl_str_to_cstr(nsl_Str str, nsl_Arena* arena) {
    char* cstr = nsl_arena_calloc(arena, str.len + 1);
    memcpy(cstr, str.data, str.len);
    return cstr;
}

NSL_API nsl_Str nsl_str_format(nsl_Arena *arena, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    usize size = (usize)vsnprintf(NULL, 0, fmt, va) + 1;
    va_end(va);

    char *buffer = nsl_arena_calloc(arena, size);
    va_start(va, fmt);
    vsnprintf(buffer, size, fmt, va);
    va_end(va);
    return nsl_str_from_parts(size - 1, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Str nsl_str_copy(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    memcpy(buffer, s.data, s.len);
    buffer[s.len] = '\0';
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API nsl_Str nsl_str_append(nsl_Str s, nsl_Str suffix, nsl_Arena *arena) {
    const usize new_size = s.len + suffix.len;
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], s.data, s.len);
    memcpy(&buffer[s.len], suffix.data, suffix.len);
    buffer[new_size] = '\0';
    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_prepend(nsl_Str s, nsl_Str prefix, nsl_Arena *arena) {
    const usize new_size = s.len + prefix.len;
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], prefix.data, prefix.len);
    memcpy(&buffer[prefix.len], s.data, s.len);
    buffer[new_size] = '\0';
    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_wrap(nsl_Str s, nsl_Str wrap, nsl_Arena *arena) {
    const usize new_size = s.len + wrap.len * 2;
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], wrap.data, wrap.len);
    memcpy(&buffer[wrap.len], s.data, s.len);
    memcpy(&buffer[wrap.len + s.len], wrap.data, wrap.len);
    buffer[new_size] = '\0';
    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_join(nsl_Str sep, usize count, nsl_Str *s, nsl_Arena *arena) {
    if (count == 0) {
        return NSL_STR("");
    }
    usize size = sep.len * (count - 1);
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        if (i != 0) {
            memcpy(&buffer[b_idx], sep.data, sep.len);
            b_idx += sep.len;
        }
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
    }
    buffer[size] = '\0';

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_join_suffix(nsl_Str suffix, usize count, nsl_Str *s, nsl_Arena *arena) {
    usize size = suffix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
        memcpy(&buffer[b_idx], suffix.data, suffix.len);
        b_idx += suffix.len;
    }
    buffer[size] = '\0';

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_join_prefix(nsl_Str prefix, usize count, nsl_Str *s, nsl_Arena *arena) {
    usize size = prefix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], prefix.data, prefix.len);
        b_idx += prefix.len;
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
    }
    buffer[size] = '\0';

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_join_wrap(nsl_Str sep, nsl_Str wrap, usize count, nsl_Str *s, nsl_Arena *arena) {
    usize size = sep.len * (count - 1) + wrap.len * count * 2;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        if (i != 0) {
            memcpy(&buffer[b_idx], sep.data, sep.len);
            b_idx += sep.len;
        }
        memcpy(&buffer[b_idx], wrap.data, wrap.len);
        b_idx += wrap.len;
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
        memcpy(&buffer[b_idx], wrap.data, wrap.len);
        b_idx += wrap.len;
    }
    buffer[size] = '\0';

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_upper(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nsl_char_to_upper(s.data[i]);
    }
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API nsl_Str nsl_str_lower(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nsl_char_to_lower(s.data[i]);
    }
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API nsl_Str nsl_str_replace(nsl_Str s, nsl_Str old, nsl_Str new, nsl_Arena *arena) {
    usize count = nsl_str_count(s, old);
    usize new_size = (s.len - (old.len * count) + (count * new.len));
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    buffer[new_size] = '\0';

    for (usize i = 0, j = 0; i < s.len;) {
        if (old.len <= s.len - i && memcmp(&s.data[i], old.data, old.len) == 0) {
            memcpy(&buffer[j], new.data, new.len);
            i += old.len;
            j += new.len;
        } else {
            buffer[j++] = s.data[i++];
        }
    }

    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_center(nsl_Str s, usize width, char fillchar, nsl_Arena *arena) {
    if (width < s.len) {
        return nsl_str_copy(s, arena);
    }
    char *buffer = nsl_arena_calloc(arena, width + 1);
    const usize left_width = (width - s.len) / 2;
    const usize right_width = (width - s.len - left_width);
    usize idx = 0;
    for (usize i = 0; i < left_width; i++) {
        buffer[idx++] = fillchar;
    }
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    for (usize i = 0; i < right_width; i++) {
        buffer[idx++] = fillchar;
    }
    return nsl_str_from_parts(width, buffer);
}

NSL_API nsl_Str nsl_str_ljust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena) {
    if (width < s.len) {
        return nsl_str_copy(s, arena);
    }
    char *buffer = nsl_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    return nsl_str_from_parts(width, buffer);
}

NSL_API nsl_Str nsl_str_rjust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena) {
    if (width < s.len) {
        return nsl_str_copy(s, arena);
    }
    char *buffer = nsl_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }

    return nsl_str_from_parts(width, buffer);
}

NSL_API nsl_Str nsl_str_repeat(nsl_Str s, usize count, nsl_Arena *arena) {
    usize len = s.len * count;
    char *buffer = nsl_arena_alloc(arena, len + 1);
    buffer[len] = '\0';

    usize idx = 0;
    for (usize i = 0; i < count; i++) {
        for (usize j = 0; j < s.len; j++) {
            buffer[idx++] = s.data[j];
        }
    }

    return nsl_str_from_parts(len, buffer);
}

NSL_API nsl_Str nsl_str_reverse(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = s.data[s.len - i - 1];
    }
    return nsl_str_from_parts(s.len, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NSL_API bool nsl_str_eq(nsl_Str s1, nsl_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    return memcmp(s1.data, s2.data, s1.len) == 0;
}

NSL_API bool nsl_str_eq_ignorecase(nsl_Str s1, nsl_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    for (size_t i = 0; i < s1.len; i++) {
        if (nsl_char_to_lower(s1.data[i]) != nsl_char_to_lower(s2.data[i])) {
            return false;
        }
    }
    return true;
}

NSL_API bool nsl_str_startswith(nsl_Str s1, nsl_Str prefix) {
    if (s1.len < prefix.len) {
        return false;
    }
    return memcmp(s1.data, prefix.data, prefix.len) == 0;
}

NSL_API bool nsl_str_endswith(nsl_Str s1, nsl_Str suffix) {
    if (s1.len < suffix.len) {
        return false;
    }
    const usize idx = s1.len - suffix.len;
    return memcmp(&s1.data[idx], suffix.data, suffix.len) == 0;
}

NSL_API bool nsl_str_endswith_predicate(nsl_Str s1, bool (*predicate)(char)) {
    if (s1.len == 0) {
        return false;
    }
    const usize idx = s1.len - 1;
    return predicate(s1.data[idx]);
}

NSL_API bool nsl_str_contains(nsl_Str haystack, nsl_Str needle) {
    if (haystack.len < needle.len) {
        return false;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            return true;
        }
    }
    return false;
}

NSL_API bool nsl_str_includes(nsl_Str haystack, char needle) {
    if (haystack.len == 0) {
        return false;
    }
    for (usize i = 0; i < haystack.len; i++) {
        if (haystack.data[i] == needle) {
            return true;
        }
    }
    return false;
}

NSL_API bool nsl_str_is_empty(nsl_Str s) {
    return s.len == 0;
}

NSL_API nsl_Str nsl_str_trim_left(nsl_Str s) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && nsl_char_is_space(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_left_by_delim(nsl_Str s, char delim) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && s.data[i] == delim; ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_left_by_predicate(nsl_Str s, bool (*predicate)(char)) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_right(nsl_Str s) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && nsl_char_is_space(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_right_by_delim(nsl_Str s, char delim) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && s.data[s.len - i - 1] == delim; ++i) {
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_right_by_predicate(nsl_Str s, bool (*predicate)(char)) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim(nsl_Str s) {
    return nsl_str_trim_left(nsl_str_trim_right(s));
}

NSL_API nsl_Str nsl_str_trim_by_delim(nsl_Str s, char delim) {
    return nsl_str_trim_left_by_delim(nsl_str_trim_right_by_delim(s, delim), delim);
}

NSL_API nsl_Str nsl_str_trim_by_predicate(nsl_Str s, bool (*predicate)(char)) {
    return nsl_str_trim_left_by_predicate(nsl_str_trim_right_by_predicate(s, predicate), predicate);
}

NSL_API bool nsl_str_try_chop_by_delim(nsl_Str *s, char delim, nsl_Str *chunk) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        if (chunk) *chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return true;
    }
    return false;
}

NSL_API nsl_Str nsl_str_chop_by_delim(nsl_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return chunk;
    }
    return *s;
}

NSL_API bool nsl_str_try_chop_by_predicate(nsl_Str *s, bool (*predicate)(char), nsl_Str *chunk) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        if (chunk) *chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return true;
    }
    return false;
}

NSL_API nsl_Str nsl_str_chop_by_predicate(nsl_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return chunk;
    }
    return *s;
}

NSL_API nsl_Str nsl_str_chop_right_by_delim(nsl_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[s->len - i - 1] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nsl_usize_min(s->len, i + 1);
        return chunk;
    }
    return *s;
}

NSL_API nsl_Str nsl_str_chop_right_by_predicate(nsl_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[s->len - i - 1])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nsl_usize_min(s->len, i + 1);
        return chunk;
    }
    return *s;
}

NSL_API nsl_Str nsl_str_substring(nsl_Str s, usize start, usize end) {
    if (end <= start || s.len <= start || s.len < end) {
        return NSL_STR("");
    }
    return nsl_str_from_parts(end - start, &s.data[start]);
}

NSL_API nsl_Str nsl_str_take(nsl_Str *s, usize count) {
    count = nsl_usize_min(s->len, count);
    nsl_Str ret = nsl_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return ret;
}

NSL_API bool nsl_str_try_take(nsl_Str *s, usize count, nsl_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nsl_usize_min(s->len, count);
    *chunk = nsl_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return true;
}

NSL_API nsl_Str nsl_str_take_right(nsl_Str *s, usize count) {
    count = nsl_usize_min(s->len, count);
    nsl_Str ret = nsl_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return ret;
}

NSL_API bool nsl_str_try_take_right(nsl_Str *s, usize count, nsl_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nsl_usize_min(s->len, count);
    *chunk = nsl_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return true;
}

NSL_API u64 nsl_str_u64(nsl_Str s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(s, &arena);
    u64 value = strtoull(owned.data, NULL, radix);
    nsl_arena_free(&arena);
    return value;
}

NSL_API u64 nsl_str_chop_u64(nsl_Str *s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(*s, &arena);
    char *endptr;
    u64 value = strtoull(owned.data, &endptr, radix);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nsl_arena_free(&arena);
    return value;
}

NSL_API i64 nsl_str_i64(nsl_Str s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(s, &arena);
    i64 value = strtoll(owned.data, NULL, radix);
    nsl_arena_free(&arena);
    return value;
}

NSL_API i64 nsl_str_chop_i64(nsl_Str *s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(*s, &arena);
    char *endptr;
    i64 value = strtoll(owned.data, &endptr, radix);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nsl_arena_free(&arena);
    return value;
}

NSL_API f64 nsl_str_f64(nsl_Str s) {
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(s, &arena);
    double value = strtod(owned.data, NULL);
    nsl_arena_free(&arena);
    return value;
}

NSL_API f64 nsl_str_chop_f64(nsl_Str *s) {
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(*s, &arena);

    char *endptr;
    f64 value = strtod(owned.data, &endptr);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nsl_arena_free(&arena);
    return value;
}

NSL_API usize nsl_str_find(nsl_Str haystack, nsl_Str needle) {
    if (haystack.len < needle.len) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            return i;
        }
    }
    return NSL_STR_NOT_FOUND;
}


NSL_API usize nsl_str_find_by_predicate(nsl_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len; i++) {
        if (predicate(haystack.data[i])) {
            return i;
        }
    }
    return NSL_STR_NOT_FOUND;
}

NSL_API usize nsl_str_find_last(nsl_Str haystack, nsl_Str needle) {
    if (haystack.len < needle.len) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = haystack.len - needle.len + 1; i > 0; i--) {
        if (memcmp(&haystack.data[i - 1], needle.data, needle.len) == 0) {
            return i - 1;
        }
    }
    return NSL_STR_NOT_FOUND;
}


NSL_API usize nsl_str_find_last_by_predicate(nsl_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = haystack.len; i > 0; i--) {
        if (predicate(haystack.data[i - 1])) {
            return i - 1;
        }
    }
    return NSL_STR_NOT_FOUND;
}

NSL_API usize nsl_str_count(nsl_Str haystack, nsl_Str needle) {
    usize count = 0;
    if (haystack.len < needle.len) {
        return count;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            count++;
            i += needle.len - 1;
        }
    }
    return count;
}

NSL_API char nsl_str_getc(nsl_Str s, usize idx) {
    if (s.len <= idx) {
        return '\0';
    }
    return s.data[idx];
}

NSL_API u64 nsl_str_hash(nsl_Str s) {
    const uint64_t magic_prime = 0x00000100000001b3;
    uint64_t hash = 0xcbf29ce484222325; // NOLINT
    for (usize i = 0; i < s.len; ++i) {
        hash = (hash ^ (u64)s.data[i]) * magic_prime;
    }
    return hash;
}
// src/nsl/types/path.c


#include <stdio.h>
#include <string.h>

NSL_API nsl_Path nsl_path_join(usize len, const nsl_Path *parts, nsl_Arena *arena) {
    if (len == 0) {
        return NSL_STR("");
    }
    if (len == 1) {
        return nsl_str_copy(parts[0], arena);
    }
    nsl_Arena scratch = {0};
    nsl_List(char) buffer = {0};
    nsl_list_init(&buffer, &scratch);

    for (usize i = 0; i < len; i++) {
        nsl_list_reserve(&buffer, parts[i].len + 2);
        if (i && buffer.len && !nsl_char_is_path_delimiter(nsl_list_last(&buffer))) {
            nsl_list_push(&buffer, '/');
        }
        for (usize j = 0; j < parts[i].len; j++) {
            if (nsl_char_is_path_delimiter(parts[i].data[j])) {
                if (buffer.len && nsl_char_is_path_delimiter(nsl_list_last(&buffer))) {
                    continue;
                }
                nsl_list_push(&buffer, '/');
            } else {
                nsl_list_push(&buffer, parts[i].data[j]);
            }
        }
    }


    nsl_Str result = nsl_str_copy(nsl_str_from_parts(buffer.len, buffer.items), arena);
    nsl_arena_free(&scratch);
    return result;
}

NSL_API nsl_Path nsl_path_normalize(nsl_Path path, nsl_Arena *arena) {
    nsl_Arena scratch = {0};
    nsl_List(nsl_Path) parts = {0};
    nsl_list_init(&parts, &scratch);
    char win_path_prefix_buffer[4] = "C:/";

    nsl_Path prefix = NSL_PATH("");
    if (nsl_path_is_absolute(path)) {
        if (path.len && nsl_char_is_path_delimiter(path.data[0])) {
            prefix = NSL_PATH("/");
        } else if (path.len > 2) {
            win_path_prefix_buffer[0] = path.data[0];
            prefix = nsl_str_from_parts(3, win_path_prefix_buffer);
            path = nsl_str_substring(path, 3, path.len);
        }
    }

    nsl_Path part;
    while (nsl_str_try_chop_by_predicate(&path, nsl_char_is_path_delimiter, &part)) {
        if (part.len == 0) {
            continue;
        } else if (nsl_str_eq(part, NSL_STR("."))) {
            continue;
        } else if (nsl_str_eq(part, NSL_STR(".."))) {
            if (parts.len) (void)nsl_list_pop(&parts);
            else if (!prefix.len) nsl_list_push(&parts, part);
            continue;
        }
        nsl_list_push(&parts, part);
    }

    nsl_Path result = nsl_path_join(parts.len, parts.items, &scratch);
    result = nsl_str_prepend(result, prefix, arena);
    nsl_arena_free(&scratch);
    return result;
}

NSL_API bool nsl_path_eq(nsl_Path p1, nsl_Path p2) {
    while (true) {
        nsl_Path c1 = nsl_str_chop_by_predicate(&p1, nsl_char_is_path_delimiter);
        nsl_Path c2 = nsl_str_chop_by_predicate(&p2, nsl_char_is_path_delimiter);
        if (c1.len == 0 && c2.len == 0) break;
        if (!nsl_str_eq(c1, c2)) return false;;
    }
    return true;
}

NSL_API bool nsl_path_is_absolute(nsl_Path path) {
    if (path.len == 0)
        return false;
    if (nsl_char_is_path_delimiter(path.data[0]))
        return true;
    if (path.len >= 3 && path.data[1] == ':' && nsl_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}

NSL_API bool nsl_path_is_root(nsl_Path path) {
    if (path.len == 1 && nsl_char_is_path_delimiter(path.data[0]))
        return true;
    if (path.len == 3 && path.data[1] == ':' && nsl_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}

NSL_API nsl_Str nsl_path_name(nsl_Path path) {
    if (nsl_str_eq(path, NSL_STR("."))) {
        return NSL_STR("");
    }
    return nsl_str_chop_right_by_predicate(&path, nsl_char_is_path_delimiter);
}

NSL_API nsl_Str nsl_path_suffix(nsl_Path path) {
    if (nsl_str_eq(path, NSL_STR("."))) {
        return NSL_STR("");
    }
    nsl_Str name = nsl_str_chop_right_by_predicate(&path, nsl_char_is_path_delimiter);
    usize idx = nsl_str_find_last(name, NSL_STR("."));
    if (idx == NSL_STR_NOT_FOUND) {
        return NSL_STR("");
    }
    return nsl_str_substring(name, idx, name.len);
}

NSL_API nsl_Str nsl_path_stem(nsl_Path path) {
    nsl_Str name = nsl_str_chop_right_by_predicate(&path, nsl_char_is_path_delimiter);
    usize idx = nsl_str_find_last(name, NSL_STR("."));
    if (idx == NSL_STR_NOT_FOUND) {
        return name;
    }
    return nsl_str_substring(name, 0, idx);
}

NSL_API nsl_Path nsl_path_parent(nsl_Path path) {
    if (path.len == 1 && nsl_char_is_path_delimiter(path.data[0])) return path;
    if (nsl_str_endswith_predicate(path, nsl_char_is_path_delimiter)) path.len--;
    usize idx = nsl_str_find_last_by_predicate(path, nsl_char_is_path_delimiter);
    if (idx == NSL_STR_NOT_FOUND) return NSL_PATH(".");
    return nsl_str_substring(path, 0, nsl_usize_min(path.len, idx+1));
}
// src/nsl/types/int.c


#include <string.h>

#define BITS(T) (sizeof(T) * 8)

#define INTEGER_IMPL(T)                                                                            \
    NSL_API T nsl_##T##_reverse_bits(T value) {                                                      \
        T reversed = 0;                                                                            \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            reversed = (T)(reversed << 1);                                                         \
            if (value & 1) {                                                                       \
                reversed = reversed | 1;                                                           \
            }                                                                                      \
            value = value >> 1;                                                                    \
        }                                                                                          \
        return reversed;                                                                           \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_leading_ones(T value) {                                                  \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> (BITS(T) - i - 1) & (T)0x1)) {                                          \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_trailing_ones(T value) {                                                 \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> i & (T)0x1)) {                                                          \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_leading_zeros(T value) {                                                 \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> (BITS(T) - i - 1) & (T)0x1) {                                             \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_trailing_zeros(T value) {                                                \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> i & (T)0x1) {                                                             \
                break;                                                                             \
            }                                                                                      \
            count++;                                                                               \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_count_zeros(T value) {                                                   \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> i & (T)0x1)) {                                                          \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API usize nsl_##T##_count_ones(T value) {                                                    \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> i & (T)0x1) {                                                             \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_swap_bytes(T value) {                                                        \
        if (1 < sizeof(T)) {                                                                       \
            u8 *bytes = (u8 *)&value;                                                              \
            for (usize i = 0; i < (sizeof(T) + 1) / 2; i++) {                                      \
                u8 temp = bytes[i];                                                                \
                bytes[i] = bytes[sizeof(T) - i - 1];                                               \
                bytes[sizeof(T) - i - 1] = temp;                                                   \
            }                                                                                      \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_to_be(T value) {                                                             \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                                   \
            return nsl_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_be(T value) {                                                           \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                                   \
            return nsl_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_be_bytes(nsl_Bytes bytes) {                                              \
        NSL_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                                   \
            return nsl_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API nsl_Bytes nsl_##T##_to_be_bytes(T value, nsl_Arena *arena) {                               \
        u8 *buffer = nsl_arena_alloc(arena, sizeof(value));                                         \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                  \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nsl_bytes_from_parts(sizeof(value), buffer);                                         \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_to_le(T value) {                                                             \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                      \
            return nsl_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_le(T value) {                                                           \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                      \
            return nsl_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_le_bytes(nsl_Bytes bytes) {                                              \
        NSL_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                      \
            return nsl_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API nsl_Bytes nsl_##T##_to_le_bytes(T value, nsl_Arena *arena) {                               \
        u8 *buffer = nsl_arena_alloc(arena, sizeof(value));                                         \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NSL_BYTE_ORDER == NSL_ENDIAN_LITTLE) {                                               \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nsl_bytes_from_parts(sizeof(value), buffer);                                         \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_from_ne_bytes(nsl_Bytes bytes) {                                              \
        NSL_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NSL_API nsl_Bytes nsl_##T##_to_ne_bytes(T value, nsl_Arena *arena) {                               \
        if (NSL_BYTE_ORDER == NSL_ENDIAN_BIG) {                                                      \
            return nsl_##T##_to_be_bytes(value, arena);                                             \
        }                                                                                          \
        return nsl_##T##_to_le_bytes(value, arena);                                                 \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_max(T a, T b) {                                                              \
        return a < b ? b : a;                                                                      \
    }                                                                                              \
    NSL_API T nsl_##T##_min(T a, T b) {                                                              \
        return a > b ? b : a;                                                                      \
    }                                                                                              \
    NSL_API T nsl_##T##_clamp(T min, T max, T value) {                                               \
        return value < min ? min : max < value ? max : value;                                      \
    }                                                                                              \
                                                                                                   \
    NSL_API u64 nsl_##T##_hash(T value) {                                                            \
        u64 hash = ((u64)value) + 1;                                                               \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (hash >> 16) ^ hash;                                                                \
        return hash;                                                                               \
    }                                                                                              \
                                                                                                   \
    NSL_API void nsl_##T##_swap(T *v1, T *v2) {                                                      \
        T temp = *v1;                                                                              \
        *v1 = *v2;                                                                                 \
        *v2 = temp;                                                                                \
    }                                                                                              \
                                                                                                   \
    NSL_API T nsl_##T##_next_pow2(T n) {                                                             \
        /* https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2 */                 \
        if (n == 0) return 1;                                                                      \
        u64 x = (u64)n;                                                                            \
        x--;                                                                                       \
        for (size_t i = 1; i < sizeof(T) * 8; i <<= 1) {                                           \
            x |= x >> i;                                                                           \
        }                                                                                          \
        T max = (((T)-1) > 0) ? (T)-1 : (T)((1ULL << (sizeof(T) * 8 - 1)) - 1);                    \
        if (x >= (u64)max) return max;                                                             \
        return (T)(x + 1);                                                                         \
    }

INTEGER_IMPL(u8)
INTEGER_IMPL(i8)
INTEGER_IMPL(u16)
INTEGER_IMPL(i16)
INTEGER_IMPL(u32)
INTEGER_IMPL(i32)
INTEGER_IMPL(u64)
INTEGER_IMPL(i64)
INTEGER_IMPL(usize)

#undef INTEGER_IMPL
#undef BITS
// src/nsl/types/char.c

#include <ctype.h>

#define DBASE 10
#define XBASE 16

///////////////////////////////////////////////////////////////////////////////

NSL_API bool nsl_char_is_alnum(char c) { return isalnum(c); }
NSL_API bool nsl_char_is_alpha(char c) { return isalpha(c); }
NSL_API bool nsl_char_is_lower(char c) { return islower(c); }
NSL_API bool nsl_char_is_upper(char c) { return isupper(c); }
NSL_API bool nsl_char_is_space(char c) { return isspace(c); }
NSL_API bool nsl_char_is_cntrl(char c) { return iscntrl(c); }
NSL_API bool nsl_char_is_print(char c) { return isprint(c); }
NSL_API bool nsl_char_is_graph(char c) { return isgraph(c); }
NSL_API bool nsl_char_is_blank(char c) { return isblank(c); }
NSL_API bool nsl_char_is_punct(char c) { return ispunct(c); }
NSL_API bool nsl_char_is_digit(char c) { return isdigit(c); }
NSL_API bool nsl_char_is_xdigit(char c) { return isxdigit(c); }
NSL_API NSL_CONST_FN bool nsl_char_is_path_delimiter(char c) { return c == '/' || c == '\\'; }

///////////////////////////////////////////////////////////////////////////////

NSL_API char nsl_char_to_lower(char c) { return (char)tolower(c); }
NSL_API char nsl_char_to_upper(char c) { return (char)toupper(c); }

NSL_API u8 nsl_char_to_u8(char c) {
  NSL_ASSERT(nsl_char_is_digit(c) && "char not convertible");
  return (u8)c - '0';
}

NSL_API u8 nsl_char_hex_to_u8(char c) {
  NSL_ASSERT(nsl_char_is_xdigit(c) && "char not convertible");
  if ('0' <= c && c <= '9') {
    return nsl_char_to_u8(c);
  }
  if ('a' <= c && c <= 'f') {
    return (u8)(DBASE + c - 'a');
  }
  if ('A' <= c && c <= 'F') {
    return (u8)(DBASE + c - 'A');
  }
  return 0;
}

NSL_API char nsl_char_from_u8(u8 d) {
  NSL_ASSERT(d < DBASE && "char not convertible");
  return '0' + (i8)d;
}

NSL_API char nsl_char_hex_from_u8(u8 d) {
  NSL_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nsl_char_from_u8(d);
  }
  if (d < XBASE) {
    return (char)('a' + (d - DBASE));
  }
  return 0;
}

NSL_API char nsl_char_HEX_from_u8(u8 d) {
  NSL_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nsl_char_from_u8(d);
  }
  if (d < XBASE) {
    return (char)('A' + (d - DBASE));
  }
  return 0;
}
// src/nsl/types/byte.c


#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Bytes nsl_bytes_from_parts(usize size, const void *data) {
    return (nsl_Bytes){.size = size, .data = data};
}

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Bytes nsl_bytes_copy(nsl_Bytes bytes, nsl_Arena *arena) {
    u8 *buffer = nsl_arena_alloc(arena, bytes.size);
    memcpy(buffer, bytes.data, bytes.size);
    return nsl_bytes_from_parts(bytes.size, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Bytes nsl_bytes_slice(nsl_Bytes bytes, usize idx1, usize idx2) {
    if (idx2 <= idx1 || bytes.size <= idx1 || bytes.size < idx2) {
        return nsl_bytes_from_parts(0, bytes.data);
    }
    return nsl_bytes_from_parts(idx2 - idx1, &bytes.data[idx1]);
}

NSL_API nsl_Bytes nsl_bytes_take(nsl_Bytes *bytes, usize count) {
    count = nsl_usize_min(bytes->size, count);
    nsl_Bytes ret = nsl_bytes_from_parts(count, bytes->data);
    bytes->size -= count;
    bytes->data += count;
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

NSL_API bool nsl_bytes_eq(nsl_Bytes b1, nsl_Bytes b2) {
    if (b1.size != b2.size) return false;
    return memcmp(b1.data, b2.data, b1.size) == 0;
}

NSL_API u64 nsl_bytes_hash(nsl_Bytes bytes) {
    const u64 offset = 2166136261UL;
    const u64 prime = 16777619;
    u64 hash = offset;
    for (usize i = 0; i < bytes.size; i++) {
        hash ^= (unsigned long)bytes.data[i];
        hash *= prime;
    }
    return hash;
}

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Str nsl_bytes_to_hex(nsl_Bytes bytes, nsl_Arena *arena) {
    char *buf = nsl_arena_calloc(arena, bytes.size * 2 + 1);
    usize idx = 0;
    for (usize i = 0; i < bytes.size; i++) {
        idx += (usize)snprintf(&buf[idx], 3, "%0*x", (i != 0) + 1, bytes.data[i]);
    }
    return (nsl_Str){.len = idx, .data = buf};
}

NSL_API nsl_Bytes nsl_bytes_from_hex(nsl_Str s, nsl_Arena *arena) {
    if (nsl_str_startswith(s, NSL_STR("0x"))) {
        s = nsl_str_substring(s, 2, s.len);
    }

    u8 *buffer = nsl_arena_calloc(arena, (s.len / 2) + (s.len % 2));
    // to convert strings like "0x101".
    // in the first iteration:
    // take 1 or 2 chars depending if s.len is even or odd
    usize idx = 0;
    for (nsl_Str ch = {0}; nsl_str_try_take(&s, idx == 0 ? 2 - s.len % 2 : 2, &ch);) {
        for (usize i = 0; i < ch.len; i++) {
            buffer[idx] = (u8)(buffer[idx] << 4);
            if (nsl_char_is_xdigit(ch.data[i])) {
                buffer[idx] |= nsl_char_hex_to_u8(ch.data[i]);
            }
        }
        idx++;
    }
    return nsl_bytes_from_parts(idx, buffer);
}

///////////////////////////////////////////////////////////////////////////////
// src/nsl/structs/set.c


NSL_API void nsl_set_init(nsl_Set *set, nsl_Arena *arena) {
    NSL_ASSERT(set && arena);
    NSL_ASSERT(set->items == NULL && "The map was already initialized");

    set->len = 0;
    set->_cap = 0;
    set->_del = 0;
    set->arena = arena;
    set->items = NULL;
}

NSL_API void nsl_set_resize(nsl_Set *set, usize size) {
    if (size < set->_cap) {
        return;
    }
    usize old_cap = set->_cap;
    u64 *old_items = set->items;

    set->_cap = size == 0 ? NSL_SET_DEFAULT_SIZE : nsl_usize_next_pow2(size);
    set->items = nsl_arena_calloc_chunk(set->arena, set->_cap * sizeof(set->items[0]));

    set->len = 0;
    set->_del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i] && old_items[i] != NSL_SET_DELETED) {
            nsl_set_add(set, old_items[i]);
        }
    }
    nsl_arena_free_chunk(set->arena, old_items);
}

NSL_API void nsl_set_reserve(nsl_Set *map, usize size) {
    usize target = map->len + size;
    if (target <= map->_cap) return;
    nsl_set_resize(map, target);
}

NSL_API bool nsl_set_remove(nsl_Set *set, u64 hash) {
    if (set->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_SET_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize idx = hash & (set->_cap - 1);
    for (usize i = 0; i < set->_cap; i++) {
        if (set->items[idx] == 0) {
            return false;
        }
        if (set->items[idx] && set->items[idx] == hash) {
            set->items[idx] = NSL_SET_DELETED;
            set->len--;
            set->_del++;
            return true;
        }
        idx = (idx + i * i) & (set->_cap - 1);
    }
    return false;
}

NSL_API bool nsl_set_add(nsl_Set *set, u64 hash) {
    if (set->_cap <= set->len + set->_del) {
        nsl_set_resize(set, set->_cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_SET_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (set->_cap - 1);

        for (usize i = 0; i < set->_cap; i++) {
            if (set->items[idx] == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    set->items[del_idx] = hash;
                    set->len++;
                    set->_del--;
                    return true;
                }
                set->items[idx] = hash;
                set->len++;
                return true;
            } else if (set->items[idx] == hash) {
                return false;
            } else if (set->items[idx] == NSL_SET_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (set->_cap - 1);
        }

        nsl_set_resize(set, set->_cap * 2);
    }

    NSL_UNREACHABLE("nsl_set_add");
}

NSL_API bool nsl_set_has(const nsl_Set *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_SET_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize idx = hash & (map->_cap - 1);
    for (usize i = 0; i < map->_cap; i++) {
        if (map->items[idx] == 0) {
            return false;
        }
        if (map->items[idx] == hash) {
            return true;
        }
        idx = (idx + i * i) & (map->_cap - 1);
    }

    return false;
}

NSL_API void nsl_set_update(nsl_Set* map, const nsl_Set* other) {
    nsl_set_reserve(map, other->len);
    for (usize i = 0; i < other->_cap; ++i) {
        if (other->items[i] && other->items[i] != NSL_SET_DELETED) {
            nsl_set_add(map, other->items[i]);
        }
    }
}

NSL_API void nsl_set_extend(nsl_Set* set, usize count, const u64* hashes) {
  nsl_set_reserve(set, count);
  for (usize i = 0; i < count; i++) {
    nsl_set_add(set, hashes[i]);
  }
}

NSL_API bool nsl_set_eq(const nsl_Set *set, const nsl_Set *other) {
  if (other->len != set->len) {
    return false;
  }

  if (other->_cap < set->_cap) {
    const nsl_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

NSL_API bool nsl_set_subset(const nsl_Set *set, const nsl_Set *other) {
  if (other->len < set->len) {
    return false;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

NSL_API bool nsl_set_disjoint(const nsl_Set *set, const nsl_Set *other) {
  if (other->len == 0 || set->len == 0) {
    return true;
  }

  if (other->_cap < set->_cap) {
    const nsl_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (nsl_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}

NSL_API void nsl_set_intersection(const nsl_Set *set, const nsl_Set *other, nsl_Set* out) {
  if (other->_cap < set->_cap) {
    const nsl_Set *temp = set;
    set = other;
    other = temp;
  }

  nsl_set_reserve(out, nsl_usize_min(set->len, other->len) * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (nsl_set_has(other, set->items[i])) {
        nsl_set_add(out, set->items[i]);
      }
    }
  }
}

NSL_API void nsl_set_difference(const nsl_Set *set, const nsl_Set *other, nsl_Set* out) {
  nsl_set_reserve(out, set->len * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        nsl_set_add(out, set->items[i]);
      }
    }
  }
}

NSL_API void nsl_set_union(const nsl_Set *set, const nsl_Set *other, nsl_Set* out) {
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(other, set->items[i])) {
        nsl_set_add(out, set->items[i]);
      }
    }
  }

  for (usize i = 0; i < other->_cap; i++) {
    if (other->items[i] && set->items[i] != NSL_SET_DELETED) {
      if (!nsl_set_has(set, other->items[i])) {
        nsl_set_add(out, other->items[i]);
      }
    }
  }
}
// src/nsl/structs/map.c


#include <string.h>

static void nsl_map_insert(nsl_Map *map, u64 hash, nsl_MapValue value) {
    if (map->cap <= map->len + map->del) {
        nsl_map_resize(map, map->cap * 2);
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_MAP_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (map->cap - 1);

        for (usize i = 0; i < map->cap; i++) {
            if (map->items[idx].hash == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    map->items[del_idx] = (nsl_MapItem){.hash = hash, .value = value};
                    map->len++;
                    map->del--;
                    return;
                }
                map->items[idx] = (nsl_MapItem){.hash = hash, .value = value};
                map->len++;
                return;
            } else if (map->items[idx].hash == hash) {
                map->items[idx].value = value;
                return;
            } else if (map->items[idx].hash == NSL_MAP_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (map->cap - 1);
        }

        nsl_map_resize(map, map->cap * 2);
    }

    NSL_UNREACHABLE("nsl_map_insert");
}

static nsl_MapValue *nsl_map_get(const nsl_Map *map, u64 hash) {
    if (map->len == 0) {
        return NULL;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_MAP_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize idx = hash & (map->cap - 1);
    for (usize i = 0; i < map->cap; i++) {
        if (map->items[idx].hash == 0) {
            return NULL;
        }
        if (map->items[idx].hash == hash) {
            return &map->items[idx].value;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }

    return NULL;
}

NSL_API void nsl_map_init(nsl_Map *map, nsl_MapType type, nsl_Arena *arena) {
    NSL_ASSERT(map->items == NULL && "The map was already initialized");

    map->type = type;
    map->len = 0;
    map->cap = 0;
    map->del = 0;
    map->arena = arena;
    map->items = NULL;
}

NSL_API void nsl_map_update(nsl_Map *map, nsl_Map *other) {
    nsl_map_reserve(map, other->len);
    for (usize i = 0; i < other->cap; ++i) {
        if (other->items[i].hash && other->items[i].hash != NSL_MAP_DELETED) {
            nsl_map_insert(map, other->items[i].hash, other->items[i].value);
        }
    }
}

NSL_API void nsl_map_extend(nsl_Map* map, usize count, nsl_MapItem* items) {
    nsl_map_reserve(map, count);
    for (usize i = 0; i < count; i++) {
        nsl_map_insert(map, items[i].hash, items[i].value);
    }
}

NSL_API void nsl_map_clear(nsl_Map* map) {
    map->len = 0;
    map->del = 0;
    memset(map->items, 0, sizeof(map->items[0]) * map->cap);
}

NSL_API void nsl_map_resize(nsl_Map *map, usize size) {
    if (size < map->cap) {
        return;
    }
    usize old_cap = map->cap;
    nsl_MapItem *old_items = map->items;

    map->cap = size == 0 ? NSL_MAP_DEFAULT_SIZE : nsl_usize_next_pow2(size);
    map->items = nsl_arena_calloc_chunk(map->arena, map->cap * sizeof(map->items[0]));

    map->len = 0;
    map->del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i].hash && old_items[i].hash != NSL_MAP_DELETED) {
            nsl_map_insert(map, old_items[i].hash, old_items[i].value);
        }
    }
    nsl_arena_free_chunk(map->arena, old_items);
}

NSL_API void nsl_map_reserve(nsl_Map *map, usize size) {
    usize target = map->len + size;
    if (target <= map->cap) return;
    nsl_map_resize(map, target);
}

NSL_API bool nsl_map_remove(nsl_Map *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }

    // NOTE: rehash in the slight chance that the hash is 0 or NSL_MAP_DELETED
    if (NSL_UNLIKELY(hash == 0 || hash == NSL_MAP_DELETED)) {
        hash = nsl_u64_hash(hash);
    }

    usize idx = hash & (map->cap - 1);
    for (usize i = 0; i < map->cap; i++) {
        if (map->items[idx].hash == 0) {
            return false;
        }
        if (map->items[idx].hash && map->items[idx].hash == hash) {
            map->items[idx].hash = NSL_MAP_DELETED;
            map->len--;
            map->del++;
            return true;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }
    return false;
}

NSL_API void nsl_map_insert_i64(nsl_Map *map, u64 hash, i64 value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_I64);
    nsl_map_insert(map, hash, (nsl_MapValue){.i64 = value});
}

NSL_API void nsl_map_insert_u64(nsl_Map *map, u64 hash, u64 value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_U64);
    nsl_map_insert(map, hash, (nsl_MapValue){.u64 = value});
}

NSL_API void nsl_map_insert_f64(nsl_Map *map, u64 hash, f64 value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_F64);
    nsl_map_insert(map, hash, (nsl_MapValue){.f64 = value});
}

NSL_API void nsl_map_insert_ptr(nsl_Map *map, u64 hash, void *value) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_PTR);
    nsl_map_insert(map, hash, (nsl_MapValue){.ptr = value});
}

NSL_API i64 *nsl_map_get_i64(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_I64);
    return (i64 *)nsl_map_get(map, hash);
}

NSL_API u64 *nsl_map_get_u64(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_U64);
    return (u64 *)nsl_map_get(map, hash);
}

NSL_API f64 *nsl_map_get_f64(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_F64);
    return (f64 *)nsl_map_get(map, hash);
}

NSL_API void *nsl_map_get_ptr(nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_PTR);
    nsl_MapValue *value = nsl_map_get(map, hash);
    return value ? value->ptr : NULL;
}

NSL_API const i64 *nsl_map_get_i64_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_I64);
    return (const i64 *)nsl_map_get(map, hash);
}

NSL_API const u64 *nsl_map_get_u64_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_U64);
    return (const u64 *)nsl_map_get(map, hash);
}

NSL_API const f64 *nsl_map_get_f64_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_F64);
    return (const f64 *)nsl_map_get(map, hash);
}

NSL_API const void *nsl_map_get_ptr_const(const nsl_Map *map, u64 hash) {
    assert(map->type == NSL_MAP_DYNAMIC || map->type == NSL_MAP_PTR);
    nsl_MapValue *value = nsl_map_get(map, hash);
    return value ? value->ptr : NULL;
}
// src/nsl/os/windows/os.c
#if defined(_WIN32)
#error "not implemented"
#endif
// src/nsl/os/windows/fs.c
#if defined(_WIN32)


#include <io.h>
#include <string.h>

typedef struct nsl_FsNode {
    struct nsl_FsNode *next;
    HANDLE handle;
    char name[];
} nsl_FsNode;

NSL_API nsl_FsIter nsl_fs_begin(nsl_Path directory, bool recursive, nsl_Error *error) {
    nsl_FsIter it = {.recursive = recursive, .error = error};

    const usize len = directory.len + (sizeof("/*") - 1);
    const usize size = sizeof(nsl_FsNode) + len + 1;
    nsl_FsNode *node = nsl_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    nsl_Path path = nsl_path_join(2, (nsl_Path[]){directory, NSL_STR("/*")}, &it.scratch);
    WIN32_FIND_DATA findFileData;
    node->handle = FindFirstFile(path.data, &findFileData);
    if (node->handle == INVALID_HANDLE_VALUE) {
        NSL_ERROR_EMIT(it.error, GetLastError(), "FindFirstFile failed\n");
    }

    return it;
}

NSL_API void nsl_fs_end(nsl_FsIter *it) {
    while (it->_handle != NULL) {
        nsl_FsNode *current = it->_handle;
        it->_handle = current->next;
        if (current->handle != INVALID_HANDLE_VALUE) FindClose(current->handle);
    }
    nsl_arena_free(&it->scratch);
}

NSL_API nsl_FsEntry* nsl_fs_next(nsl_FsIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nsl_arena_reset(&it->scratch);
        nsl_FsNode *current = it->_handle;

        WIN32_FIND_DATA findFileData;
        if (!FindNextFile(current->handle, &findFileData)) {
            FindClose(current->handle);
            it->_handle = current->next;
            nsl_arena_free_chunk(&it->scratch, current);
            continue;
        }

        // skip "." and ".." directories
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        nsl_FsEntry *e = nsl_arena_alloc(&it->scratch, sizeof(nsl_FsEntry));
        nsl_Path parts[] = {
            nsl_str_from_cstr(current->name),
            nsl_str_from_cstr(findFileData.cFileName),
        };
        e->path = nsl_path_join(NSL_ARRAY_LEN(parts), parts, &it->scratch);

        e->is_dir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e->size = ((u64)findFileData.nFileSizeHigh << 32) | findFileData.nFileSizeLow;
        e->mtime = ((u64)findFileData.ftLastWriteTime.dwHighDateTime << 32) | findFileData.ftLastWriteTime.dwLowDateTime;

        if (e->is_dir && it->recursive) {
            nsl_Path path = nsl_path_join(2, (nsl_Path[]){e->path, NSL_STR("/*")}, &it->scratch);
            HANDLE handle = FindFirstFile(path.data, &findFileData);
            if (handle == INVALID_HANDLE_VALUE) {
                continue;
            }

            const usize size = sizeof(nsl_FsNode) + e->path.len + 1;
            nsl_FsNode *node = nsl_arena_calloc_chunk(&it->scratch, size);
            memcpy(node->name, e->path.data, e->path.len);

            node->handle = handle;
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }

    return NULL;
}
#endif
// src/nsl/os/windows/cmd.c
#if defined(_WIN32)

#include <windows.h>

static void _nc_cmd_win32_wrap(usize argc, const char **argv, nsl_StrBuilder *sb) {
    // https://github.com/tsoding/nob.h/blob/45fa6efcd3e105bb4e39fa4cb9b57c19690d00a2/nob.h#L893
    for (usize i = 0; i < argc; i++) {
        if (0 < i) nsl_list_push(sb, ' ');
        const char *arg = argv[i];

        nsl_list_push(sb, '\"');
        usize backslashes = 0;
        while (*arg) {
            char c = *arg;
            if (c == '\\') {
                backslashes += 1;
            } else {
                if (c == '\"') {
                    for (size_t k = 0; k < 1 + backslashes; k++) {
                        nsl_list_push(sb, '\\');
                    }
                }
                backslashes = 0;
            }
            nsl_list_push(sb, c);
            arg++;
        }

        for (usize k = 0; k < backslashes; k++) {
            nsl_list_push(sb, '\\');
        }

        nsl_list_push(sb, '\"');
    }
}

NSL_API nsl_CmdError nsl_cmd_exec(size_t argc, const char **argv) {
    if (argc == 0) return NSL_CMD_NOT_FOUND;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD result = 0;

    nsl_Arena arena = {0};

    nsl_StrBuilder sb = {0};
    nsl_list_init(&sb, &arena);

    _nc_cmd_win32_wrap(argc, argv, &sb);
    nsl_list_push(&sb, '\0');

    if (!CreateProcessA(NULL, sb.items, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        DWORD ec = GetLastError();
        if (ec == ERROR_FILE_NOT_FOUND || ec == ERROR_PATH_NOT_FOUND) NSL_DEFER(NSL_CMD_NOT_FOUND);

        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
        char msg[512] = {0};
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ec,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer, (DWORD)sizeof(msg), NULL
        );
        NSL_PANIC(msg);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
defer:
    nsl_arena_free(&arena);
    return (nsl_CmdError)result;
}


#endif
// src/nsl/os/posix/os.c
#if !defined(_WIN32)


#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

NSL_API void nsl_os_mkdir(nsl_Path path, nsl_Error *error, nsl_OsDirConfig config) {
    if (config.parents) {
        if (nsl_path_is_root(path)) return;
        if (path.len == 1 && path.data[0] == '.') return;
        config.exists_ok = true;
        nsl_os_mkdir(nsl_path_parent(path), error, config);
    }
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    if (mkdir(filepath, config.mode ? config.mode : 0755) != 0) {
        if (config.exists_ok && errno == EEXIST) {
            struct stat info;
            if (stat(filepath, &info) == 0 && S_ISDIR(info.st_mode)) return;
        }
        NSL_ERROR_EMIT(error, errno, strerror(errno));
    }
}

NSL_API void nsl_os_chdir(nsl_Path path, nsl_Error* error) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    if (chdir(filepath) != 0) {
        NSL_ERROR_EMIT(error, errno, strerror(errno));
    }
}

NSL_API nsl_Path nsl_os_cwd(nsl_Arena *arena, nsl_Error *error) {
    errno = 0;
    char *buf = nsl_arena_alloc(arena, FILENAME_MAX);
    char *ret = getcwd(buf, FILENAME_MAX - 1);
    if (ret == NULL) {
        NSL_ERROR_EMIT(error, errno, strerror(errno));
        return (nsl_Str){0};
    }
    return nsl_str_from_cstr(ret);
}

NSL_API nsl_Str nsl_os_getenv(const char *env, nsl_Error *error) {
    const char *var = getenv(env);
    if (var == NULL) {
        NSL_ERROR_EMIT(error, -1, env);
        return (nsl_Str){0};
    }
    return nsl_str_from_cstr(var);
}

#endif
// src/nsl/os/posix/fs.c
#if !defined(_WIN32)


#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

typedef struct nsl_FsNode {
    struct nsl_FsNode *next;
    DIR *handle;
    char name[];
} nsl_FsNode;

NSL_API nsl_FsIter nsl_fs_begin(nsl_Path directory, bool recursive, nsl_Error* error) {
    nsl_FsIter it = {.recursive=recursive, .error=error};

    const usize size = sizeof(nsl_FsNode) + directory.len + 1;
    nsl_FsNode* node = nsl_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    node->handle = opendir(node->name);
    if (node->handle == NULL) {
        NSL_ERROR_EMIT(it.error, errno, strerror(errno));
    }

    return it;
}

NSL_API void nsl_fs_end(nsl_FsIter *it) {
    while (it->_handle != NULL) {
        nsl_FsNode* node = it->_handle;
        if (node->handle) closedir(node->handle);
        it->_handle = node->next;
    }
    nsl_arena_free(&it->scratch);
}

NSL_API nsl_FsEntry *nsl_fs_next(nsl_FsIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nsl_arena_reset(&it->scratch);
        nsl_FsNode *current = it->_handle;

        struct dirent *entry = readdir(current->handle);
        if (entry == NULL) {
            closedir(current->handle);
            it->_handle = current->next;
            nsl_arena_free_chunk(&it->scratch, current);
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        nsl_FsEntry *e = nsl_arena_alloc(&it->scratch, sizeof(nsl_FsEntry));
        nsl_Path parts[] = {
            nsl_str_from_cstr(current->name), nsl_str_from_cstr(entry->d_name),
        };
        e->path = nsl_path_join(NSL_ARRAY_LEN(parts), parts, &it->scratch);

        struct stat entry_info;
        if (stat(e->path.data, &entry_info) == -1) continue;

        e->is_dir = S_ISDIR(entry_info.st_mode);
        e->size = (usize)entry_info.st_size;
        e->mtime = (u64)entry_info.st_mtime;

        if (it->recursive && e->is_dir) {
            DIR *handle = opendir(e->path.data);
            if (handle == NULL) continue;
            const usize size = sizeof(nsl_FsNode) + e->path.len + 1;
            nsl_FsNode *node = nsl_arena_calloc_chunk(&it->scratch, size);
            node->handle = handle;
            memcpy(node->name, e->path.data, e->path.len);
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }
    return NULL;
}
#endif
// src/nsl/os/posix/cmd.c
#if !defined(_WIN32)


#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

NSL_API nsl_CmdError nsl_cmd_exec(size_t argc, const char **argv) {
    if (argc == 0) return NSL_CMD_NOT_FOUND;

    errno = 0;
    pid_t pid = fork();
    if (pid == -1) {
        NSL_PANIC("fork failed");
    } else if (pid == 0) {
        nsl_Arena arena = {0};

        nsl_List(const char *) args = {0};
        nsl_list_init(&args, &arena);

        nsl_list_extend(&args, argc, argv);
        nsl_list_push(&args, NULL);
        execvp(args.items[0], (char *const *)(void *)args.items);

        nsl_arena_free(&arena);
        exit(127);
    }

    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        nsl_CmdError exit_code = WEXITSTATUS(status);
        return exit_code == 127 ? NSL_CMD_NOT_FOUND : exit_code;
    }

    return NSL_CMD_OK;
}
#endif
// src/nsl/os/fs.c


#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
#    include <direct.h>
#    include <io.h>
#    define stat _stat
#    define access(path, mode) _access(path, mode)
#    define unlink(path) _unlink(path)
#else
#    include <unistd.h>
#endif

NSL_API bool nsl_fs_exists(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    return access(filepath, 0) == 0;
}

NSL_API bool nsl_fs_is_dir(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));

    struct stat info;
    if (stat(filepath, &info) == -1) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}

NSL_API bool nsl_fs_remove(nsl_Path path) {
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));
    return (unlink(filepath) != 0);
}
// src/nsl/os/file.c


#include <errno.h>
#include <string.h>
#include <stdarg.h>


NSL_API FILE *nsl_file_open(nsl_Path path, const char *mode, nsl_Error *error) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));

    FILE *file = fopen(filepath, mode);
    if (file == NULL) NSL_ERROR_EMIT(error, errno, strerror(errno));
    return file;
}

NSL_API void nsl_file_close(FILE *file) {
    fclose(file);
}

NSL_API void nsl_file_check_error(FILE* file, nsl_Error* error) {
    if (ferror(file) != 0) NSL_ERROR_EMIT(error, errno, strerror(errno));
}

NSL_API usize nsl_file_size(FILE* file) {
    long current = ftell(file);
    fseek(file, 0, SEEK_END);
    usize size = (usize)ftell(file);
    fseek(file, current, SEEK_SET);
    return size;
}

NSL_API nsl_Str nsl_file_read_str(FILE* file, nsl_Arena* arena) {
    usize size = nsl_file_size(file);
    char* data = nsl_arena_calloc(arena, size+1);
    size = fread(data, 1, size, file);
    return nsl_str_from_parts(size, data);
}

NSL_API nsl_Str nsl_file_read_sb(FILE* file, nsl_StrBuilder* sb) {
    usize size = nsl_file_size(file);
    nsl_list_reserve(sb, size);
    char* start = &nsl_list_last(sb);
    size = fread(start, 1, size, file);
    sb->len += size;
    return nsl_str_from_parts(size, start);
}

NSL_API nsl_Str nsl_file_read_line(FILE* file, nsl_StrBuilder* sb) {
    usize off = sb->len;
    i32 c = 0;
    while (!feof(file) && c != '\n') {
        c = fgetc(file);
        nsl_list_push(sb, (char)c);
    }
    return nsl_str_from_parts(sb->len - off, &sb->items[off]);
}

NSL_API nsl_Bytes nsl_file_read_bytes(FILE* file, usize size, u8* buffer) {
    size = fread(buffer, 1, size, file);
    return nsl_bytes_from_parts(size, buffer);
}

NSL_API NSL_FMT(2) void nsl_file_write_fmt(FILE* file, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(file, fmt, va);
    va_end(va);
}

NSL_API void nsl_file_write_str(FILE* file, nsl_Str content) {
    fwrite(content.data, 1, content.len, file);
}

NSL_API void nsl_file_write_bytes(FILE* file, nsl_Bytes content) {
    fwrite(content.data, 1, content.size, file);
}
// src/nsl/os/cmd.c

NSL_API nsl_CmdError nsl_cmd_exec_list(const nsl_Cmd *cmd) {
    return nsl_cmd_exec(cmd->len, cmd->items);
}

// src/nsl/core/arena.c

#include <stdlib.h>
#include <string.h>

// 4 kb
#define CHUNK_DEFAULT_SIZE 4000

struct nsl_Chunk {
    nsl_Chunk *next, *prev;
    usize cap;
    usize allocated;
    u8 data[];
};

static nsl_Chunk *chunk_allocate(usize size) {
    nsl_Chunk *chunk = malloc(sizeof(nsl_Chunk) + size);
    NSL_ASSERT(chunk != NULL && "Memory allocation failed");
    chunk->cap = size;
    chunk->allocated = 0;
    chunk->next = chunk->prev = 0;
    return chunk;
}

static void chunk_free(nsl_Chunk *chunk) { 
    free(chunk); 
}

NSL_CONST_FN static usize align(usize size) {
    const usize mask = sizeof(void *) - 1;
    return (size + mask) & ~mask;
}

NSL_API void nsl_arena_free(nsl_Arena *arena) {
    nsl_Chunk *next = arena->begin;
    while (next != NULL) {
        nsl_Chunk *temp = next;
        next = next->next;
        chunk_free(temp);
    }
    arena->begin = NULL;
}

NSL_API void nsl_arena_reset(nsl_Arena *arena) {
    for (nsl_Chunk *next = arena->begin; next != NULL; next = next->next) {
        if (next->cap != 0) {
            next->allocated = 0;
        }
    }
}

NSL_API usize nsl_arena_size(nsl_Arena *arena) {
    usize size = 0;
    for (nsl_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->allocated;
    }
    return size;
}

NSL_API usize nsl_arena_real_size(nsl_Arena *arena) {
    usize size = 0;
    for (nsl_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->cap ? chunk->cap : chunk->allocated;
    }
    return size;
}

NSL_API void *nsl_arena_alloc(nsl_Arena *arena, usize size) {
    size = align(size);
    nsl_Chunk *chunk = arena->begin;
    for (; chunk != NULL; chunk = chunk->next) {
        NSL_ASSERT(size <= SIZE_MAX - chunk->allocated && "integer overflow");
        if (chunk->allocated + size < chunk->cap) {
            break;
        }
    }
    if (chunk == NULL) {
        const usize chunk_size =
            size >= CHUNK_DEFAULT_SIZE ? size : CHUNK_DEFAULT_SIZE;
        chunk = chunk_allocate(chunk_size);
        chunk->next = arena->begin;
        if (arena->begin) {
            arena->begin->prev = chunk;
        }
        arena->begin = chunk;
    }
    void *ptr = &chunk->data[chunk->allocated];
    chunk->allocated += size;
    return ptr;
}

NSL_API void *nsl_arena_calloc(nsl_Arena *arena, usize size) {
    void *ptr = nsl_arena_alloc(arena, size);
    memset(ptr, 0, size);
    return ptr;
}

NSL_API void *nsl_arena_alloc_chunk(nsl_Arena *arena, usize size) {
    nsl_Chunk *chunk = chunk_allocate(size);
    chunk->cap = 0;
    chunk->allocated = size;
    chunk->next = arena->begin;
    if (arena->begin) {
        arena->begin->prev = chunk;
    }
    arena->begin = chunk;
    return chunk->data;
}

NSL_API void *nsl_arena_calloc_chunk(nsl_Arena *arena, usize size) {
    void *data = nsl_arena_alloc_chunk(arena, size);
    memset(data, 0, size);
    return data;
}

NSL_API void *nsl_arena_realloc_chunk(nsl_Arena *arena, void *ptr, usize size) {
    if (ptr == NULL) {
        return nsl_arena_alloc_chunk(arena, size);
    }
    nsl_Chunk *chunk = (nsl_Chunk *)((usize)ptr - sizeof(nsl_Chunk));
    if (size < chunk->allocated) {
        return chunk->data;
    }
    nsl_Chunk *new_chunk = realloc(chunk, sizeof(nsl_Chunk) + size);
    if (new_chunk->prev) {
        new_chunk->prev->next = new_chunk;
    }
    if (new_chunk->next) {
        new_chunk->next->prev = new_chunk;
    }
    if (arena->begin == chunk) {
        arena->begin = new_chunk;
    }
    return new_chunk->data;
}

NSL_API void nsl_arena_free_chunk(nsl_Arena *arena, void *ptr) {
    if (ptr == NULL) {
        return;
    }
    nsl_Chunk *chunk = (nsl_Chunk *)((usize)ptr - sizeof(nsl_Chunk));
    if (chunk == arena->begin) {
        arena->begin = chunk->next;
    }
    if (chunk->prev) {
        chunk->prev->next = chunk->next;
    }
    if (chunk->next) {
        chunk->next->prev = chunk->prev;
    }
    free(chunk);
}
#endif // NSL_IMPLEMENTATION
