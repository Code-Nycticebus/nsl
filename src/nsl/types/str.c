#include "nsl/types/str.h"

#include "nsl/core/arena.h"
#include "nsl/types/byte.h"
#include "nsl/types/char.h"
#include "nsl/types/int.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NSL_API nsl_Str nsl_str_from_parts(usize size, const char *cstr) {
    return (nsl_Str){.len = size, .data = cstr};
}

NSL_API nsl_Str nsl_str_from_bytes(nsl_Bytes bytes) {
    return nsl_str_from_parts(bytes.size, (const char *)bytes.data);
}

NSL_API nsl_Bytes nsl_str_to_bytes(nsl_Str s) {
    return nsl_bytes_from_parts(s.len, s.data);
}

NSL_API nsl_Str nsl_str_from_cstr(const char *cstr) {
    return (nsl_Str){.len = strlen(cstr), .data = cstr};
}

NSL_API const char* nsl_str_to_cstr(nsl_Str str, nsl_Arena* arena) {
    char* cstr = nsl_arena_calloc(arena, str.len + 1);
    memcpy(cstr, str.data, str.len);
    return cstr;
}

NSL_API nsl_Str nsl_str_format(nsl_Arena *arena, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    usize size = (usize)vsnprintf(NULL, 0, fmt, va) + 1;
    va_end(va);

    char *buffer = nsl_arena_calloc(arena, size);
    va_start(va, fmt);
    vsnprintf(buffer, size, fmt, va);
    va_end(va);
    return nsl_str_from_parts(size - 1, buffer);
}

NSL_API nsl_Str nsl_str_copy(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    memcpy(buffer, s.data, s.len);
    buffer[s.len] = '\0';
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API nsl_Str nsl_str_append(nsl_Str s, nsl_Str suffix, nsl_Arena *arena) {
    const usize new_size = s.len + suffix.len;
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], s.data, s.len);
    memcpy(&buffer[s.len], suffix.data, suffix.len);
    buffer[new_size] = '\0';
    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_prepend(nsl_Str s, nsl_Str prefix, nsl_Arena *arena) {
    const usize new_size = s.len + prefix.len;
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], prefix.data, prefix.len);
    memcpy(&buffer[prefix.len], s.data, s.len);
    buffer[new_size] = '\0';
    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_wrap(nsl_Str s, nsl_Str wrap, nsl_Arena *arena) {
    const usize new_size = s.len + wrap.len * 2;
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
    memcpy(&buffer[0], wrap.data, wrap.len);
    memcpy(&buffer[wrap.len], s.data, s.len);
    memcpy(&buffer[wrap.len + s.len], wrap.data, wrap.len);
    buffer[new_size] = '\0';
    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_join(nsl_Str sep, usize count, nsl_Str *s, nsl_Arena *arena) {
    if (count == 0) {
        return NSL_STR("");
    }
    usize size = sep.len * (count - 1);
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
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

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_join_suffix(nsl_Str suffix, usize count, nsl_Str *s, nsl_Arena *arena) {
    usize size = suffix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
        memcpy(&buffer[b_idx], suffix.data, suffix.len);
        b_idx += suffix.len;
    }
    buffer[size] = '\0';

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_join_prefix(nsl_Str prefix, usize count, nsl_Str *s, nsl_Arena *arena) {
    usize size = prefix.len * count;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < count; i++) {
        memcpy(&buffer[b_idx], prefix.data, prefix.len);
        b_idx += prefix.len;
        memcpy(&buffer[b_idx], s[i].data, s[i].len);
        b_idx += s[i].len;
    }
    buffer[size] = '\0';

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_join_wrap(nsl_Str sep, nsl_Str wrap, usize count, nsl_Str *s, nsl_Arena *arena) {
    usize size = sep.len * (count - 1) + wrap.len * count * 2;
    for (usize i = 0; i < count; i++) {
        size += s[i].len;
    }
    char *buffer = nsl_arena_alloc(arena, size + 1);
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

    return nsl_str_from_parts(size, buffer);
}

NSL_API nsl_Str nsl_str_upper(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nsl_char_to_upper(s.data[i]);
    }
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API nsl_Str nsl_str_lower(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = nsl_char_to_lower(s.data[i]);
    }
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API nsl_Str nsl_str_replace(nsl_Str s, nsl_Str old, nsl_Str new, nsl_Arena *arena) {
    usize count = nsl_str_count(s, old);
    usize new_size = (s.len - (old.len * count) + (count * new.len));
    char *buffer = nsl_arena_alloc(arena, new_size + 1);
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

    return nsl_str_from_parts(new_size, buffer);
}

NSL_API nsl_Str nsl_str_center(nsl_Str s, usize width, char fillchar, nsl_Arena *arena) {
    if (width < s.len) {
        return nsl_str_copy(s, arena);
    }
    char *buffer = nsl_arena_calloc(arena, width + 1);
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
    return nsl_str_from_parts(width, buffer);
}

NSL_API nsl_Str nsl_str_ljust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena) {
    if (width < s.len) {
        return nsl_str_copy(s, arena);
    }
    char *buffer = nsl_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    return nsl_str_from_parts(width, buffer);
}

NSL_API nsl_Str nsl_str_rjust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena) {
    if (width < s.len) {
        return nsl_str_copy(s, arena);
    }
    char *buffer = nsl_arena_calloc(arena, width + 1);
    usize idx = 0;
    for (usize i = 0; i < width - s.len; i++) {
        buffer[idx++] = fillchar;
    }
    for (usize i = 0; i < s.len; i++) {
        buffer[idx++] = s.data[i];
    }

    return nsl_str_from_parts(width, buffer);
}

NSL_API nsl_Str nsl_str_repeat(nsl_Str s, usize count, nsl_Arena *arena) {
    usize len = s.len * count;
    char *buffer = nsl_arena_alloc(arena, len + 1);
    buffer[len] = '\0';

    usize idx = 0;
    for (usize i = 0; i < count; i++) {
        for (usize j = 0; j < s.len; j++) {
            buffer[idx++] = s.data[j];
        }
    }

    return nsl_str_from_parts(len, buffer);
}

NSL_API nsl_Str nsl_str_reverse(nsl_Str s, nsl_Arena *arena) {
    char *buffer = nsl_arena_alloc(arena, s.len + 1);
    buffer[s.len] = '\0';
    for (usize i = 0; i < s.len; i++) {
        buffer[i] = s.data[s.len - i - 1];
    }
    return nsl_str_from_parts(s.len, buffer);
}

NSL_API bool nsl_str_eq(nsl_Str s1, nsl_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    return memcmp(s1.data, s2.data, s1.len) == 0;
}

NSL_API bool nsl_str_eq_ignorecase(nsl_Str s1, nsl_Str s2) {
    if (s1.len != s2.len) {
        return false;
    }
    for (size_t i = 0; i < s1.len; i++) {
        if (nsl_char_to_lower(s1.data[i]) != nsl_char_to_lower(s2.data[i])) {
            return false;
        }
    }
    return true;
}

NSL_API bool nsl_str_startswith(nsl_Str s1, nsl_Str prefix) {
    if (s1.len < prefix.len) {
        return false;
    }
    return memcmp(s1.data, prefix.data, prefix.len) == 0;
}

NSL_API bool nsl_str_endswith(nsl_Str s1, nsl_Str suffix) {
    if (s1.len < suffix.len) {
        return false;
    }
    const usize idx = s1.len - suffix.len;
    return memcmp(&s1.data[idx], suffix.data, suffix.len) == 0;
}

NSL_API bool nsl_str_endswith_predicate(nsl_Str s1, bool (*predicate)(char)) {
    if (s1.len == 0) {
        return false;
    }
    const usize idx = s1.len - 1;
    return predicate(s1.data[idx]);
}

NSL_API bool nsl_str_contains(nsl_Str haystack, nsl_Str needle) {
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

NSL_API bool nsl_str_includes(nsl_Str haystack, char needle) {
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

NSL_API bool nsl_str_is_empty(nsl_Str s) {
    return s.len == 0;
}

NSL_API nsl_Str nsl_str_trim_left(nsl_Str s) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && nsl_char_is_space(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_left_by_delim(nsl_Str s, char delim) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && s.data[i] == delim; ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_left_by_predicate(nsl_Str s, bool (*predicate)(char)) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[i]); ++i) {
        result.data++;
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_right(nsl_Str s) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && nsl_char_is_space(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_right_by_delim(nsl_Str s, char delim) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && s.data[s.len - i - 1] == delim; ++i) {
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim_right_by_predicate(nsl_Str s, bool (*predicate)(char)) {
    nsl_Str result = s;
    for (usize i = 0; i < s.len && predicate(s.data[s.len - i - 1]); ++i) {
        result.len--;
    }
    return result;
}

NSL_API nsl_Str nsl_str_trim(nsl_Str s) {
    return nsl_str_trim_left(nsl_str_trim_right(s));
}

NSL_API nsl_Str nsl_str_trim_by_delim(nsl_Str s, char delim) {
    return nsl_str_trim_left_by_delim(nsl_str_trim_right_by_delim(s, delim), delim);
}

NSL_API nsl_Str nsl_str_trim_by_predicate(nsl_Str s, bool (*predicate)(char)) {
    return nsl_str_trim_left_by_predicate(nsl_str_trim_right_by_predicate(s, predicate), predicate);
}

NSL_API bool nsl_str_try_chop_by_delim(nsl_Str *s, char delim, nsl_Str *chunk) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        if (chunk) *chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return true;
    }
    return false;
}

NSL_API nsl_Str nsl_str_chop_by_delim(nsl_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[i] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return chunk;
    }
    return *s;
}

NSL_API bool nsl_str_try_chop_by_predicate(nsl_Str *s, bool (*predicate)(char), nsl_Str *chunk) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        if (chunk) *chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return true;
    }
    return false;
}

NSL_API nsl_Str nsl_str_chop_by_predicate(nsl_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[i])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, s->data);
        const usize new_len = nsl_usize_min(s->len, i + 1);
        s->data += new_len;
        s->len -= new_len;
        return chunk;
    }
    return *s;
}

NSL_API nsl_Str nsl_str_chop_right_by_delim(nsl_Str *s, char delim) {
    usize i = 0;
    while (i < s->len && s->data[s->len - i - 1] != delim) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nsl_usize_min(s->len, i + 1);
        return chunk;
    }
    return *s;
}

NSL_API nsl_Str nsl_str_chop_right_by_predicate(nsl_Str *s, bool (*predicate)(char)) {
    usize i = 0;
    while (i < s->len && !predicate(s->data[s->len - i - 1])) {
        ++i;
    }

    if (s->len && i <= s->len) {
        nsl_Str chunk = nsl_str_from_parts(i, &s->data[s->len - i]);
        s->len -= nsl_usize_min(s->len, i + 1);
        return chunk;
    }
    return *s;
}

NSL_API nsl_Str nsl_str_substring(nsl_Str s, usize start, usize end) {
    if (end <= start || s.len <= start || s.len < end) {
        return NSL_STR("");
    }
    return nsl_str_from_parts(end - start, &s.data[start]);
}

NSL_API nsl_Str nsl_str_take(nsl_Str *s, usize count) {
    count = nsl_usize_min(s->len, count);
    nsl_Str ret = nsl_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return ret;
}

NSL_API bool nsl_str_try_take(nsl_Str *s, usize count, nsl_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nsl_usize_min(s->len, count);
    *chunk = nsl_str_from_parts(count, s->data);
    s->len -= count;
    s->data += count;
    return true;
}

NSL_API nsl_Str nsl_str_take_right(nsl_Str *s, usize count) {
    count = nsl_usize_min(s->len, count);
    nsl_Str ret = nsl_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return ret;
}

NSL_API bool nsl_str_try_take_right(nsl_Str *s, usize count, nsl_Str *chunk) {
    if (s->len == 0) {
        return false;
    }
    count = nsl_usize_min(s->len, count);
    *chunk = nsl_str_from_parts(count, &s->data[s->len - count]);
    s->len -= count;
    return true;
}

NSL_API u64 nsl_str_u64(nsl_Str s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(s, &arena);
    u64 value = strtoull(owned.data, NULL, radix);
    nsl_arena_free(&arena);
    return value;
}

NSL_API u64 nsl_str_chop_u64(nsl_Str *s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(*s, &arena);
    char *endptr;
    u64 value = strtoull(owned.data, &endptr, radix);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nsl_arena_free(&arena);
    return value;
}

NSL_API i64 nsl_str_i64(nsl_Str s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(s, &arena);
    i64 value = strtoll(owned.data, NULL, radix);
    nsl_arena_free(&arena);
    return value;
}

NSL_API i64 nsl_str_chop_i64(nsl_Str *s) {
    const int radix = 10;
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(*s, &arena);
    char *endptr;
    i64 value = strtoll(owned.data, &endptr, radix);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nsl_arena_free(&arena);
    return value;
}

NSL_API f64 nsl_str_f64(nsl_Str s) {
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(s, &arena);
    double value = strtod(owned.data, NULL);
    nsl_arena_free(&arena);
    return value;
}

NSL_API f64 nsl_str_chop_f64(nsl_Str *s) {
    nsl_Arena arena = {0};
    nsl_Str owned = nsl_str_copy(*s, &arena);

    char *endptr;
    f64 value = strtod(owned.data, &endptr);

    const usize size = (usize)(endptr - owned.data);
    s->data += size;
    s->len -= size;

    nsl_arena_free(&arena);
    return value;
}

NSL_API usize nsl_str_find(nsl_Str haystack, nsl_Str needle) {
    if (haystack.len < needle.len) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len - needle.len + 1; i++) {
        if (memcmp(&haystack.data[i], needle.data, needle.len) == 0) {
            return i;
        }
    }
    return NSL_STR_NOT_FOUND;
}


NSL_API usize nsl_str_find_by_predicate(nsl_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = 0; i < haystack.len; i++) {
        if (predicate(haystack.data[i])) {
            return i;
        }
    }
    return NSL_STR_NOT_FOUND;
}

NSL_API usize nsl_str_find_last(nsl_Str haystack, nsl_Str needle) {
    if (haystack.len < needle.len) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = haystack.len - needle.len + 1; i > 0; i--) {
        if (memcmp(&haystack.data[i - 1], needle.data, needle.len) == 0) {
            return i - 1;
        }
    }
    return NSL_STR_NOT_FOUND;
}


NSL_API usize nsl_str_find_last_by_predicate(nsl_Str haystack, bool (*predicate)(char)) {
    if (haystack.len == 0) {
        return NSL_STR_NOT_FOUND;
    }
    for (usize i = haystack.len; i > 0; i--) {
        if (predicate(haystack.data[i - 1])) {
            return i - 1;
        }
    }
    return NSL_STR_NOT_FOUND;
}

NSL_API usize nsl_str_count(nsl_Str haystack, nsl_Str needle) {
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

NSL_API char nsl_str_getc(nsl_Str s, usize idx) {
    if (s.len <= idx) {
        return '\0';
    }
    return s.data[idx];
}

NSL_API u64 nsl_str_hash(nsl_Str s) {
    const uint64_t magic_prime = 0x00000100000001b3;
    uint64_t hash = 0xcbf29ce484222325;
    for (usize i = 0; i < s.len; ++i) {
        hash = (hash ^ (u64)s.data[i]) * magic_prime;
    }
    return hash;
}
