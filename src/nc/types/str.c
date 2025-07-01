#include "nc/types/str.h"

#include "nc/core/arena.h"
#include "nc/types/byte.h"
#include "nc/types/char.h"
#include "nc/types/int.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////

NC_API nc_Str nc_str_from_parts(usize size, const char *cstr) {
    return (nc_Str){.len = size, .data = cstr};
}

NC_API nc_Str nc_str_from_bytes(nc_Bytes bytes) {
    return nc_str_from_parts(bytes.size, (const char *)bytes.data);
}

NC_API nc_Bytes nc_str_to_bytes(nc_Str s) {
    return nc_bytes_from_parts(s.len, s.data);
}

NC_API nc_Str nc_str_from_cstr(const char *cstr) {
    return (nc_Str){.len = strlen(cstr), .data = cstr};
}

NC_API const char* nc_str_to_cstr(nc_Str str, nc_Arena* arena) {
    char* cstr = nc_arena_calloc(arena, str.len + 1);
    memcpy(cstr, str.data, str.len);
    return cstr;
}

NC_API nc_Str nc_str_format(nc_Arena *arena, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    usize size = (usize)vsnprintf(NULL, 0, fmt, va) + 1;
    va_end(va);

    char *buffer = nc_arena_calloc(arena, size);
    va_start(va, fmt);
    vsnprintf(buffer, size, fmt, va);
    va_end(va);
    return nc_str_from_parts(size - 1, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NC_API nc_Str nc_str_copy(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    memcpy(buffer, s.data, s.len);
    buffer[s.len] = '\0';
    return nc_str_from_parts(s.len, buffer);
}

NC_API nc_Str nc_str_append(nc_Str s, nc_Str suffix, nc_Arena *arena) {
    const usize new_size = s.len + suffix.len;
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], s.data, s.len);
    memcpy(&buffer[s.len], suffix.data, suffix.len);
    buffer[new_size] = '\0';
    return nc_str_from_parts(new_size, buffer);
}

NC_API nc_Str nc_str_prepend(nc_Str s, nc_Str prefix, nc_Arena *arena) {
    const usize new_size = s.len + prefix.len;
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], prefix.data, prefix.len);
    memcpy(&buffer[prefix.len], s.data, s.len);
    buffer[new_size] = '\0';
    return nc_str_from_parts(new_size, buffer);
}

NC_API nc_Str nc_str_wrap(nc_Str s, nc_Str wrap, nc_Arena *arena) {
    const usize new_size = s.len + wrap.len * 2;
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], wrap.data, wrap.len);
    memcpy(&buffer[wrap.len], s.data, s.len);
    memcpy(&buffer[wrap.len + s.len], wrap.data, wrap.len);
    buffer[new_size] = '\0';
    return nc_str_from_parts(new_size, buffer);
}

NC_API nc_Str nc_str_join(nc_Str sep, usize count, nc_Str *s, nc_Arena *arena) {
    if (count == 0) {
        return NC_STR("");
    }
    usize size = sep.len * (count - 1);
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        if (i != 0) {
            memcpy(&buffer[b_idx], sep.data, sep.len);
            b_idx += sep.len;
        }
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
    }
    buffer[size] = '\0';

    return nc_str_from_parts(size, buffer);
}

NC_API nc_Str nc_str_join_suffix(nc_Str suffix, usize count, nc_Str *s, nc_Arena *arena) {
    usize size = suffix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
        memcpy(&buffer[b_idx], suffix.data, suffix.len);
        b_idx += suffix.len;
    }
    buffer[size] = '\0';

    return nc_str_from_parts(size, buffer);
}

NC_API nc_Str nc_str_join_prefix(nc_Str prefix, usize count, nc_Str *s, nc_Arena *arena) {
    usize size = prefix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], prefix.data, prefix.len);
        b_idx += prefix.len;
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
    }
    buffer[size] = '\0';

    return nc_str_from_parts(size, buffer);
}

NC_API nc_Str nc_str_join_wrap(nc_Str sep, nc_Str wrap, usize count, nc_Str *s, nc_Arena *arena) {
    usize size = sep.len * (count - 1) + wrap.len * count * 2;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        if (i != 0) {
            memcpy(&buffer[b_idx], sep.data, sep.len);
            b_idx += sep.len;
        }
        memcpy(&buffer[b_idx], wrap.data, wrap.len);
        b_idx += wrap.len;
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
        memcpy(&buffer[b_idx], wrap.data, wrap.len);
        b_idx += wrap.len;
    }
    buffer[size] = '\0';

    return nc_str_from_parts(size, buffer);
}

NC_API nc_Str nc_str_upper(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nc_char_to_upper(s.data[i]);
    }
    return nc_str_from_parts(s.len, buffer);
}

NC_API nc_Str nc_str_lower(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nc_char_to_lower(s.data[i]);
    }
    return nc_str_from_parts(s.len, buffer);
}

NC_API nc_Str nc_str_replace(nc_Str s, nc_Str old, nc_Str new, nc_Arena *arena) {
    usize count = nc_str_count(s, old);
    usize new_size = (s.len - (old.len * count) + (count * new.len));
    char *buffer = nc_arena_alloc(arena, new_size + 1);
    buffer[new_size] = '\0';

    for (usize i = 0, j = 0; i < s.len;) {
        if (old.len <= s.len - i && memcmp(&s.data[i], old.data, old.len) == 0) {
            memcpy(&buffer[j], new.data, new.len);
            i += old.len;
            j += new.len;
        } else {
            buffer[j++] = s.data[i++];
        }
    }

    return nc_str_from_parts(new_size, buffer);
}

NC_API nc_Str nc_str_center(nc_Str s, usize width, char fillchar, nc_Arena *arena) {
    if (width < s.len) {
        return nc_str_copy(s, arena);
    }
    char *buffer = nc_arena_calloc(arena, width + 1);
    const usize left_width = (width - s.len) / 2;
    const usize right_width = (width - s.len - left_width);
    usize idx = 0;
    for (usize i = 0; i < left_width; i++) {
        buffer[idx++] = fillchar;
    }
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    for (usize i = 0; i < right_width; i++) {
        buffer[idx++] = fillchar;
    }
    return nc_str_from_parts(width, buffer);
}

NC_API nc_Str nc_str_ljust(nc_Str s, usize width, char fillchar, nc_Arena *arena) {
    if (width < s.len) {
        return nc_str_copy(s, arena);
    }
    char *buffer = nc_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    return nc_str_from_parts(width, buffer);
}

NC_API nc_Str nc_str_rjust(nc_Str s, usize width, char fillchar, nc_Arena *arena) {
    if (width < s.len) {
        return nc_str_copy(s, arena);
    }
    char *buffer = nc_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }

    return nc_str_from_parts(width, buffer);
}

NC_API nc_Str nc_str_repeat(nc_Str s, usize count, nc_Arena *arena) {
    usize len = s.len * count;
    char *buffer = nc_arena_alloc(arena, len + 1);
    buffer[len] = '\0';

    usize idx = 0;
    for (usize i = 0; i < count; i++) {
        for (usize j = 0; j < s.len; j++) {
            buffer[idx++] = s.data[j];
        }
    }

    return nc_str_from_parts(len, buffer);
}

NC_API nc_Str nc_str_reverse(nc_Str s, nc_Arena *arena) {
    char *buffer = nc_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = s.data[s.len - i - 1];
    }
    return nc_str_from_parts(s.len, buffer);
}

///////////////////////////////////////////////////////////////////////////////

NC_API bool nc_str_eq(nc_Str s1, nc_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    return memcmp(s1.data, s2.data, s1.len) == 0;
}

NC_API bool nc_str_eq_ignorecase(nc_Str s1, nc_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    for (size_t i = 0; i < s1.len; i++) {
        if (nc_char_to_lower(s1.data[i]) != nc_char_to_lower(s2.data[i])) {
            return false;
        }
    }
    return true;
}

NC_API bool nc_str_startswith(nc_Str s1, nc_Str prefix) {
    if (s1.len < prefix.len) {
        return false;
    }
    return memcmp(s1.data, prefix.data, prefix.len) == 0;
}

NC_API bool nc_str_endswith(nc_Str s1, nc_Str suffix) {
    if (s1.len < suffix.len) {
        return false;
    }
    const usize idx = s1.len - suffix.len;
    return memcmp(&s1.data[idx], suffix.data, suffix.len) == 0;
}

NC_API bool nc_str_endswith_predicate(nc_Str s1, bool (*predicate)(char)) {
    if (s1.len == 0) {
        return false;
    }
    const usize idx = s1.len - 1;
    return predicate(s1.data[idx]);
}

NC_API bool nc_str_contains(nc_Str haystack, nc_Str needle) {
    if (haystack.len < needle.len) {
        return false;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            return true;
        }
    }
    return false;
}

NC_API bool nc_str_includes(nc_Str haystack, char needle) {
    if (haystack.len == 0) {
        return false;
    }
    for (usize i = 0; i < haystack.len; i++) {
        if (haystack.data[i] == needle) {
            return true;
        }
    }
    return false;
}

NC_API bool nc_str_is_empty(nc_Str s) {
    return s.len == 0;
}

NC_API nc_Str nc_str_trim_left(nc_Str s) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && nc_char_is_space(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NC_API nc_Str nc_str_trim_left_by_delim(nc_Str s, char delim) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && s.data[i] == delim; ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NC_API nc_Str nc_str_trim_left_by_predicate(nc_Str s, bool (*predicate)(char)) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NC_API nc_Str nc_str_trim_right(nc_Str s) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && nc_char_is_space(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}

NC_API nc_Str nc_str_trim_right_by_delim(nc_Str s, char delim) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && s.data[s.len - i - 1] == delim; ++i) {
        result.len--;
    }
    return result;
}

NC_API nc_Str nc_str_trim_right_by_predicate(nc_Str s, bool (*predicate)(char)) {
    nc_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}

NC_API nc_Str nc_str_trim(nc_Str s) {
    return nc_str_trim_left(nc_str_trim_right(s));
}

NC_API nc_Str nc_str_trim_by_delim(nc_Str s, char delim) {
    return nc_str_trim_left_by_delim(nc_str_trim_right_by_delim(s, delim), delim);
}

NC_API nc_Str nc_str_trim_by_predicate(nc_Str s, bool (*predicate)(char)) {
    return nc_str_trim_left_by_predicate(nc_str_trim_right_by_predicate(s, predicate), predicate);
}

NC_API bool nc_str_try_chop_by_delim(nc_Str *s, char delim, nc_Str *chunk) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        if (chunk) *chunk = nc_str_from_parts(i, s->data);
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return true;
    }
    return false;
}

NC_API nc_Str nc_str_chop_by_delim(nc_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, s->data);
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return chunk;
    }
    return *s;
}

NC_API bool nc_str_try_chop_by_predicate(nc_Str *s, bool (*predicate)(char), nc_Str *chunk) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        if (chunk) *chunk = nc_str_from_parts(i, s->data);
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return true;
    }
    return false;
}

NC_API nc_Str nc_str_chop_by_predicate(nc_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, s->data);
        const usize new_len = nc_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return chunk;
    }
    return *s;
}

NC_API nc_Str nc_str_chop_right_by_delim(nc_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[s->len - i - 1] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nc_usize_min(s->len, i + 1);
        return chunk;
    }
    return *s;
}

NC_API nc_Str nc_str_chop_right_by_predicate(nc_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[s->len - i - 1])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nc_Str chunk = nc_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nc_usize_min(s->len, i + 1);
        return chunk;
    }
    return *s;
}

NC_API nc_Str nc_str_substring(nc_Str s, usize start, usize end) {
    if (end <= start || s.len <= start || s.len < end) {
        return NC_STR("");
    }
    return nc_str_from_parts(end - start, &s.data[start]);
}

NC_API nc_Str nc_str_take(nc_Str *s, usize count) {
    count = nc_usize_min(s->len, count);
    nc_Str ret = nc_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return ret;
}

NC_API bool nc_str_try_take(nc_Str *s, usize count, nc_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nc_usize_min(s->len, count);
    *chunk = nc_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return true;
}

NC_API nc_Str nc_str_take_right(nc_Str *s, usize count) {
    count = nc_usize_min(s->len, count);
    nc_Str ret = nc_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return ret;
}

NC_API bool nc_str_try_take_right(nc_Str *s, usize count, nc_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nc_usize_min(s->len, count);
    *chunk = nc_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return true;
}

NC_API u64 nc_str_u64(nc_Str s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(s, &arena);
    u64 value = strtoull(owned.data, NULL, radix);
    nc_arena_free(&arena);
    return value;
}

NC_API u64 nc_str_chop_u64(nc_Str *s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(*s, &arena);
    char *endptr;
    u64 value = strtoull(owned.data, &endptr, radix);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nc_arena_free(&arena);
    return value;
}

NC_API i64 nc_str_i64(nc_Str s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(s, &arena);
    i64 value = strtoll(owned.data, NULL, radix);
    nc_arena_free(&arena);
    return value;
}

NC_API i64 nc_str_chop_i64(nc_Str *s) {
    const int radix = 10;
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(*s, &arena);
    char *endptr;
    i64 value = strtoll(owned.data, &endptr, radix);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nc_arena_free(&arena);
    return value;
}

NC_API f64 nc_str_f64(nc_Str s) {
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(s, &arena);
    double value = strtod(owned.data, NULL);
    nc_arena_free(&arena);
    return value;
}

NC_API f64 nc_str_chop_f64(nc_Str *s) {
    nc_Arena arena = {0};
    nc_Str owned = nc_str_copy(*s, &arena);

    char *endptr;
    f64 value = strtod(owned.data, &endptr);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nc_arena_free(&arena);
    return value;
}

NC_API usize nc_str_find(nc_Str haystack, nc_Str needle) {
    if (haystack.len < needle.len) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            return i;
        }
    }
    return NC_STR_NOT_FOUND;
}


NC_API usize nc_str_find_by_predicate(nc_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len; i++) {
        if (predicate(haystack.data[i])) {
            return i;
        }
    }
    return NC_STR_NOT_FOUND;
}

NC_API usize nc_str_find_last(nc_Str haystack, nc_Str needle) {
    if (haystack.len < needle.len) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = haystack.len - needle.len + 1; i > 0; i--) {
        if (memcmp(&haystack.data[i - 1], needle.data, needle.len) == 0) {
            return i - 1;
        }
    }
    return NC_STR_NOT_FOUND;
}


NC_API usize nc_str_find_last_by_predicate(nc_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NC_STR_NOT_FOUND;
    }
    for (usize i = haystack.len; i > 0; i--) {
        if (predicate(haystack.data[i - 1])) {
            return i - 1;
        }
    }
    return NC_STR_NOT_FOUND;
}

NC_API usize nc_str_count(nc_Str haystack, nc_Str needle) {
    usize count = 0;
    if (haystack.len < needle.len) {
        return count;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            count++;
            i += needle.len - 1;
        }
    }
    return count;
}

NC_API char nc_str_getc(nc_Str s, usize idx) {
    if (s.len <= idx) {
        return '\0';
    }
    return s.data[idx];
}

NC_API u64 nc_str_hash(nc_Str s) {
    const uint64_t magic_prime = 0x00000100000001b3;
    uint64_t hash = 0xcbf29ce484222325; // NOLINT
    for (usize i = 0; i < s.len; ++i) {
        hash = (hash ^ (u64)s.data[i]) * magic_prime;
    }
    return hash;
}
