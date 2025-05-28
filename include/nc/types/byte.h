#ifndef _NC_BYTE_H_
#define _NC_BYTE_H_

#include "nc/defines.h"
#include "nc/core/arena.h"

NC_API nc_Bytes nc_bytes_from_parts(usize size, const void *data);

NC_API nc_Bytes nc_bytes_copy(nc_Bytes bytes, nc_Arena *arena);

NC_API nc_Bytes nc_bytes_slice(nc_Bytes bytes, usize idx1, size_t idx2);
NC_API nc_Bytes nc_bytes_take(nc_Bytes *bytes, usize count);

NC_API bool nc_bytes_eq(nc_Bytes b1, nc_Bytes b2);
NC_API u64 nc_bytes_hash(nc_Bytes bytes);

NC_API nc_Str nc_bytes_to_hex(nc_Bytes bytes, nc_Arena *arena);
NC_API nc_Bytes nc_bytes_from_hex(nc_Str s, nc_Arena *arena);

#endif // _NC_BYTE_H_
