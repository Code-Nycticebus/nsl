#ifndef _NSL_BYTE_H_
#define _NSL_BYTE_H_

#include "nsl/defines.h"
#include "nsl/core/arena.h"

NSL_API nsl_Bytes nsl_bytes_from_parts(usize size, const void *data);

NSL_API nsl_Bytes nsl_bytes_copy(nsl_Bytes bytes, nsl_Arena *arena);

NSL_API nsl_Bytes nsl_bytes_slice(nsl_Bytes bytes, usize idx1, size_t idx2);
NSL_API nsl_Bytes nsl_bytes_take(nsl_Bytes *bytes, usize count);

NSL_API bool nsl_bytes_eq(nsl_Bytes b1, nsl_Bytes b2);
NSL_API u64 nsl_bytes_hash(nsl_Bytes bytes);

NSL_API nsl_Str nsl_bytes_to_hex(nsl_Bytes bytes, nsl_Arena *arena);
NSL_API nsl_Bytes nsl_bytes_from_hex(nsl_Str s, nsl_Arena *arena);

#endif // _NSL_BYTE_H_
