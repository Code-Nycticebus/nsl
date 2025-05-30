#ifndef _NC_H_
#define _NC_H_

#ifndef _NC_DEFINES_H_
#define _NC_DEFINES_H_
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef NC_NO_INT_TYPEDEFS
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
#ifndef NC_ASSERT
#    include <assert.h>
#    define NC_ASSERT assert
#endif
#define BOOL_FMT "%s"
#define BOOL_ARG(b) (b ? "true" : "false")
#define NC_ARRAY_LEN(...) (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))
#define NC_STR(str) ((nc_Str){.len = sizeof(str) - 1, .data = (str)})
#define NC_STR_STATIC(str) { .len = sizeof(str) - 1, .data = (str) }
#define NC_STR_FMT "%.*s"
#define NC_STR_REPR "'%.*s'"
#define NC_STR_ARG(str) (i32)(str).len, (str).data
typedef struct {
    usize len;
    const char *data;
} nc_Str;
#define NC_BYTES(...)                                                                              \
    (nc_Bytes) {                                                                                   \
        .size = sizeof((const u8[]){__VA_ARGS__}), .data = (const u8[]){__VA_ARGS__},              \
    }
#define NC_BYTES_STR(s)                                                                            \
    (nc_Bytes) {                                                                                   \
        .size = sizeof(s) - 1, .data = (const u8 *)(s),                                            \
    }
typedef struct {
    usize size;
    const u8 *data;
} nc_Bytes;
typedef nc_Str nc_Path;
#define NC_PATH(cstr) ((nc_Path){.len = sizeof(cstr) - 1, .data = (cstr)})
typedef struct nc_Chunk nc_Chunk;
typedef struct {
    nc_Chunk *begin, *end;
} nc_Arena;
typedef struct {
    i64 code;            // 0 = no error
    const char *file;    // __FILE__ where error occured
    i32 line;            // __LINE__ where error occured
    const char *func;    // __func__ where error occured
    const char *message; // human-readable message
} nc_Error;
#define NC_UNUSED(v) (void)(v)
#define NC_PASS ((void)(0))
#define NC_UNREACHABLE(msg) do { fprintf(stderr, "Unreachable: %s:%d: %s", __FILE__, __LINE__, msg); abort(); } while(0)
#define NC_TODO(msg) do { fprintf(stderr, "TODO: %s:%d: %s", __FILE__, __LINE__, msg); abort(); } while(0)
#define NC_NOT_IMPLEMENTED(msg) do { fprintf(stderr, "Not Implemented: %s:%d: %s", __FILE__, __LINE__, msg); abort(); } while(0)
#if defined(__clang__)
#    define NC_COMPILER_CLANG 1
#    define NC_COMPILER_NAME "clang"
#elif defined(__GNUC__) && !defined(__clang__)
#    define NC_COMPILER_GCC 1
#    define NC_COMPILER_NAME "gcc"
#elif defined(__TINYC__)
#    define NC_COMPILER_TCC 1
#    define NC_COMPILER_NAME "tcc"
#elif defined(_MSC_VER)
#    define NC_COMPILER_MSVC 1
#    define NC_COMPILER_NAME "cl"
#elif defined(__MINGW32__)
#    define NC_COMPILER_MINGW32 1
#    define NC_COMPILER_NAME "mingw32"
#elif defined(__MINGW64__)
#    define NC_COMPILER_MINGW64 1
#    define NC_COMPILER_NAME "mingw64"
#else
#    define NC_COMPILER_UNKNOWN 1
#    define NC_COMPILER_NAME "unknown"
#endif
#if defined(NC_COMPILER_GCC) || defined(NC_COMPILER_CLANG)
#    define NC_EXPORT __attribute__((used))
#    define NC_NORETURN __attribute__((noreturn))
#    define NC_PURE_FN __attribute__((pure)) __attribute__((warn_unused_result))
#    define NC_CONST_FN __attribute__((const)) __attribute__((warn_unused_result))
#    define NC_LIKELY(exp) __builtin_expect(!!(exp), 1)
#    define NC_UNLIKELY(exp) __builtin_expect(!!(exp), 0)
#    define NC_FMT(fmt_idx) __attribute__((format(printf, fmt_idx, fmt_idx + 1)))
#elif defined(NC_COMPILER_MSVC)
#    include <sal.h>
#    define NC_EXPORT __declspec(dllexport)
#    define NC_NORETURN __declspec(noreturn)
#    define NC_PURE_FN _Check_return_
#    define NC_CONST_FN _Check_return_
#    define NC_LIKELY(exp) (exp)
#    define NC_UNLIKELY(exp) (exp)
#    define NC_FMT(fmt_idx)
#else
#    define NC_EXPORT
#    define NC_NORETURN
#    define NC_UNUSED
#    define NC_PURE_FN
#    define NC_CONST_FN
#    define NC_LIKELY(exp) (exp)
#    define NC_UNLIKELY(exp) (exp)
#    define NC_FMT(fmt_idx)
#endif
#ifdef NC_STATIC
#    define NC_API static
#elif defined(_WIN32)
#    ifdef NC_BUILD
#        define NC_API __declspec(dllexport)
#    else
#        define NC_API __declspec(dllimport)
#    endif
#else
#    define NC_API __attribute__((visibility("default")))
#endif
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
#    define NC_ENDIAN_LITTLE __ORDER_LITTLE_ENDIAN__
#    define NC_ENDIAN_BIG __ORDER_BIG_ENDIAN__
#    define NC_BYTE_ORDER __BYTE_ORDER__
#elif defined(_MSC_VER)
#    define NC_ENDIAN_LITTLE 1234
#    define NC_ENDIAN_BIG 4321
#    define NC_BYTE_ORDER ENDIAN_LITTLE
#else
#    error "No Byte Order detected!"
#endif
#endif // _NC_DEFINES_H_

// include/nc/os/dirent.h
#ifndef _NC_DIR_
#define _NC_DIR_
typedef struct {
    nc_Path path;
    bool is_dir;
    usize size;
    u64 mtime;
} nc_DirEntry;
typedef struct {
    nc_Arena scratch; // per file scratch buffer
    bool recursive;   // recursive
    nc_Error *error;  // Error
    void *_handle;    // platform specific handle
} nc_DirIter;
nc_DirIter nc_dir_begin(nc_Path directory, bool recursive, nc_Error* error);
void nc_dir_end(nc_DirIter *it);
nc_DirEntry* nc_dir_next(nc_DirIter *it);
#endif // _NC_DIR_

// include/nc/core/error.h
#ifndef _NC_ERROR_H_
#define _NC_ERROR_H_
#include <stdlib.h>
#include <stdio.h>
#define NC_ERROR_FMT "%s:%d (%s): %s"
#define NC_ERROR_ARG(E) (E)->file, (E)->line, (E)->func, (E)->message
#define NC_ERROR_EMIT(E, error_code, error_message)
#define NC_ERROR_HANDLE(E, ...)
#define NC_ERROR_EXCEPT_CASE(E, CODE, ...)
#define NC_ERROR_PROPAGATE(E, ...)
#define NC_ERROR_CLEAR(E)
#define NC_ERROR_RAISE(E)
#define NC_ERROR_LOG(E)
// Implemetation
#undef NC_ERROR_EMIT
#define NC_ERROR_EMIT(E, error_code, error_message)                            \
    do {                                                                       \
        if ((E) == NULL) {                                                            \
            nc_Error _panic_error = {                                          \
                error_code, __FILE__, __LINE__, __func__, error_message,       \
            };                                                                 \
            NC_ERROR_RAISE(&_panic_error);                                     \
        } else {                                                               \
            (E)->code = error_code;                                            \
            (E)->file = __FILE__;                                              \
            (E)->line = __LINE__;                                              \
            (E)->func = __func__;                                              \
            (E)->message = error_message;                                      \
        }                                                                      \
    } while (0)
#undef NC_ERROR_HANDLE
#define NC_ERROR_HANDLE(E, ...)                                                \
    do {                                                                       \
        if ((E) != NULL && (E)->code) {                                                \
            { __VA_ARGS__; }                                                   \
            if ((E)->code) {                                                   \
                NC_ERROR_RAISE(E);                                             \
            }                                                                  \
        }                                                                      \
    } while (0)
#undef NC_ERROR_EXCEPT_CASE
#define NC_ERROR_EXCEPT_CASE(E, CODE, ...)                                     \
    case CODE: {                                                               \
        __VA_ARGS__;                                                           \
        NC_ERROR_CLEAR(E);                                                     \
        break;                                                                 \
    }
#undef NC_ERROR_PROPAGATE
#define NC_ERROR_PROPAGATE(E, ...)                                             \
    do {                                                                       \
        if ((E) != NULL && (E)->code) {                                                \
            { __VA_ARGS__; }                                                   \
        }                                                                      \
    } while (0)
#undef NC_ERROR_CLEAR
#define NC_ERROR_CLEAR(E)                                                      \
    do {                                                                       \
        *(E) = (nc_Error){0};                                                  \
    } while (0)
#undef NC_ERROR_RAISE
#define NC_ERROR_RAISE(E)                                                      \
    do {                                                                       \
        fprintf(stderr, NC_ERROR_FMT "\n", NC_ERROR_ARG(E));                   \
        abort();                                                               \
    } while (0)
#undef NC_ERROR_LOG
#define NC_ERROR_LOG(E)                                                        \
    do {                                                                       \
        fprintf(stderr, NC_ERROR_FMT "\n", NC_ERROR_ARG(E));                   \
    } while (0)
#endif // _NC_ERROR_H_

// include/nc/core/arena.h
#ifndef _NC_ARENA_H_
#define _NC_ARENA_H_
NC_API void nc_arena_free(nc_Arena *arena);
NC_API void *nc_arena_alloc(nc_Arena *arena, usize size);
NC_API void *nc_arena_calloc(nc_Arena *arena, usize size);
NC_API void nc_arena_reset(nc_Arena *arena);
NC_API usize nc_arena_size(nc_Arena *arena);
NC_API usize nc_arena_real_size(nc_Arena *arena);
////////////////////////////////////////////////////////////////////////////
NC_API void *nc_arena_alloc_chunk(nc_Arena *arena, usize size);
NC_API void *nc_arena_calloc_chunk(nc_Arena *arena, usize size);
NC_API void *nc_arena_realloc_chunk(nc_Arena *arena, void *ptr, usize size);
NC_API void nc_arena_free_chunk(nc_Arena *arena, void *ptr);
#endif // _NC_ARENA_H_

// include/nc/structs/list.h
#ifndef _NC_LIST_H_
#define _NC_LIST_H_
#include <stdlib.h>
///////////////////////////////////////////////////////////////////////////////
#define nc_List(T)                                                                                 \
    struct {                                                                                       \
        usize cap;                                                                                 \
        usize len;                                                                                 \
        nc_Arena *arena;                                                                           \
        T *items;                                                                                  \
    }
#define nc_list_first(list) (list)->items[0]
#define nc_list_last(list) (list)->items[(list)->len - 1]
#define nc_list_pop(list) (list)->items[--(list)->len]
#define nc_list_is_empty(list) (!(list)->len)
#define nc_list_clear(list) ((list)->len = 0)
///////////////////////////////////////////////////////////////////////////////
#define nc_list_init(list, _arena)                                                                 \
    do {                                                                                           \
        (list)->len = 0;                                                                           \
        (list)->cap = 0;                                                                           \
        (list)->arena = _arena;                                                                    \
        (list)->items = NULL;                                                                      \
    } while (0)
#define nc_list_copy(src, dest)                                                                    \
    do {                                                                                           \
        nc_list_resize((dest), (src)->len);                                                        \
        for (usize __c_i = 0; __c_i < (src)->len; __c_i++) {                                       \
            (dest)->items[__c_i] = (src)->items[__c_i];                                            \
        }                                                                                          \
        (dest)->len = (src)->len;                                                                  \
    } while (0)
///////////////////////////////////////////////////////////////////////////////
#define nc_list_resize(list, size)                                                                 \
    do {                                                                                           \
        if (size < (list)->cap) {                                                                  \
            break;                                                                                 \
        }                                                                                          \
        (list)->cap = size;                                                                        \
        (list)->items = nc_arena_realloc_chunk((list)->arena, (list)->items,                       \
                                               (list)->cap * sizeof(*(list)->items));              \
    } while (0)
#define nc_list_reserve(list, size)                                                                \
    do {                                                                                           \
        const usize __rs = (list)->len + size;                                                     \
        if (__rs < (list)->cap) {                                                                  \
            break;                                                                                 \
        }                                                                                          \
        usize __ns = (list)->cap == 0 ? 5 : (list)->cap;                                           \
        while (__ns < __rs) {                                                                      \
            __ns *= 2;                                                                             \
        }                                                                                          \
        nc_list_resize(list, __ns);                                                                \
    } while (0)
///////////////////////////////////////////////////////////////////////////////
#define nc_list_push(list, ...)                                                                    \
    do {                                                                                           \
        nc_list_reserve((list), 1);                                                                \
        (list)->items[(list)->len++] = (__VA_ARGS__);                                              \
    } while (0)
#define nc_list_extend(list, count, ...)                                                           \
    do {                                                                                           \
        nc_list_reserve((list), (count));                                                          \
        for (usize __e_i = 0; __e_i < (count); __e_i++) {                                          \
            (list)->items[(list)->len + __e_i] = (__VA_ARGS__)[__e_i];                             \
        }                                                                                          \
        (list)->len += count;                                                                      \
    } while (0)
#define nc_list_extend_static(list, ...)                                                           \
    do {                                                                                           \
        nc_list_extend(list, NC_ARRAY_LEN(__VA_ARGS__), __VA_ARGS__);                              \
    } while (0)
#define nc_list_extend_list(list, other)                                                             \
    do {                                                                                           \
        nc_list_reserve((list), (other)->len);                                                     \
        for (usize __e_i = 0; __e_i < (other)->len; __e_i++) {                                     \
            (list)->items[(list)->len + __e_i] = (other)->items[__e_i];                            \
        }                                                                                          \
        (list)->len += (other)->len;                                                               \
    } while (0)
#define nc_list_insert(list, value, idx)                                                           \
    do {                                                                                           \
        nc_list_reserve(list, 1);                                                                  \
        for (usize __r_i = idx + 1; __r_i < (list)->len + 1; __r_i++) {                            \
            (list)->items[__r_i] = (list)->items[__r_i - 1];                                       \
        }                                                                                          \
        (list)->items[idx] = value;                                                                \
        (list)->len++;                                                                             \
    } while (0)
#define nc_list_remove(list, idx)                                                                  \
    do {                                                                                           \
        for (usize __r_i = idx + 1; __r_i < (list)->len; __r_i++) {                                \
            (list)->items[__r_i - 1] = (list)->items[__r_i];                                       \
        }                                                                                          \
        (list)->len--;                                                                             \
    } while (0)
///////////////////////////////////////////////////////////////////////////////
#define nc_list_map(src, dest, map)                                                                \
    do {                                                                                           \
        nc_list_reserve((dest), (src)->len);                                                       \
        for (usize __m_i = 0; __m_i < (src)->len; __m_i++) {                                       \
            (dest)->items[__m_i] = map((src)->items[__m_i]);                                       \
        }                                                                                          \
        (dest)->len = (src)->len;                                                                  \
    } while (0)
#define nc_list_map_ctx(src, dest, map, ctx)                                                       \
    do {                                                                                           \
        nc_list_reserve((dest), (src)->len);                                                       \
        for (usize __m_i = 0; __m_i < (src)->len; __m_i++) {                                       \
            (dest)->items[__m_i] = map(ctx, (src)->items[__m_i]);                                  \
        }                                                                                          \
        (dest)->len = (src)->len;                                                                  \
    } while (0)
#define nc_list_filter(src, dest, filter)                                                          \
    do {                                                                                           \
        nc_list_reserve((dest), (src)->len);                                                       \
        usize __f_count = 0;                                                                       \
        for (usize __f_i = 0; __f_i < (src)->len; __f_i++) {                                       \
            if (filter((src)->items[__f_i])) {                                                     \
                (dest)->items[__f_count++] = (src)->items[__f_i];                                  \
            }                                                                                      \
        }                                                                                          \
        (dest)->len = __f_count;                                                                   \
    } while (0)
#define nc_list_filter_ctx(src, dest, filter, ctx)                                                 \
    do {                                                                                           \
        nc_list_reserve((dest), (src)->len);                                                       \
        usize __f_count = 0;                                                                       \
        for (usize __f_i = 0; __f_i < (src)->len; __f_i++) {                                       \
            if (filter((ctx), (src)->items[__f_i])) {                                              \
                (dest)->items[__f_count++] = (src)->items[__f_i];                                  \
            }                                                                                      \
        }                                                                                          \
        (dest)->len = __f_count;                                                                   \
    } while (0)
#define nc_list_sort(src, sort) qsort((src)->items, (src)->len, sizeof((src)->items[0]), sort)
#define nc_list_reverse(list)                                                                      \
    do {                                                                                           \
        nc_list_reserve((list), 1);                                                                \
        for (usize __r_i = 0; __r_i < (list)->len - __r_i - 1; __r_i++) {                          \
            usize __last_idx = (list)->len - __r_i - 1;                                            \
            (list)->items[(list)->len] = (list)->items[__r_i];                                     \
            (list)->items[__r_i] = (list)->items[__last_idx];                                      \
            (list)->items[__last_idx] = (list)->items[(list)->len];                                \
        }                                                                                          \
    } while (0)
#define nc_list_for_each(T, iter, da)                                                              \
    for (T iter = &nc_list_first(da); iter <= &nc_list_last(da); iter++)
#endif // _NC_LIST_H_

// include/nc/structs/set.h
#ifndef _NC_SET_H_
#define _NC_SET_H_ 
typedef struct {
    usize len;
    usize _cap;
    usize _del;
    nc_Arena *arena;
    u64 *items;
} nc_Set;
#define NC_SET_DEFAULT_SIZE 8
#define NC_SET_DELETED ((u64)0xdeaddeaddeaddead)
NC_API void nc_set_init(nc_Set* set, nc_Arena *arena);
NC_API void nc_set_resize(nc_Set *set, usize size);
NC_API void nc_set_reserve(nc_Set *set, usize size);
NC_API bool nc_set_remove(nc_Set* set, u64 hash);
NC_API bool nc_set_add(nc_Set *set, u64 hash);
NC_API bool nc_set_has(const nc_Set *set, u64 hash);
NC_API void nc_set_update(nc_Set* set, const nc_Set* other);
NC_API void nc_set_extend(nc_Set* set, usize count, const u64* hashes);
NC_API bool nc_set_eq(const nc_Set* set, const nc_Set* other);
NC_API bool nc_set_subset(const nc_Set* set, const nc_Set* other);
NC_API bool nc_set_disjoint(const nc_Set* set, const nc_Set* other);
NC_API void nc_set_intersection(const nc_Set *set, const nc_Set *other, nc_Set* out);
NC_API void nc_set_difference(const nc_Set *set, const nc_Set *other, nc_Set* out);
NC_API void nc_set_union(const nc_Set *set, const nc_Set *other, nc_Set* out);
#endif // _NC_SET_H_

// include/nc/structs/map.h
#ifndef _NC_MAP_H_
#define _NC_MAP_H_
typedef enum {
    NC_MAP_DYNAMIC,
    NC_MAP_I64,
    NC_MAP_U64,
    NC_MAP_F64,
    NC_MAP_PTR,
} nc_MapType;
typedef union {
    i64 i64;
    u64 u64;
    f64 f64;
    void* ptr;
    const void* const_ptr;
} nc_MapValue;
typedef struct {
    u64 hash;
    nc_MapValue value;
} nc_MapItem;
typedef struct nc_Map {
    nc_MapType type;
    usize len;
    usize cap;
    usize del;
    nc_Arena *arena;
    nc_MapItem *items;
} nc_Map;
#define NC_MAP_DEFAULT_SIZE 8
#define NC_MAP_DELETED ((u64)0xdeaddeaddeaddead)
NC_API void nc_map_init(nc_Map *map, nc_Arena *arena);
NC_API void nc_map_update(nc_Map *map, nc_Map *other);
NC_API void nc_map_extend(nc_Map *map, usize count, nc_MapItem *items);
NC_API void nc_map_clear(nc_Map *map);
NC_API void nc_map_resize(nc_Map *map, usize size);
NC_API void nc_map_reserve(nc_Map *map, usize size);
NC_API bool nc_map_remove(nc_Map *map, u64 hash);
NC_API void nc_map_insert_i64(nc_Map *map, u64 hash, i64 value);
NC_API void nc_map_insert_u64(nc_Map *map, u64 hash, u64 value);
NC_API void nc_map_insert_f64(nc_Map *map, u64 hash, f64 value);
NC_API void nc_map_insert_ptr(nc_Map *map, u64 hash, void *value);
NC_API i64 *nc_map_get_i64(nc_Map *map, u64 hash);
NC_API u64 *nc_map_get_u64(nc_Map *map, u64 hash);
NC_API f64 *nc_map_get_f64(nc_Map *map, u64 hash);
NC_API void *nc_map_get_ptr(nc_Map *map, u64 hash);
NC_API const i64 *nc_map_get_i64_const(const nc_Map *map, u64 hash);
NC_API const u64 *nc_map_get_u64_const(const nc_Map *map, u64 hash);
NC_API const f64 *nc_map_get_f64_const(const nc_Map *map, u64 hash);
NC_API const void *nc_map_get_ptr_const(const nc_Map *map, u64 hash);
#endif // _NC_MAP_H_

// include/nc/types/str.h
#ifndef _NC_STR_H
#define _NC_STR_H
///////////////////////////////////////////////////////////////////////////////
#define NC_STR_NOT_FOUND SIZE_MAX
#define NC_STR_LETTERS NC_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define NC_STR_UPPERCASE NC_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define NC_STR_LOWERCASE NC_STR("abcdefghijklmnopqrstuvwxyz")
#define NC_STR_DIGITS NC_STR("0123456789")
#define NC_STR_HEXDIGITS NC_STR("0123456789abcdefABCDEF")
#define NC_STR_PUNCTUATION NC_STR("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~")
#define NC_STR_WHITESPACE NC_STR(" \t\n\r\x0b\x0c")
NC_API nc_Str nc_str_from_parts(usize size, const char *cstr);
NC_API nc_Str nc_str_from_bytes(nc_Bytes bytes);
NC_API nc_Bytes nc_str_to_bytes(nc_Str s);
NC_API nc_Str nc_str_from_cstr(const char *cstr);
NC_FMT(2) nc_Str nc_str_format(nc_Arena *arena, const char *fmt, ...);
NC_API nc_Str nc_str_copy(nc_Str s, nc_Arena *arena);
NC_API nc_Str nc_str_append(nc_Str s1, nc_Str suffix, nc_Arena *arena);
NC_API nc_Str nc_str_prepend(nc_Str s1, nc_Str prefix, nc_Arena *arena);
NC_API nc_Str nc_str_wrap(nc_Str s, nc_Str wrap, nc_Arena *arena);
// Inserts sep in between elements
NC_API nc_Str nc_str_join(nc_Str sep, usize count, nc_Str *s, nc_Arena *arena);
// Appends suffix to every element, even the last one
NC_API nc_Str nc_str_join_suffix(nc_Str suffix, usize count, nc_Str *s, nc_Arena *arena);
// Prepends prefix to every element, even the last one
NC_API nc_Str nc_str_join_prefix(nc_Str prefix, usize count, nc_Str *s, nc_Arena *arena);
NC_API nc_Str nc_str_join_wrap(nc_Str sep, nc_Str wrap, usize count, nc_Str *s, nc_Arena *arena);
NC_API nc_Str nc_str_upper(nc_Str s, nc_Arena *arena);
NC_API nc_Str nc_str_lower(nc_Str s, nc_Arena *arena);
NC_API nc_Str nc_str_replace(nc_Str s, nc_Str old, nc_Str new, nc_Arena *arena);
NC_API nc_Str nc_str_center(nc_Str s, usize width, char fillchar, nc_Arena *arena);
NC_API nc_Str nc_str_ljust(nc_Str s, usize width, char fillchar, nc_Arena *arena);
NC_API nc_Str nc_str_rjust(nc_Str s, usize width, char fillchar, nc_Arena *arena);
NC_API nc_Str nc_str_repeat(nc_Str s, usize count, nc_Arena *arena);
NC_API nc_Str nc_str_reverse(nc_Str s, nc_Arena *arena);
NC_API bool nc_str_eq(nc_Str s1, nc_Str s2);
NC_API bool nc_str_eq_ignorecase(nc_Str s1, nc_Str s2);
NC_API bool nc_str_startswith(nc_Str s1, nc_Str prefix);
NC_API bool nc_str_endswith(nc_Str s1, nc_Str suffix);
NC_API bool nc_str_endswith_predicate(nc_Str s1, bool (*predicate)(char));
NC_API bool nc_str_contains(nc_Str haystack, nc_Str needle);
NC_API bool nc_str_includes(nc_Str haystack, char needle);
NC_API bool nc_str_is_empty(nc_Str s);
NC_API nc_Str nc_str_trim_left(nc_Str s);
NC_API nc_Str nc_str_trim_left_by_delim(nc_Str s, char delim);
NC_API nc_Str nc_str_trim_left_by_predicate(nc_Str s, bool (*predicate)(char));
NC_API nc_Str nc_str_trim_right(nc_Str s);
NC_API nc_Str nc_str_trim_right_by_delim(nc_Str s, char delim);
NC_API nc_Str nc_str_trim_right_by_predicate(nc_Str s, bool (*predicate)(char));
NC_API nc_Str nc_str_trim(nc_Str s);
NC_API nc_Str nc_str_trim_by_delim(nc_Str s, char delim);
NC_API nc_Str nc_str_trim_by_predicate(nc_Str s, bool (*predicate)(char));
NC_API bool nc_str_try_chop_by_delim(nc_Str *s, char delim, nc_Str *chunk);
NC_API nc_Str nc_str_chop_by_delim(nc_Str *s, char delim);
NC_API bool nc_str_try_chop_by_predicate(nc_Str *s, bool (*predicate)(char), nc_Str *chunk);
NC_API nc_Str nc_str_chop_by_predicate(nc_Str *s, bool (*predicate)(char));
NC_API nc_Str nc_str_chop_right_by_delim(nc_Str *s, char delim);
NC_API nc_Str nc_str_chop_right_by_predicate(nc_Str *s, bool (*predicate)(char));
NC_API nc_Str nc_str_take(nc_Str *s, usize count);
NC_API bool nc_str_try_take(nc_Str *s, usize count, nc_Str *chunk);
NC_API nc_Str nc_str_take_right(nc_Str *s, usize count);
NC_API bool nc_str_try_take_right(nc_Str *s, usize count, nc_Str *chunk);
NC_API nc_Str nc_str_substring(nc_Str s, usize start, usize end);
NC_API u64 nc_str_u64(nc_Str s);
NC_API u64 nc_str_chop_u64(nc_Str *s);
NC_API i64 nc_str_i64(nc_Str s);
NC_API i64 nc_str_chop_i64(nc_Str *s);
NC_API f64 nc_str_f64(nc_Str s);
NC_API f64 nc_str_chop_f64(nc_Str *s);
// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NC_API usize nc_str_find(nc_Str haystack, nc_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NC_API usize nc_str_find_by_predicate(nc_Str haystack, bool (*predicate)(char));
// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NC_API usize nc_str_find_last(nc_Str haystack, nc_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NC_API usize nc_str_find_last_by_predicate(nc_Str haystack, bool (*predicate)(char));
NC_API usize nc_str_count(nc_Str haystack, nc_Str needle);
// Returns '\0' if the index is out of bounds.
NC_API char nc_str_getc(nc_Str s, usize idx);
// Basic FNV hash.
NC_API u64 nc_str_hash(nc_Str s);
#endif // _NC_STR_H

// include/nc/types/char.h
#ifndef _NC_CHAR_H_
#define _NC_CHAR_H_
NC_API NC_CONST_FN bool nc_char_is_alnum(char c);
NC_API NC_CONST_FN bool nc_char_is_alpha(char c);
NC_API NC_CONST_FN bool nc_char_is_lower(char c);
NC_API NC_CONST_FN bool nc_char_is_upper(char c);
NC_API NC_CONST_FN bool nc_char_is_space(char c);
NC_API NC_CONST_FN bool nc_char_is_cntrl(char c);
NC_API NC_CONST_FN bool nc_char_is_print(char c);
NC_API NC_CONST_FN bool nc_char_is_graph(char c);
NC_API NC_CONST_FN bool nc_char_is_blank(char c);
NC_API NC_CONST_FN bool nc_char_is_punct(char c);
NC_API NC_CONST_FN bool nc_char_is_digit(char c);
NC_API NC_CONST_FN bool nc_char_is_xdigit(char c);
NC_API NC_CONST_FN bool nc_char_is_path_delimiter(char c);
NC_API NC_CONST_FN char nc_char_to_lower(char c);
NC_API NC_CONST_FN char nc_char_to_upper(char c);
NC_API NC_CONST_FN u8 nc_char_to_u8(char c);
NC_API NC_CONST_FN u8 nc_char_hex_to_u8(char c);
NC_API NC_CONST_FN char nc_char_from_u8(u8 d);
NC_API NC_CONST_FN char nc_char_hex_from_u8(u8 d);
NC_API NC_CONST_FN char nc_char_HEX_from_u8(u8 d);
#endif // _NC_CHAR_H_

// include/nc/types/path.h
#ifndef _NC_PATH_H_
#define _NC_PATH_H_ 
NC_API nc_Path nc_path_join(usize len, const nc_Path* parts, nc_Arena* arena);
NC_API nc_Path nc_path_normalize(nc_Path path, nc_Arena* arena);
NC_API bool nc_path_eq(nc_Path p1, nc_Path p2);
NC_API bool nc_path_is_absolute(nc_Path path);
NC_API nc_Str nc_path_name(nc_Path path);
NC_API nc_Str nc_path_suffix(nc_Path path);
NC_API nc_Str nc_path_stem(nc_Path path);
NC_API nc_Path nc_path_parent(nc_Path path);
#endif // _NC_PATH_H_

// include/nc/types/int.h
#ifndef _NC_INT_H_
#define _NC_INT_H_
#define INTEGER_DECL(T)                                                                            \
    NC_API NC_CONST_FN T nc_##T##_reverse_bits(T value);                                           \
    NC_API NC_CONST_FN usize nc_##T##_leading_ones(T value);                                       \
    NC_API NC_CONST_FN usize nc_##T##_trailing_ones(T value);                                      \
    NC_API NC_CONST_FN usize nc_##T##_leading_zeros(T value);                                      \
    NC_API NC_CONST_FN usize nc_##T##_trailing_zeros(T value);                                     \
    NC_API NC_CONST_FN usize nc_##T##_count_zeros(T value);                                        \
    NC_API NC_CONST_FN usize nc_##T##_count_ones(T value);                                         \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_swap_bytes(T value);                                             \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_to_be(T value);                                                  \
    NC_API NC_CONST_FN T nc_##T##_from_be(T value);                                                \
    NC_API T nc_##T##_from_be_bytes(nc_Bytes bytes);                                               \
    NC_API nc_Bytes nc_##T##_to_be_bytes(T value, nc_Arena *arena);                                \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_to_le(T value);                                                  \
    NC_API NC_CONST_FN T nc_##T##_from_le(T value);                                                \
    NC_API T nc_##T##_from_le_bytes(nc_Bytes bytes);                                               \
    NC_API nc_Bytes nc_##T##_to_le_bytes(T value, nc_Arena *arena);                                \
                                                                                                   \
    NC_API T nc_##T##_from_ne_bytes(nc_Bytes bytes);                                               \
    NC_API nc_Bytes nc_##T##_to_ne_bytes(T value, nc_Arena *arena);                                \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_max(T a, T b);                                                   \
    NC_API NC_CONST_FN T nc_##T##_min(T a, T b);                                                   \
    NC_API NC_CONST_FN T nc_##T##_clamp(T min, T max, T value);                                    \
                                                                                                   \
    NC_API NC_CONST_FN u64 nc_##T##_hash(T value);                                                 \
    NC_API void nc_##T##_swap(T *v1, T *v2);                                                       \
                                                                                                   \
    NC_API NC_CONST_FN T nc_##T##_next_pow2(T n);
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
#endif // _NC_INT_H_

// include/nc/types/byte.h
#ifndef _NC_BYTE_H_
#define _NC_BYTE_H_
NC_API nc_Bytes nc_bytes_from_parts(usize size, const void *data);
NC_API nc_Bytes nc_bytes_copy(nc_Bytes bytes, nc_Arena *arena);
NC_API nc_Bytes nc_bytes_slice(nc_Bytes bytes, usize idx1, size_t idx2);
NC_API nc_Bytes nc_bytes_take(nc_Bytes *bytes, usize count);
NC_API bool nc_bytes_eq(nc_Bytes b1, nc_Bytes b2);
NC_API u64 nc_bytes_hash(nc_Bytes bytes);
NC_API nc_Str nc_bytes_to_hex(nc_Bytes bytes, nc_Arena *arena);
NC_API nc_Bytes nc_bytes_from_hex(nc_Str s, nc_Arena *arena);
#endif // _NC_BYTE_H_

#endif // _NC_H_
#ifdef NC_IMPLEMENTATION
#if !defined(_WIN32)
// src/nc/os/dirent_posix.c
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
typedef struct nc_DirNode {
    struct nc_DirNode *next;
    DIR *handle;
    char name[];
} nc_DirNode;
nc_DirIter nc_dir_begin(nc_Path directory, bool recursive, nc_Error* error) {
    nc_DirIter it = {.recursive=recursive, .error=error};
    const usize size = sizeof(nc_DirNode) + directory.len + 1;
    nc_DirNode* node = nc_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;
    node->handle = opendir(node->name);
    if (node->handle == NULL) {
        NC_ERROR_EMIT(it.error, errno, strerror(errno));
    }
    return it;
}
void nc_dir_end(nc_DirIter *it) {
    while (it->_handle != NULL) {
        nc_DirNode* node = it->_handle;
        if (node->handle) closedir(node->handle);
        it->_handle = node->next;
    }
    nc_arena_free(&it->scratch);
}
nc_DirEntry *nc_dir_next(nc_DirIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nc_arena_reset(&it->scratch);
        nc_DirNode *current = it->_handle;
        struct dirent *entry = readdir(current->handle);
        if (entry == NULL) {
            closedir(current->handle);
            it->_handle = current->next;
            nc_arena_free_chunk(&it->scratch, current);
            continue;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        nc_DirEntry *e = nc_arena_alloc(&it->scratch, sizeof(nc_DirEntry));
        nc_Path parts[] = {
            nc_str_from_cstr(current->name), nc_str_from_cstr(entry->d_name),
        };
        e->path = nc_path_join(NC_ARRAY_LEN(parts), parts, &it->scratch);
        struct stat entry_info;
        if (stat(e->path.data, &entry_info) == -1) continue;
        e->is_dir = S_ISDIR(entry_info.st_mode);
        e->size = (usize)entry_info.st_size;
        e->mtime = (u64)entry_info.st_mtime;
        if (it->recursive && e->is_dir) {
            DIR *handle = opendir(e->path.data);
            if (handle == NULL) continue;
            const usize size = sizeof(nc_DirNode) + e->path.len + 1;
            nc_DirNode *node = nc_arena_calloc_chunk(&it->scratch, size);
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

#if defined(_WIN32)
// src/nc/os/dirent_windows.c
#include <io.h>
#include <string.h>
typedef struct nc_DirNode {
    struct nc_DirNode *next;
    HANDLE handle;
    char name[];
} nc_DirNode;
nc_DirIter nc_dir_begin(nc_Path directory, bool recursive, nc_Error *error) {
    nc_DirIter it = {.recursive = recursive, .error = error};
    const usize len = directory.len + (sizeof("/*") - 1);
    const usize size = sizeof(nc_DirNode) + len + 1;
    nc_DirNode *node = nc_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;
    nc_Path path = nc_path_join(2, (nc_Path[]){directory, NC_STR("/*")}, &it.scratch);
    WIN32_FIND_DATA findFileData;
    node->handle = FindFirstFile(path.data, &findFileData);
    if (node->handle == INVALID_HANDLE_VALUE) {
        NC_ERROR_EMIT(it.error, GetLastError(), "FindFirstFile failed\n");
    }
    return it;
}
void nc_dir_end(nc_DirIter *it) {
    while (it->_handle != NULL) {
        nc_DirNode *current = it->_handle;
        it->_handle = current->next;
        if (current->handle != INVALID_HANDLE_VALUE) FindClose(current->handle);
    }
    nc_arena_free(&it->scratch);
}
nc_DirEntry* nc_dir_next(nc_DirIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nc_arena_reset(&it->scratch);
        nc_DirNode *current = it->_handle;
        WIN32_FIND_DATA findFileData;
        if (!FindNextFile(current->handle, &findFileData)) {
            FindClose(current->handle);
            it->_handle = current->next;
            nc_arena_free_chunk(&it->scratch, current);
            continue;
        }
        // skip "." and ".." directories
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }
        nc_DirEntry *e = nc_arena_alloc(&it->scratch, sizeof(nc_DirEntry));
        nc_Path parts[] = {
            nc_str_from_cstr(current->name),
            nc_str_from_cstr(findFileData.cFileName),
        };
        e->path = nc_path_join(NC_ARRAY_LEN(parts), parts, &it->scratch);
        e->is_dir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e->size = ((uint64_t)findFileData.nFileSizeHigh << 32) | findFileData.nFileSizeLow;
        e->mtime = findFileData.ftLastWriteTime;
        if (e->is_dir && it->recursive) {
            nc_Path path = nc_path_join(2, (nc_Path[]){e->path, NC_STR("/*")}, &it->scratch);
            HANDLE handle = FindFirstFile(path.data, &findFileData);
            if (handle == INVALID_HANDLE_VALUE) {
                continue;
            }
            const usize size = sizeof(nc_DirNode) + e->path.len + 1;
            nc_DirNode *node = nc_arena_calloc_chunk(&it->scratch, size);
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

// src/nc/os/dirent.c
// The filter stuff for dirent

// src/nc/core/arena.c
#include <stdlib.h>
#include <string.h>
// 4 kb
#define CHUNK_DEFAULT_SIZE 4000
struct nc_Chunk {
    nc_Chunk *next, *prev;
    usize cap;
    usize allocated;
    u8 data[];
};
static nc_Chunk *chunk_allocate(usize size) {
    nc_Chunk *chunk = malloc(sizeof(nc_Chunk) + size);
    NC_ASSERT(chunk != NULL && "Memory allocation failed");
    chunk->cap = size;
    chunk->allocated = 0;
    chunk->next = chunk->prev = 0;
    return chunk;
}
static void chunk_free(nc_Chunk *chunk) { 
    free(chunk); 
}
NC_CONST_FN static usize align(usize size) {
    const usize mask = sizeof(void *) - 1;
    return (size + mask) & ~mask;
}
NC_API void nc_arena_free(nc_Arena *arena) {
    nc_Chunk *next = arena->begin;
    while (next != NULL) {
        nc_Chunk *temp = next;
        next = next->next;
        chunk_free(temp);
    }
    arena->begin = NULL;
}
NC_API void nc_arena_reset(nc_Arena *arena) {
    for (nc_Chunk *next = arena->begin; next != NULL; next = next->next) {
        if (next->cap != 0) {
            next->allocated = 0;
        }
    }
}
NC_API usize nc_arena_size(nc_Arena *arena) {
    usize size = 0;
    for (nc_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->allocated;
    }
    return size;
}
NC_API usize nc_arena_real_size(nc_Arena *arena) {
    usize size = 0;
    for (nc_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->cap ? chunk->cap : chunk->allocated;
    }
    return size;
}
NC_API void *nc_arena_alloc(nc_Arena *arena, usize size) {
    size = align(size);
    nc_Chunk *chunk = arena->begin;
    for (; chunk != NULL; chunk = chunk->next) {
        NC_ASSERT(size <= SIZE_MAX - chunk->allocated && "integer overflow");
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
NC_API void *nc_arena_calloc(nc_Arena *arena, usize size) {
    void *ptr = nc_arena_alloc(arena, size);
    memset(ptr, 0, size);
    return ptr;
}
NC_API void *nc_arena_alloc_chunk(nc_Arena *arena, usize size) {
    nc_Chunk *chunk = chunk_allocate(size);
    chunk->cap = 0;
    chunk->allocated = size;
    chunk->next = arena->begin;
    if (arena->begin) {
        arena->begin->prev = chunk;
    }
    arena->begin = chunk;
    return chunk->data;
}
NC_API void *nc_arena_calloc_chunk(nc_Arena *arena, usize size) {
    void *data = nc_arena_alloc_chunk(arena, size);
    memset(data, 0, size);
    return data;
}
NC_API void *nc_arena_realloc_chunk(nc_Arena *arena, void *ptr, usize size) {
    if (ptr == NULL) {
        return nc_arena_alloc_chunk(arena, size);
    }
    nc_Chunk *chunk = (nc_Chunk *)((uintptr_t)ptr - sizeof(nc_Chunk));
    if (size < chunk->allocated) {
        return chunk->data;
    }
    nc_Chunk *new_chunk = realloc(chunk, sizeof(nc_Chunk) + size);
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
NC_API void nc_arena_free_chunk(nc_Arena *arena, void *ptr) {
    if (ptr == NULL) {
        return;
    }
    nc_Chunk *chunk = (nc_Chunk *)((uintptr_t)ptr - sizeof(nc_Chunk));
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

// src/nc/structs/map.c
#include <string.h>
static void nc_map_insert(nc_Map *map, u64 hash, nc_MapValue value) {
    if (map->cap <= map->len + map->del) {
        nc_map_resize(map, map->cap * 2);
    }
    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
    }
    usize del_idx = (usize)-1;
    while (true) {
        usize idx = hash & (map->cap - 1);
        for (usize i = 0; i < map->cap; i++) {
            if (map->items[idx].hash == 0) {
                // NOTE: reusing a deleted slot
                if (del_idx != (usize)-1) {
                    map->items[del_idx] = (nc_MapItem){.hash = hash, .value = value};
                    map->len++;
                    map->del--;
                    return;
                }
                map->items[idx] = (nc_MapItem){.hash = hash, .value = value};
                map->len++;
                return;
            } else if (map->items[idx].hash == hash) {
                map->items[idx].value = value;
                return;
            } else if (map->items[idx].hash == NC_MAP_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (map->cap - 1);
        }
        nc_map_resize(map, map->cap * 2);
    }
    NC_UNREACHABLE("nc_map_insert");
}
static nc_MapValue *nc_map_get(const nc_Map *map, u64 hash) {
    if (map->len == 0) {
        return NULL;
    }
    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
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
NC_API void nc_map_init(nc_Map *map, nc_Arena *arena) {
    NC_ASSERT(map->items == NULL && "The map was already initialized");
    map->type = 0;
    map->len = 0;
    map->cap = 0;
    map->del = 0;
    map->arena = arena;
    map->items = NULL;
}
NC_API void nc_map_update(nc_Map *map, nc_Map *other) {
    nc_map_reserve(map, other->len);
    for (usize i = 0; i < other->cap; ++i) {
        if (other->items[i].hash && other->items[i].hash != NC_MAP_DELETED) {
            nc_map_insert(map, other->items[i].hash, other->items[i].value);
        }
    }
}
NC_API void nc_map_extend(nc_Map* map, usize count, nc_MapItem* items) {
    nc_map_reserve(map, count);
    for (usize i = 0; i < count; i++) {
        nc_map_insert(map, items[i].hash, items[i].value);
    }
}
NC_API void nc_map_clear(nc_Map* map) {
    map->len = 0;
    map->del = 0;
    memset(map->items, 0, sizeof(map->items[0]) * map->cap);
}
NC_API void nc_map_resize(nc_Map *map, usize size) {
    if (size < map->cap) {
        return;
    }
    usize old_cap = map->cap;
    nc_MapItem *old_items = map->items;
    map->cap = size == 0 ? NC_MAP_DEFAULT_SIZE : nc_usize_next_pow2(size);
    map->items = nc_arena_calloc_chunk(map->arena, map->cap * sizeof(map->items[0]));
    map->len = 0;
    map->del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i].hash && old_items[i].hash != NC_MAP_DELETED) {
            nc_map_insert(map, old_items[i].hash, old_items[i].value);
        }
    }
    nc_arena_free_chunk(map->arena, old_items);
}
NC_API void nc_map_reserve(nc_Map *map, usize size) {
    usize target = map->len + size;
    if (target <= map->cap) return;
    nc_map_resize(map, target);
}
NC_API bool nc_map_remove(nc_Map *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }
    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_MAP_DELETED)) {
        hash = nc_u64_hash(hash);
    }
    usize idx = hash & (map->cap - 1);
    for (usize i = 0; i < map->cap; i++) {
        if (map->items[idx].hash == 0) {
            return false;
        }
        if (map->items[idx].hash && map->items[idx].hash == hash) {
            map->items[idx].hash = NC_MAP_DELETED;
            map->len--;
            map->del++;
            return true;
        }
        idx = (idx + i * i) & (map->cap - 1);
    }
    return false;
}
NC_API void nc_map_insert_i64(nc_Map *map, u64 hash, i64 value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_I64);
    nc_map_insert(map, hash, (nc_MapValue){.i64 = value});
}
NC_API void nc_map_insert_u64(nc_Map *map, u64 hash, u64 value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_U64);
    nc_map_insert(map, hash, (nc_MapValue){.u64 = value});
}
NC_API void nc_map_insert_f64(nc_Map *map, u64 hash, f64 value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_F64);
    nc_map_insert(map, hash, (nc_MapValue){.f64 = value});
}
NC_API void nc_map_insert_ptr(nc_Map *map, u64 hash, void *value) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_PTR);
    nc_map_insert(map, hash, (nc_MapValue){.ptr = value});
}
NC_API i64 *nc_map_get_i64(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_I64);
    return (i64 *)nc_map_get(map, hash);
}
NC_API u64 *nc_map_get_u64(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_U64);
    return (u64 *)nc_map_get(map, hash);
}
NC_API f64 *nc_map_get_f64(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_F64);
    return (f64 *)nc_map_get(map, hash);
}
NC_API void *nc_map_get_ptr(nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_PTR);
    nc_MapValue *value = nc_map_get(map, hash);
    return value ? value->ptr : NULL;
}
NC_API const i64 *nc_map_get_i64_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_I64);
    return (const i64 *)nc_map_get(map, hash);
}
NC_API const u64 *nc_map_get_u64_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_U64);
    return (const u64 *)nc_map_get(map, hash);
}
NC_API const f64 *nc_map_get_f64_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_F64);
    return (const f64 *)nc_map_get(map, hash);
}
NC_API const void *nc_map_get_ptr_const(const nc_Map *map, u64 hash) {
    assert(map->type == NC_MAP_DYNAMIC || map->type == NC_MAP_PTR);
    nc_MapValue *value = nc_map_get(map, hash);
    return value ? value->ptr : NULL;
}

// src/nc/structs/set.c
NC_API void nc_set_init(nc_Set *set, nc_Arena *arena) {
    NC_ASSERT(set && arena);
    NC_ASSERT(set->items == NULL && "The map was already initialized");
    set->len = 0;
    set->_cap = 0;
    set->_del = 0;
    set->arena = arena;
    set->items = NULL;
}
NC_API void nc_set_resize(nc_Set *set, usize size) {
    if (size < set->_cap) {
        return;
    }
    usize old_cap = set->_cap;
    u64 *old_items = set->items;
    set->_cap = size == 0 ? NC_SET_DEFAULT_SIZE : nc_usize_next_pow2(size);
    set->items = nc_arena_calloc_chunk(set->arena, set->_cap * sizeof(set->items[0]));
    set->len = 0;
    set->_del = 0;
    for (usize i = 0; i < old_cap; ++i) {
        if (old_items[i] && old_items[i] != NC_SET_DELETED) {
            nc_set_add(set, old_items[i]);
        }
    }
    nc_arena_free_chunk(set->arena, old_items);
}
NC_API void nc_set_reserve(nc_Set *map, usize size) {
    usize target = map->len + size;
    if (target <= map->_cap) return;
    nc_set_resize(map, target);
}
NC_API bool nc_set_remove(nc_Set *set, u64 hash) {
    if (set->len == 0) {
        return false;
    }
    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_SET_DELETED)) {
        hash = nc_u64_hash(hash);
    }
    usize idx = hash & (set->_cap - 1);
    for (usize i = 0; i < set->_cap; i++) {
        if (set->items[idx] == 0) {
            return false;
        }
        if (set->items[idx] && set->items[idx] == hash) {
            set->items[idx] = NC_SET_DELETED;
            set->len--;
            set->_del++;
            return true;
        }
        idx = (idx + i * i) & (set->_cap - 1);
    }
    return false;
}
NC_API bool nc_set_add(nc_Set *set, u64 hash) {
    if (set->_cap <= set->len + set->_del) {
        nc_set_resize(set, set->_cap * 2);
    }
    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_SET_DELETED)) {
        hash = nc_u64_hash(hash);
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
            } else if (set->items[idx] == NC_SET_DELETED && del_idx == (usize)-1) {
                del_idx = idx;
            }
            idx = (idx + i * i) & (set->_cap - 1);
        }
        nc_set_resize(set, set->_cap * 2);
    }
    NC_UNREACHABLE("nc_set_add");
}
NC_API bool nc_set_has(const nc_Set *map, u64 hash) {
    if (map->len == 0) {
        return false;
    }
    // NOTE: rehash in the slight chance that the hash is 0 or NC_MAP_DELETED
    if (NC_UNLIKELY(hash == 0 || hash == NC_SET_DELETED)) {
        hash = nc_u64_hash(hash);
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
NC_API void nc_set_update(nc_Set* map, const nc_Set* other) {
    nc_set_reserve(map, other->len);
    for (usize i = 0; i < other->_cap; ++i) {
        if (other->items[i] && other->items[i] != NC_SET_DELETED) {
            nc_set_add(map, other->items[i]);
        }
    }
}
NC_API void nc_set_extend(nc_Set* set, usize count, const u64* hashes) {
  nc_set_reserve(set, count);
  for (usize i = 0; i < count; i++) {
    nc_set_add(set, hashes[i]);
  }
}
NC_API bool nc_set_eq(const nc_Set *set, const nc_Set *other) {
  if (other->len != set->len) {
    return false;
  }
  if (other->_cap < set->_cap) {
    const nc_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}
NC_API bool nc_set_subset(const nc_Set *set, const nc_Set *other) {
  if (other->len < set->len) {
    return false;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}
NC_API bool nc_set_disjoint(const nc_Set *set, const nc_Set *other) {
  if (other->len == 0 || set->len == 0) {
    return true;
  }
  if (other->_cap < set->_cap) {
    const nc_Set *temp = set;
    set = other;
    other = temp;
  }
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (nc_set_has(other, set->items[i])) {
        return false;
      }
    }
  }
  return true;
}
NC_API void nc_set_intersection(const nc_Set *set, const nc_Set *other, nc_Set* out) {
  if (other->_cap < set->_cap) {
    const nc_Set *temp = set;
    set = other;
    other = temp;
  }
  nc_set_reserve(out, nc_usize_min(set->len, other->len) * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (nc_set_has(other, set->items[i])) {
        nc_set_add(out, set->items[i]);
      }
    }
  }
}
NC_API void nc_set_difference(const nc_Set *set, const nc_Set *other, nc_Set* out) {
  nc_set_reserve(out, set->len * 2);
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        nc_set_add(out, set->items[i]);
      }
    }
  }
}
NC_API void nc_set_union(const nc_Set *set, const nc_Set *other, nc_Set* out) {
  for (usize i = 0; i < set->_cap; i++) {
    if (set->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(other, set->items[i])) {
        nc_set_add(out, set->items[i]);
      }
    }
  }
  for (usize i = 0; i < other->_cap; i++) {
    if (other->items[i] && set->items[i] != NC_SET_DELETED) {
      if (!nc_set_has(set, other->items[i])) {
        nc_set_add(out, other->items[i]);
      }
    }
  }
}

// src/nc/types/int.c
#include <string.h>
#define BITS(T) (sizeof(T) * 8)
#define INTEGER_IMPL(T)                                                                            \
    NC_API T nc_##T##_reverse_bits(T value) {                                                      \
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
    NC_API usize nc_##T##_leading_ones(T value) {                                                  \
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
    NC_API usize nc_##T##_trailing_ones(T value) {                                                 \
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
    NC_API usize nc_##T##_leading_zeros(T value) {                                                 \
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
    NC_API usize nc_##T##_trailing_zeros(T value) {                                                \
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
    NC_API usize nc_##T##_count_zeros(T value) {                                                   \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (!(value >> i & (T)0x1)) {                                                          \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API usize nc_##T##_count_ones(T value) {                                                    \
        usize count = 0;                                                                           \
        for (usize i = 0; i < BITS(T); i++) {                                                      \
            if (value >> i & (T)0x1) {                                                             \
                count++;                                                                           \
            }                                                                                      \
        }                                                                                          \
        return count;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_swap_bytes(T value) {                                                        \
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
    NC_API T nc_##T##_to_be(T value) {                                                             \
        if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                                   \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_be(T value) {                                                           \
        if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                                   \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_be_bytes(nc_Bytes bytes) {                                              \
        NC_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                                   \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API nc_Bytes nc_##T##_to_be_bytes(T value, nc_Arena *arena) {                               \
        u8 *buffer = nc_arena_alloc(arena, sizeof(value));                                         \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                  \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nc_bytes_from_parts(sizeof(value), buffer);                                         \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_to_le(T value) {                                                             \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_le(T value) {                                                           \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_le_bytes(nc_Bytes bytes) {                                              \
        NC_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_swap_bytes(value);                                                     \
        }                                                                                          \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API nc_Bytes nc_##T##_to_le_bytes(T value, nc_Arena *arena) {                               \
        u8 *buffer = nc_arena_alloc(arena, sizeof(value));                                         \
        u8 *bytes = (u8 *)&value;                                                                  \
        for (usize i = 0; i < sizeof(value); i++) {                                                \
            if (NC_BYTE_ORDER == NC_ENDIAN_LITTLE) {                                               \
                buffer[i] = bytes[i];                                                              \
            } else {                                                                               \
                buffer[sizeof(value) - i - 1] = bytes[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return nc_bytes_from_parts(sizeof(value), buffer);                                         \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_from_ne_bytes(nc_Bytes bytes) {                                              \
        NC_ASSERT(sizeof(T) == bytes.size && "expected " #T);                                      \
        T value = 0;                                                                               \
        memcpy(&value, bytes.data, sizeof(T));                                                     \
        return value;                                                                              \
    }                                                                                              \
                                                                                                   \
    NC_API nc_Bytes nc_##T##_to_ne_bytes(T value, nc_Arena *arena) {                               \
        if (NC_BYTE_ORDER == NC_ENDIAN_BIG) {                                                      \
            return nc_##T##_to_be_bytes(value, arena);                                             \
        }                                                                                          \
        return nc_##T##_to_le_bytes(value, arena);                                                 \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_max(T a, T b) {                                                              \
        return a < b ? b : a;                                                                      \
    }                                                                                              \
    NC_API T nc_##T##_min(T a, T b) {                                                              \
        return a > b ? b : a;                                                                      \
    }                                                                                              \
    NC_API T nc_##T##_clamp(T min, T max, T value) {                                               \
        return value < min ? min : max < value ? max : value;                                      \
    }                                                                                              \
                                                                                                   \
    NC_API u64 nc_##T##_hash(T value) {                                                            \
        u64 hash = ((u64)value) + 1;                                                               \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (((hash >> 16) ^ hash) % 0x3AA387A8B1) * 0x45d9f3b;                                 \
        hash = (hash >> 16) ^ hash;                                                                \
        return hash;                                                                               \
    }                                                                                              \
                                                                                                   \
    NC_API void nc_##T##_swap(T *v1, T *v2) {                                                      \
        T temp = *v1;                                                                              \
        *v1 = *v2;                                                                                 \
        *v2 = temp;                                                                                \
    }                                                                                              \
                                                                                                   \
    NC_API T nc_##T##_next_pow2(T n) {                                                             \
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

// src/nc/types/path.c
#include <stdio.h>
#include <string.h>
NC_API nc_Path nc_path_join(usize len, const nc_Path *parts, nc_Arena *arena) {
    if (len == 0) {
        return NC_STR("");
    }
    if (len == 1) {
        return nc_str_copy(parts[0], arena);
    }
    nc_List(char) buffer = {0};
    nc_list_init(&buffer, arena);
    for (usize i = 0; i < len; i++) {
        nc_list_reserve(&buffer, parts[i].len + 2);
        if (i && buffer.len && !nc_char_is_path_delimiter(nc_list_last(&buffer))) {
            nc_list_push(&buffer, '/');
        }
        for (usize j = 0; j < parts[i].len; j++) {
            if (nc_char_is_path_delimiter(parts[i].data[j])) {
                if (buffer.len && nc_char_is_path_delimiter(nc_list_last(&buffer))) {
                    continue;
                }
                nc_list_push(&buffer, '/');
            } else {
                nc_list_push(&buffer, parts[i].data[j]);
            }
        }
    }
    nc_list_push(&buffer, '\0');
    return nc_str_from_parts(buffer.len-1, buffer.items);
}
NC_API nc_Path nc_path_normalize(nc_Path path, nc_Arena *arena) {
    nc_Arena scratch = {0};
    nc_List(nc_Path) parts = {0};
    nc_list_init(&parts, &scratch);
    char win_path_prefix_buffer[4] = "C:/";
    nc_Path prefix = NC_PATH("");
    if (nc_path_is_absolute(path)) {
        if (path.len && nc_char_is_path_delimiter(path.data[0])) {
            prefix = NC_PATH("/");
        } else if (path.len > 2) {
            win_path_prefix_buffer[0] = path.data[0];
            prefix = nc_str_from_parts(3, win_path_prefix_buffer);
            path = nc_str_substring(path, 3, path.len);
        }
    }
    nc_Path part;
    while (nc_str_try_chop_by_predicate(&path, nc_char_is_path_delimiter, &part)) {
        if (part.len == 0) {
            continue;
        } else if (nc_str_eq(part, NC_STR("."))) {
            continue;
        } else if (nc_str_eq(part, NC_STR(".."))) {
            if (parts.len) (void)nc_list_pop(&parts);
            else if (!prefix.len) nc_list_push(&parts, part);
            continue;
        }
        nc_list_push(&parts, part);
    }
    nc_Path result = nc_path_join(parts.len, parts.items, &scratch);
    result = nc_str_prepend(result, prefix, arena);
    nc_arena_free(&scratch);
    return result;
}
NC_API bool nc_path_eq(nc_Path p1, nc_Path p2) {
    while (true) {
        nc_Path c1 = nc_str_chop_by_predicate(&p1, nc_char_is_path_delimiter);
        nc_Path c2 = nc_str_chop_by_predicate(&p2, nc_char_is_path_delimiter);
        if (c1.len == 0 && c2.len == 0) break;
        if (!nc_str_eq(c1, c2)) return false;;
    }
    return true;
}
NC_API bool nc_path_is_absolute(nc_Path path) {
    if (path.len == 0)
        return false;
    if (path.data[0] == '/')
        return true;
    if (path.len >= 3 && path.data[1] == ':' && nc_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}
NC_API nc_Str nc_path_name(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    return nc_str_chop_right_by_predicate(&path, nc_char_is_path_delimiter);
}
NC_API nc_Str nc_path_suffix(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    nc_Str name = nc_str_chop_right_by_predicate(&path, nc_char_is_path_delimiter);
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return NC_STR("");
    }
    return nc_str_substring(name, idx, name.len);
}
NC_API nc_Str nc_path_stem(nc_Path path) {
    nc_Str name = nc_str_chop_right_by_predicate(&path, nc_char_is_path_delimiter);
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return name;
    }
    return nc_str_substring(name, 0, idx);
}
NC_API nc_Path nc_path_parent(nc_Path path) {
    if (path.len == 1 && nc_char_is_path_delimiter(path.data[0])) return path;
    if (nc_str_endswith_predicate(path, nc_char_is_path_delimiter)) path.len--;
    usize idx = nc_str_find_last_by_predicate(path, nc_char_is_path_delimiter);
    if (idx == NC_STR_NOT_FOUND) return NC_PATH(".");
    return nc_str_substring(path, 0, nc_usize_min(path.len, idx+1));
}

// src/nc/types/char.c
#include <ctype.h>
#define DBASE 10
#define XBASE 16
///////////////////////////////////////////////////////////////////////////////
NC_API bool nc_char_is_alnum(char c) { return isalnum(c); }
NC_API bool nc_char_is_alpha(char c) { return isalpha(c); }
NC_API bool nc_char_is_lower(char c) { return islower(c); }
NC_API bool nc_char_is_upper(char c) { return isupper(c); }
NC_API bool nc_char_is_space(char c) { return isspace(c); }
NC_API bool nc_char_is_cntrl(char c) { return iscntrl(c); }
NC_API bool nc_char_is_print(char c) { return isprint(c); }
NC_API bool nc_char_is_graph(char c) { return isgraph(c); }
NC_API bool nc_char_is_blank(char c) { return isblank(c); }
NC_API bool nc_char_is_punct(char c) { return ispunct(c); }
NC_API bool nc_char_is_digit(char c) { return isdigit(c); }
NC_API bool nc_char_is_xdigit(char c) { return isxdigit(c); }
NC_API NC_CONST_FN bool nc_char_is_path_delimiter(char c) { return c == '/' || c == '\\'; }
///////////////////////////////////////////////////////////////////////////////
NC_API char nc_char_to_lower(char c) { return (char)tolower(c); }
NC_API char nc_char_to_upper(char c) { return (char)toupper(c); }
NC_API u8 nc_char_to_u8(char c) {
  NC_ASSERT(nc_char_is_digit(c) && "char not convertible");
  return (u8)c - '0';
}
NC_API u8 nc_char_hex_to_u8(char c) {
  NC_ASSERT(nc_char_is_xdigit(c) && "char not convertible");
  if ('0' <= c && c <= '9') {
    return nc_char_to_u8(c);
  }
  if ('a' <= c && c <= 'f') {
    return DBASE + (u8)c - 'a';
  }
  if ('A' <= c && c <= 'F') {
    return DBASE + (u8)c - 'A';
  }
  return 0;
}
NC_API char nc_char_from_u8(u8 d) {
  NC_ASSERT(d < DBASE && "char not convertible");
  return '0' + (i8)d;
}
NC_API char nc_char_hex_from_u8(u8 d) {
  NC_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nc_char_from_u8(d);
  }
  if (d < XBASE) {
    return 'a' + ((i8)d - DBASE);
  }
  return 0;
}
NC_API char nc_char_HEX_from_u8(u8 d) {
  NC_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nc_char_from_u8(d);
  }
  if (d < XBASE) {
    return 'A' + ((i8)d - DBASE);
  }
  return 0;
}

// src/nc/types/str.c
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////
NC_API nc_Str nc_str_from_parts(usize size, const char *cstr) {
    return (nc_Str){.len = size, .data = cstr};
}
NC_API nc_Str nc_str_from_bytes(nc_Bytes bytes) {
    return nc_str_from_parts(bytes.size, (const char *)bytes.data);
}
NC_API nc_Bytes nc_str_to_bytes(nc_Str s) {
    return nc_bytes_from_parts(s.len, s.data);
}
NC_API nc_Str nc_str_from_cstr(const char *cstr) {
    return (nc_Str){.len = strlen(cstr), .data = cstr};
}
NC_API nc_Str nc_str_format(nc_Arena *arena, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    usize size = (usize)vsnprintf(NULL, 0, fmt, va) + 1;
    va_end(va);
    char *buffer = nc_arena_calloc(arena, size);
    va_start(va, fmt);
    vsnprintf(buffer, size, fmt, va);
    va_end(va);
    return nc_str_from_parts(size - 1, buffer);
}
///////////////////////////////////////////////////////////////////////////////
NC_API nc_Str nc_str_copy(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    memcpy(buffer, s.data, s.len);
    buffer[s.len] = '\0';
    return nc_str_from_parts(s.len, buffer);
}
NC_API nc_Str nc_str_append(nc_Str s, nc_Str suffix, nc_Arena *arena) {
    const usize new_size = s.len + suffix.len;
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], s.data, s.len);
    memcpy(&buffer[s.len], suffix.data, suffix.len);
    buffer[new_size] = '\0';
    return nc_str_from_parts(new_size, buffer);
}
NC_API nc_Str nc_str_prepend(nc_Str s, nc_Str prefix, nc_Arena *arena) {
    const usize new_size = s.len + prefix.len;
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], prefix.data, prefix.len);
    memcpy(&buffer[prefix.len], s.data, s.len);
    buffer[new_size] = '\0';
    return nc_str_from_parts(new_size, buffer);
}
NC_API nc_Str nc_str_wrap(nc_Str s, nc_Str wrap, nc_Arena *arena) {
    const usize new_size = s.len + wrap.len * 2;
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], wrap.data, wrap.len);
    memcpy(&buffer[wrap.len], s.data, s.len);
    memcpy(&buffer[wrap.len + s.len], wrap.data, wrap.len);
    buffer[new_size] = '\0';
    return nc_str_from_parts(new_size, buffer);
}
NC_API nc_Str nc_str_join(nc_Str sep, usize count, nc_Str *s, nc_Arena *arena) {
    if (count == 0) {
        return NC_STR("");
    }
    usize size = sep.len * (count - 1);
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
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
    return nc_str_from_parts(size, buffer);
}
NC_API nc_Str nc_str_join_suffix(nc_Str suffix, usize count, nc_Str *s, nc_Arena *arena) {
    usize size = suffix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
        memcpy(&buffer[b_idx], suffix.data, suffix.len);
        b_idx += suffix.len;
    }
    buffer[size] = '\0';
    return nc_str_from_parts(size, buffer);
}
NC_API nc_Str nc_str_join_prefix(nc_Str prefix, usize count, nc_Str *s, nc_Arena *arena) {
    usize size = prefix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], prefix.data, prefix.len);
        b_idx += prefix.len;
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
    }
    buffer[size] = '\0';
    return nc_str_from_parts(size, buffer);
}
NC_API nc_Str nc_str_join_wrap(nc_Str sep, nc_Str wrap, usize count, nc_Str *s, nc_Arena *arena) {
    usize size = sep.len * (count - 1) + wrap.len * count * 2;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
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
    return nc_str_from_parts(size, buffer);
}
NC_API nc_Str nc_str_upper(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nc_char_to_upper(s.data[i]);
    }
    return nc_str_from_parts(s.len, buffer);
}
NC_API nc_Str nc_str_lower(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nc_char_to_lower(s.data[i]);
    }
    return nc_str_from_parts(s.len, buffer);
}
NC_API nc_Str nc_str_replace(nc_Str s, nc_Str old, nc_Str new, nc_Arena *arena) {
    usize count = nc_str_count(s, old);
    usize new_size = (s.len - (old.len * count) + (count * new.len));
    char *buffer = nc_arena_alloc(arena, new_size + 1);
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
    return nc_str_from_parts(new_size, buffer);
}
NC_API nc_Str nc_str_center(nc_Str s, usize width, char fillchar, nc_Arena *arena) {
    if (width < s.len) {
        return nc_str_copy(s, arena);
    }
    char *buffer = nc_arena_calloc(arena, width + 1);
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
    return nc_str_from_parts(width, buffer);
}
NC_API nc_Str nc_str_ljust(nc_Str s, usize width, char fillchar, nc_Arena *arena) {
    if (width < s.len) {
        return nc_str_copy(s, arena);
    }
    char *buffer = nc_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    return nc_str_from_parts(width, buffer);
}
NC_API nc_Str nc_str_rjust(nc_Str s, usize width, char fillchar, nc_Arena *arena) {
    if (width < s.len) {
        return nc_str_copy(s, arena);
    }
    char *buffer = nc_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    return nc_str_from_parts(width, buffer);
}
NC_API nc_Str nc_str_repeat(nc_Str s, usize count, nc_Arena *arena) {
    usize len = s.len * count;
    char *buffer = nc_arena_alloc(arena, len + 1);
    buffer[len] = '\0';
    usize idx = 0;
    for (usize i = 0; i < count; i++) {
        for (usize j = 0; j < s.len; j++) {
            buffer[idx++] = s.data[j];
        }
    }
    return nc_str_from_parts(len, buffer);
}
NC_API nc_Str nc_str_reverse(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = s.data[s.len - i - 1];
    }
    return nc_str_from_parts(s.len, buffer);
}
///////////////////////////////////////////////////////////////////////////////
NC_API bool nc_str_eq(nc_Str s1, nc_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    return memcmp(s1.data, s2.data, s1.len) == 0;
}
NC_API bool nc_str_eq_ignorecase(nc_Str s1, nc_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    for (size_t i = 0; i < s1.len; i++) {
        if (nc_char_to_lower(s1.data[i]) != nc_char_to_lower(s2.data[i])) {
            return false;
        }
    }
    return true;
}
NC_API bool nc_str_startswith(nc_Str s1, nc_Str prefix) {
    if (s1.len < prefix.len) {
        return false;
    }
    return memcmp(s1.data, prefix.data, prefix.len) == 0;
}
NC_API bool nc_str_endswith(nc_Str s1, nc_Str suffix) {
    if (s1.len < suffix.len) {
        return false;
    }
    const usize idx = s1.len - suffix.len;
    return memcmp(&s1.data[idx], suffix.data, suffix.len) == 0;
}
NC_API bool nc_str_endswith_predicate(nc_Str s1, bool (*predicate)(char)) {
    if (s1.len == 0) {
        return false;
    }
    const usize idx = s1.len - 1;
    return predicate(s1.data[idx]);
}
NC_API bool nc_str_contains(nc_Str haystack, nc_Str needle) {
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
NC_API bool nc_str_includes(nc_Str haystack, char needle) {
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
NC_API bool nc_str_is_empty(nc_Str s) {
    return s.len == 0;
}
NC_API nc_Str nc_str_trim_left(nc_Str s) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && nc_char_is_space(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}
NC_API nc_Str nc_str_trim_left_by_delim(nc_Str s, char delim) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && s.data[i] == delim; ++i) {
        result.data++;
        result.len--;
    }
    return result;
}
NC_API nc_Str nc_str_trim_left_by_predicate(nc_Str s, bool (*predicate)(char)) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}
NC_API nc_Str nc_str_trim_right(nc_Str s) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && nc_char_is_space(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}
NC_API nc_Str nc_str_trim_right_by_delim(nc_Str s, char delim) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && s.data[s.len - i - 1] == delim; ++i) {
        result.len--;
    }
    return result;
}
NC_API nc_Str nc_str_trim_right_by_predicate(nc_Str s, bool (*predicate)(char)) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}
NC_API nc_Str nc_str_trim(nc_Str s) {
    return nc_str_trim_left(nc_str_trim_right(s));
}
NC_API nc_Str nc_str_trim_by_delim(nc_Str s, char delim) {
    return nc_str_trim_left_by_delim(nc_str_trim_right_by_delim(s, delim), delim);
}
NC_API nc_Str nc_str_trim_by_predicate(nc_Str s, bool (*predicate)(char)) {
    return nc_str_trim_left_by_predicate(nc_str_trim_right_by_predicate(s, predicate), predicate);
}
NC_API bool nc_str_try_chop_by_delim(nc_Str *s, char delim, nc_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }
    if (s->len && i <= s->len) {
        if (chunk) {
            *chunk = nc_str_from_parts(i, s->data);
        }
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        *s = nc_str_trim_left_by_delim(*s, delim);
        return true;
    }
    return false;
}
NC_API nc_Str nc_str_chop_by_delim(nc_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }
    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, s->data);
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        *s = nc_str_trim_left_by_delim(*s, delim);
        return chunk;
    }
    return *s;
}
NC_API bool nc_str_try_chop_by_predicate(nc_Str *s, bool (*predicate)(char), nc_Str *chunk) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }
    if (s->len && i <= s->len) {
        if (chunk) {
            *chunk = nc_str_from_parts(i, s->data);
        }
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        *s = nc_str_trim_left_by_predicate(*s, predicate);
        return true;
    }
    return false;
}
NC_API nc_Str nc_str_chop_by_predicate(nc_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }
    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, s->data);
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        *s = nc_str_trim_left_by_predicate(*s, predicate);
        return chunk;
    }
    return *s;
}
NC_API nc_Str nc_str_chop_right_by_delim(nc_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[s->len - i - 1] != delim) {
        ++i;
    }
    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nc_usize_min(s->len, i + 1);
        *s = nc_str_trim_right_by_delim(*s, delim);
        return chunk;
    }
    return *s;
}
NC_API nc_Str nc_str_chop_right_by_predicate(nc_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[s->len - i - 1])) {
        ++i;
    }
    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nc_usize_min(s->len, i + 1);
        *s = nc_str_trim_right_by_predicate(*s, predicate);
        return chunk;
    }
    return *s;
}
NC_API nc_Str nc_str_substring(nc_Str s, usize start, usize end) {
    if (end <= start || s.len <= start || s.len < end) {
        return NC_STR("");
    }
    return nc_str_from_parts(end - start, &s.data[start]);
}
NC_API nc_Str nc_str_take(nc_Str *s, usize count) {
    count = nc_usize_min(s->len, count);
    nc_Str ret = nc_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return ret;
}
NC_API bool nc_str_try_take(nc_Str *s, usize count, nc_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nc_usize_min(s->len, count);
    *chunk = nc_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return true;
}
NC_API nc_Str nc_str_take_right(nc_Str *s, usize count) {
    count = nc_usize_min(s->len, count);
    nc_Str ret = nc_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return ret;
}
NC_API bool nc_str_try_take_right(nc_Str *s, usize count, nc_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nc_usize_min(s->len, count);
    *chunk = nc_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return true;
}
NC_API u64 nc_str_u64(nc_Str s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(s, &arena);
    u64 value = strtoull(owned.data, NULL, radix);
    nc_arena_free(&arena);
    return value;
}
NC_API u64 nc_str_chop_u64(nc_Str *s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(*s, &arena);
    char *endptr;
    u64 value = strtoull(owned.data, &endptr, radix);
    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;
    nc_arena_free(&arena);
    return value;
}
NC_API i64 nc_str_i64(nc_Str s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(s, &arena);
    i64 value = strtoll(owned.data, NULL, radix);
    nc_arena_free(&arena);
    return value;
}
NC_API i64 nc_str_chop_i64(nc_Str *s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(*s, &arena);
    char *endptr;
    i64 value = strtoll(owned.data, &endptr, radix);
    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;
    nc_arena_free(&arena);
    return value;
}
NC_API f64 nc_str_f64(nc_Str s) {
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(s, &arena);
    double value = strtod(owned.data, NULL);
    nc_arena_free(&arena);
    return value;
}
NC_API f64 nc_str_chop_f64(nc_Str *s) {
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(*s, &arena);
    char *endptr;
    f64 value = strtod(owned.data, &endptr);
    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;
    nc_arena_free(&arena);
    return value;
}
NC_API usize nc_str_find(nc_Str haystack, nc_Str needle) {
    if (haystack.len < needle.len) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            return i;
        }
    }
    return NC_STR_NOT_FOUND;
}
NC_API usize nc_str_find_by_predicate(nc_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len; i++) {
        if (predicate(haystack.data[i])) {
            return i;
        }
    }
    return NC_STR_NOT_FOUND;
}
NC_API usize nc_str_find_last(nc_Str haystack, nc_Str needle) {
    if (haystack.len < needle.len) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = haystack.len - needle.len + 1; i > 0; i--) {
        if (memcmp(&haystack.data[i - 1], needle.data, needle.len) == 0) {
            return i - 1;
        }
    }
    return NC_STR_NOT_FOUND;
}
NC_API usize nc_str_find_last_by_predicate(nc_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = haystack.len; i > 0; i--) {
        if (predicate(haystack.data[i - 1])) {
            return i - 1;
        }
    }
    return NC_STR_NOT_FOUND;
}
NC_API usize nc_str_count(nc_Str haystack, nc_Str needle) {
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
NC_API char nc_str_getc(nc_Str s, usize idx) {
    if (s.len <= idx) {
        return '\0';
    }
    return s.data[idx];
}
NC_API u64 nc_str_hash(nc_Str s) {
    const uint64_t magic_prime = 0x00000100000001b3;
    uint64_t hash = 0xcbf29ce484222325; // NOLINT
    for (usize i = 0; i < s.len; ++i) {
        hash = (hash ^ (u64)s.data[i]) * magic_prime;
    }
    return hash;
}

// src/nc/types/byte.c
#include <stdio.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////
NC_API nc_Bytes nc_bytes_from_parts(usize size, const void *data) {
    return (nc_Bytes){.size = size, .data = data};
}
///////////////////////////////////////////////////////////////////////////////
NC_API nc_Bytes nc_bytes_copy(nc_Bytes bytes, nc_Arena *arena) {
    u8 *buffer = nc_arena_alloc(arena, bytes.size);
    memcpy(buffer, bytes.data, bytes.size);
    return nc_bytes_from_parts(bytes.size, buffer);
}
///////////////////////////////////////////////////////////////////////////////
NC_API nc_Bytes nc_bytes_slice(nc_Bytes bytes, usize idx1, usize idx2) {
    if (idx2 <= idx1 || bytes.size <= idx1 || bytes.size < idx2) {
        return nc_bytes_from_parts(0, bytes.data);
    }
    return nc_bytes_from_parts(idx2 - idx1, &bytes.data[idx1]);
}
NC_API nc_Bytes nc_bytes_take(nc_Bytes *bytes, usize count) {
    count = nc_usize_min(bytes->size, count);
    nc_Bytes ret = nc_bytes_from_parts(count, bytes->data);
    bytes->size -= count;
    bytes->data += count;
    return ret;
}
///////////////////////////////////////////////////////////////////////////////
NC_API bool nc_bytes_eq(nc_Bytes b1, nc_Bytes b2) {
    if (b1.size != b2.size) return false;
    return memcmp(b1.data, b2.data, b1.size) == 0;
}
NC_API u64 nc_bytes_hash(nc_Bytes bytes) {
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
NC_API nc_Str nc_bytes_to_hex(nc_Bytes bytes, nc_Arena *arena) {
    char *buf = nc_arena_calloc(arena, bytes.size * 2 + 1);
    usize idx = 0;
    for (usize i = 0; i < bytes.size; i++) {
        idx += (usize)snprintf(&buf[idx], 3, "%0*x", (i != 0) + 1, bytes.data[i]);
    }
    return (nc_Str){.len = idx, .data = buf};
}
NC_API nc_Bytes nc_bytes_from_hex(nc_Str s, nc_Arena *arena) {
    if (nc_str_startswith(s, NC_STR("0x"))) {
        s = nc_str_substring(s, 2, s.len);
    }
    u8 *buffer = nc_arena_calloc(arena, (s.len / 2) + (s.len % 2));
    // to convert strings like "0x101".
    // in the first iteration:
    // take 1 or 2 chars depending if s.len is even or odd
    usize idx = 0;
    for (nc_Str ch = {0}; nc_str_try_take(&s, idx == 0 ? 2 - s.len % 2 : 2, &ch);) {
        for (usize i = 0; i < ch.len; i++) {
            buffer[idx] = (u8)(buffer[idx] << 4);
            if (nc_char_is_xdigit(ch.data[i])) {
                buffer[idx] |= nc_char_hex_to_u8(ch.data[i]);
            }
        }
        idx++;
    }
    return nc_bytes_from_parts(idx, buffer);
}
///////////////////////////////////////////////////////////////////////////////

#endif // NC_IMPLEMENTATION
