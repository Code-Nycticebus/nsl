#pragma once

#include "nc/defines.h"
#include "nc/core/arena.h"

nc_Bytes nc_bytes_from_parts(usize size, const void *data);

nc_Bytes nc_bytes_copy(nc_Bytes bytes, nc_Arena *arena);

nc_Bytes nc_bytes_slice(nc_Bytes bytes, usize idx1, size_t idx2);
nc_Bytes nc_bytes_take(nc_Bytes *bytes, usize count);

bool nc_bytes_eq(nc_Bytes b1, nc_Bytes b2);
u64 nc_bytes_hash(nc_Bytes bytes);

nc_Str nc_bytes_to_hex(nc_Bytes bytes, nc_Arena *arena);
nc_Bytes nc_bytes_from_hex(nc_Str s, nc_Arena *arena);
