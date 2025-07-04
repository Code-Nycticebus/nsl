#include "nsl/types/byte.h"

#include "nsl/types/char.h"
#include "nsl/types/int.h"
#include "nsl/types/str.h"

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Bytes nsl_bytes_from_parts(usize size, const void *data) {
    return (nsl_Bytes){.size = size, .data = data};
}

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Bytes nsl_bytes_copy(nsl_Bytes bytes, nsl_Arena *arena) {
    u8 *buffer = nsl_arena_alloc(arena, bytes.size);
    memcpy(buffer, bytes.data, bytes.size);
    return nsl_bytes_from_parts(bytes.size, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NSL_API nsl_Bytes nsl_bytes_slice(nsl_Bytes bytes, usize idx1, usize idx2) {
    if (idx2 <= idx1 || bytes.size <= idx1 || bytes.size < idx2) {
        return nsl_bytes_from_parts(0, bytes.data);
    }
    return nsl_bytes_from_parts(idx2 - idx1, &bytes.data[idx1]);
}

NSL_API nsl_Bytes nsl_bytes_take(nsl_Bytes *bytes, usize count) {
    count = nsl_usize_min(bytes->size, count);
    nsl_Bytes ret = nsl_bytes_from_parts(count, bytes->data);
    bytes->size -= count;
    bytes->data += count;
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

NSL_API bool nsl_bytes_eq(nsl_Bytes b1, nsl_Bytes b2) {
    if (b1.size != b2.size) return false;
    return memcmp(b1.data, b2.data, b1.size) == 0;
}

NSL_API u64 nsl_bytes_hash(nsl_Bytes bytes) {
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

NSL_API nsl_Str nsl_bytes_to_hex(nsl_Bytes bytes, nsl_Arena *arena) {
    char *buf = nsl_arena_calloc(arena, bytes.size * 2 + 1);
    usize idx = 0;
    for (usize i = 0; i < bytes.size; i++) {
        idx += (usize)snprintf(&buf[idx], 3, "%0*x", (i != 0) + 1, bytes.data[i]);
    }
    return (nsl_Str){.len = idx, .data = buf};
}

NSL_API nsl_Bytes nsl_bytes_from_hex(nsl_Str s, nsl_Arena *arena) {
    if (nsl_str_startswith(s, NSL_STR("0x"))) {
        s = nsl_str_substring(s, 2, s.len);
    }

    u8 *buffer = nsl_arena_calloc(arena, (s.len / 2) + (s.len % 2));
    // to convert strings like "0x101".
    // in the first iteration:
    // take 1 or 2 chars depending if s.len is even or odd
    usize idx = 0;
    for (nsl_Str ch = {0}; nsl_str_try_take(&s, idx == 0 ? 2 - s.len % 2 : 2, &ch);) {
        for (usize i = 0; i < ch.len; i++) {
            buffer[idx] = (u8)(buffer[idx] << 4);
            if (nsl_char_is_xdigit(ch.data[i])) {
                buffer[idx] |= nsl_char_hex_to_u8(ch.data[i]);
            }
        }
        idx++;
    }
    return nsl_bytes_from_parts(idx, buffer);
}

///////////////////////////////////////////////////////////////////////////////
