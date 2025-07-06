#ifndef _NSL_DEFINES_H_
#define _NSL_DEFINES_H_

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

#endif // _NSL_DEFINES_H_
