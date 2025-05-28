#ifndef _NC_ERROR_H_
#define _NC_ERROR_H_

#include "nc/defines.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    i64 code;            // 0 = no error
    const char *file;    // __FILE__ where error occured
    i32 line;            // __LINE__ where error occured
    const char *func;    // __func__ where error occured
    const char *message; // human-readable message
} nc_Error;

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
