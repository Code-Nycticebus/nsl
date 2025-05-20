#pragma once

#include <stdbool.h>

#ifndef NC_NO_INT_TYPEDEFS
    #include <stdint.h>
    #include <stddef.h>
    typedef uint8_t u8;
    typedef int8_t i8;
    typedef uint16_t u16;
    typedef int16_t i16;
    typedef uint32_t u32;
    typedef int32_t i32;
    typedef uint64_t u64;
    typedef int64_t i64;
    typedef size_t usize;
    typedef float f32;
    typedef double f64;
#endif

#define BOOL_FMT "%s"
#define BOOL_ARG(b) (b ? "true" : "false")

#if defined(__clang__)
    #define NC_COMPILER_CLANG 1
    #define NC_COMPILER_NAME "clang"
#elif defined(__GNUC__) && !defined(__clang__)
    #define NC_COMPILER_GCC 1
    #define NC_COMPILER_NAME "gcc"
#elif defined(__TINYC__)
    #define NC_COMPILER_TCC 1
    #define NC_COMPILER_NAME "tcc"
#elif defined(_MSC_VER)
    #define NC_COMPILER_MSVC 1
    #define NC_COMPILER_NAME "cl"
#elif defined(__MINGW32__)
    #define NC_COMPILER_MINGW32 1
    #define NC_COMPILER_NAME "mingw32"
#elif defined(__MINGW64__)
    #define NC_COMPILER_MINGW64 1
    #define NC_COMPILER_NAME "mingw64"
#else
    #define NC_COMPILER_UNKNOWN 1
    #define NC_COMPILER_NAME "unknown"
#endif

#if defined(NC_COMPILER_GCC) || defined(NC_COMPILER_CLANG)
    #define NC_EXPORT            __attribute__((used))
    #define NC_NORETURN          __attribute__((noreturn))
    #define NC_UNUSED            __attribute__((unused))
    #define NC_PURE_FN           __attribute__((pure)) __attribute__((warn_unused_result))
    #define NC_CONST_FN          __attribute__((const)) __attribute__((warn_unused_result))
    #define NC_LIKELY(exp)       __builtin_expect(!!(exp), 1)
    #define NC_UNLIKELY(exp)     __builtin_expect(!!(exp), 0)
    #define NC_FMT(fmt_idx)      __attribute__((format(printf, fmt_idx, fmt_idx + 1)))
#elif defined(NC_COMPILER_MSVC)
    #include <sal.h>
    #define NC_EXPORT            __declspec(dllexport)
    #define NC_NORETURN          __declspec(noreturn)
    #define NC_UNUSED            __pragma(warning(suppress : 4100))
    #define NC_PURE_FN           _Check_return_
    #define NC_CONST_FN          _Check_return_
    #define NC_LIKELY(exp)       (exp)
    #define NC_UNLIKELY(exp)     (exp)
    #define NC_FMT(fmt_idx)
#else
    #define NC_EXPORT
    #define NC_NORETURN
    #define NC_UNUSED
    #define NC_PURE_FN
    #define NC_CONST_FN
    #define NC_LIKELY(exp)       (exp)
    #define NC_UNLIKELY(exp)     (exp)
    #define NC_FMT(fmt_idx)
#endif
