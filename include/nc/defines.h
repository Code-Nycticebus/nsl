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

#define nc_List(T)                                                                                 \
    struct {                                                                                       \
        usize cap;                                                                                 \
        usize len;                                                                                 \
        nc_Arena *arena;                                                                           \
        T *items;                                                                                  \
    }

typedef nc_List(char) nc_StrBuilder;
typedef nc_List(u8) nc_ByteBuffer;

#define NC_UNUSED(v) (void)(v)
#define NC_PASS ((void)(0))

#define NC_PANIC(msg)                                                                              \
    do {                                                                                           \
        fprintf(stderr, "Panic: %s:%d: %s\n", __FILE__, __LINE__, msg);                            \
        abort();                                                                                   \
    } while (0)

#define NC_UNREACHABLE(msg)                                                                        \
    do {                                                                                           \
        fprintf(stderr, "Unreachable: %s:%d: %s\n", __FILE__, __LINE__, msg);                      \
        abort();                                                                                   \
    } while (0)

#define NC_TODO(msg)                                                                               \
    do {                                                                                           \
        fprintf(stderr, "TODO: %s:%d: %s\n", __FILE__, __LINE__, msg);                             \
        abort();                                                                                   \
    } while (0)

#define NC_NOT_IMPLEMENTED(msg)                                                                    \
    do {                                                                                           \
        fprintf(stderr, "Not Implemented: %s:%d: %s\n", __FILE__, __LINE__, msg);                  \
        abort();                                                                                   \
    } while (0)


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
#    define NC_BYTE_ORDER NC_ENDIAN_LITTLE
#else
#    error "No Byte Order detected!"
#endif

#endif // _NC_DEFINES_H_
