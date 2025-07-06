#ifndef _NSL_ERROR_H_
#define _NSL_ERROR_H_

#include "nsl/defines.h"

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
#define NSL_ERROR_EMIT(E, error_code, error_message)                                               \
    do {                                                                                           \
        if ((E) == NULL) {                                                                         \
            nsl_Error _panic_error = {                                                             \
                error_code, __FILE__, __LINE__, __func__, error_message,                           \
            };                                                                                     \
            NSL_ERROR_RAISE(&_panic_error);                                                        \
        } else {                                                                                   \
            (E)->code = error_code;                                                                \
            (E)->file = __FILE__;                                                                  \
            (E)->line = __LINE__;                                                                  \
            (E)->func = __func__;                                                                  \
            (E)->message = error_message;                                                          \
        }                                                                                          \
    } while (0)

#undef NSL_ERROR_HANDLE
#define NSL_ERROR_HANDLE(E, ...)                                                                   \
    do {                                                                                           \
        if ((E) != NULL && (E)->code) {                                                            \
            { __VA_ARGS__; }                                                                       \
            if ((E)->code) {                                                                       \
                NSL_ERROR_RAISE(E);                                                                \
            }                                                                                      \
        }                                                                                          \
    } while (0)

#undef NSL_ERROR_EXCEPT_CASE
#define NSL_ERROR_EXCEPT_CASE(E, CODE, ...)                                                        \
    case CODE: {                                                                                   \
        __VA_ARGS__;                                                                               \
        NSL_ERROR_CLEAR(E);                                                                        \
        break;                                                                                     \
    }

#undef NSL_ERROR_PROPAGATE
#define NSL_ERROR_PROPAGATE(E, ...)                                                                \
    do {                                                                                           \
        if ((E) != NULL && (E)->code) {                                                            \
            { __VA_ARGS__; }                                                                       \
        }                                                                                          \
    } while (0)

#undef NSL_ERROR_CLEAR
#define NSL_ERROR_CLEAR(E)                                                                         \
    do {                                                                                           \
        *(E) = (nsl_Error){0};                                                                     \
    } while (0)

#undef NSL_ERROR_RAISE
#define NSL_ERROR_RAISE(E)                                                                         \
    do {                                                                                           \
        fprintf(stderr, NSL_ERROR_FMT "\n", NSL_ERROR_ARG(E));                                     \
        abort();                                                                                   \
    } while (0)

#undef NSL_ERROR_LOG
#define NSL_ERROR_LOG(E)                                                                           \
    do {                                                                                           \
        fprintf(stderr, NSL_ERROR_FMT "\n", NSL_ERROR_ARG(E));                                     \
    } while (0)
#endif // _NSL_ERROR_H_
