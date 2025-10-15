#ifndef _NSL_BUILDER_H_
#define _NSL_BUILDER_H_

#include "nsl/defines.h"

#define nsl_sb_push_c(sb, c) nsl_list_push(sb, c)
#define nsl_sb_push_cstr(sb, cstr) nsl_list_extend(sb, strlen(cstr), cstr)
#define nsl_sb_push_str(sb, str) nsl_list_extend(sb, (str).len, (str).data)
#define nsl_sb_push_fmt(sb, ...) do {\
    i32 size = snprintf(NULL, 0, __VA_ARGS__);\
    nsl_list_reserve(sb, size + 1);\
    snprintf(&(sb)->items[(sb)->len ? (sb)->len-1 : 0], size + 1, __VA_ARGS__);\
    (sb)->len += size;\
} while(0);

#endif // _NSL_BUILDER_H_
