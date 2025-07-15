#ifndef _NSL_ERROR_H_
#define _NSL_ERROR_H_

#include "nsl/defines.h"

#include <stdlib.h>
#include <stdio.h>

#define NSL_ERROR_FMT "%s:%d: %s():"
#define NSL_ERROR_ARG __FILE__, __LINE__, __func__

// nsl_Error
enum {
    NSL_ERROR = -1,
    NSL_NO_ERROR = 0,
    NSL_ERROR_FILE_NOT_FOUND = 256,
    NSL_ERROR_ACCESS_DENIED,
    NSL_ERROR_ALREADY_EXISTS,
    NSL_ERROR_NOT_DIRECTORY,
    NSL_ERROR_IS_DIRECTORY,
    NSL_ERROR_FILE_BUSY,
};

#endif // _NSL_ERROR_H_
