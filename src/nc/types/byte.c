#include "nc/types/byte.h"

#include "nc/types/char.h"
#include "nc/types/int.h"
#include "nc/types/str.h"

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

NC_API nc_Bytes nc_bytes_from_parts(usize size, const void *data) {
    return (nc_Bytes){.size = size, .data = data};
}

///////////////////////////////////////////////////////////////////////////////

NC_API nc_Bytes nc_bytes_copy(nc_Bytes bytes, nc_Arena *arena) {
    u8 *buffer = nc_arena_alloc(arena, bytes.size);
    memcpy(buffer, bytes.data, bytes.size);
    return nc_bytes_from_parts(bytes.size, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NC_API nc_Bytes nc_bytes_slice(nc_Bytes bytes, usize idx1, usize idx2) {
    if (idx2 <= idx1 || bytes.size <= idx1 || bytes.size < idx2) {
        return nc_bytes_from_parts(0, bytes.data);
    }
    return nc_bytes_from_parts(idx2 - idx1, &bytes.data[idx1]);
}

NC_API nc_Bytes nc_bytes_take(nc_Bytes *bytes, usize count) {
    count = nc_usize_min(bytes->size, count);
    nc_Bytes ret = nc_bytes_from_parts(count, bytes->data);
    bytes->size -= count;
    bytes->data += count;
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

NC_API bool nc_bytes_eq(nc_Bytes b1, nc_Bytes b2) {
    if (b1.size != b2.size) return false;
    return memcmp(b1.data, b2.data, b1.size) == 0;
}

NC_API u64 nc_bytes_hash(nc_Bytes bytes) {
    const u64 offset = 2166136261UL;
    const u64 prime = 16777619;
    u64 hash = offset;
    for (usize i = 0; i < bytes.size; i++) {
        hash ^= (unsigned long)bytes.data[i];
        hash *= prime;
    }
    return hash;
}

///////////////////////////////////////////////////////////////////////////////

NC_API nc_Str nc_bytes_to_hex(nc_Bytes bytes, nc_Arena *arena) {
    char *buf = nc_arena_calloc(arena, bytes.size * 2 + 1);
    usize idx = 0;
    for (usize i = 0; i < bytes.size; i++) {
        idx += (usize)snprintf(&buf[idx], 3, "%0*x", (i != 0) + 1, bytes.data[i]);
    }
    return (nc_Str){.len = idx, .data = buf};
}

NC_API nc_Bytes nc_bytes_from_hex(nc_Str s, nc_Arena *arena) {
    if (nc_str_startswith(s, NC_STR("0x"))) {
        s = nc_str_substring(s, 2, s.len);
    }

    u8 *buffer = nc_arena_calloc(arena, (s.len / 2) + (s.len % 2));
    // to convert strings like "0x101".
    // in the first iteration:
    // take 1 or 2 chars depending if s.len is even or odd
    usize idx = 0;
    for (nc_Str ch = {0}; nc_str_try_take(&s, idx == 0 ? 2 - s.len % 2 : 2, &ch);) {
        for (usize i = 0; i < ch.len; i++) {
            buffer[idx] = (u8)(buffer[idx] << 4);
            if (nc_char_is_xdigit(ch.data[i])) {
                buffer[idx] |= nc_char_hex_to_u8(ch.data[i]);
            }
        }
        idx++;
    }
    return nc_bytes_from_parts(idx, buffer);
}

///////////////////////////////////////////////////////////////////////////////
