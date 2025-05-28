#ifndef _NC_STR_H
#define _NC_STR_H

#include "nc/core/arena.h"
#include "nc/defines.h"

///////////////////////////////////////////////////////////////////////////////

#define NC_STR_NOT_FOUND SIZE_MAX

#define NC_STR_LETTERS NC_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define NC_STR_UPPERCASE NC_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define NC_STR_LOWERCASE NC_STR("abcdefghijklmnopqrstuvwxyz")
#define NC_STR_DIGITS NC_STR("0123456789")
#define NC_STR_HEXDIGITS NC_STR("0123456789abcdefABCDEF")
#define NC_STR_PUNCTUATION NC_STR("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~")
#define NC_STR_WHITESPACE NC_STR(" \t\n\r\x0b\x0c")

NC_API nc_Str nc_str_from_parts(usize size, const char *cstr);
NC_API nc_Str nc_str_from_bytes(nc_Bytes bytes);
NC_API nc_Bytes nc_str_to_bytes(nc_Str s);
NC_API nc_Str nc_str_from_cstr(const char *cstr);
NC_FMT(2) nc_Str nc_str_format(nc_Arena *arena, const char *fmt, ...);

NC_API nc_Str nc_str_copy(nc_Str s, nc_Arena *arena);
NC_API nc_Str nc_str_append(nc_Str s1, nc_Str suffix, nc_Arena *arena);
NC_API nc_Str nc_str_prepend(nc_Str s1, nc_Str prefix, nc_Arena *arena);
NC_API nc_Str nc_str_wrap(nc_Str s, nc_Str wrap, nc_Arena *arena);

// Inserts sep in between elements
NC_API nc_Str nc_str_join(nc_Str sep, usize count, nc_Str *s, nc_Arena *arena);
// Appends suffix to every element, even the last one
NC_API nc_Str nc_str_join_suffix(nc_Str suffix, usize count, nc_Str *s, nc_Arena *arena);
// Prepends prefix to every element, even the last one
NC_API nc_Str nc_str_join_prefix(nc_Str prefix, usize count, nc_Str *s, nc_Arena *arena);

NC_API nc_Str nc_str_join_wrap(nc_Str sep, nc_Str wrap, usize count, nc_Str *s, nc_Arena *arena);

NC_API nc_Str nc_str_upper(nc_Str s, nc_Arena *arena);
NC_API nc_Str nc_str_lower(nc_Str s, nc_Arena *arena);
NC_API nc_Str nc_str_replace(nc_Str s, nc_Str old, nc_Str new, nc_Arena *arena);
NC_API nc_Str nc_str_center(nc_Str s, usize width, char fillchar, nc_Arena *arena);
NC_API nc_Str nc_str_ljust(nc_Str s, usize width, char fillchar, nc_Arena *arena);
NC_API nc_Str nc_str_rjust(nc_Str s, usize width, char fillchar, nc_Arena *arena);
NC_API nc_Str nc_str_repeat(nc_Str s, usize count, nc_Arena *arena);
NC_API nc_Str nc_str_reverse(nc_Str s, nc_Arena *arena);

NC_API bool nc_str_eq(nc_Str s1, nc_Str s2);
NC_API bool nc_str_eq_ignorecase(nc_Str s1, nc_Str s2);
NC_API bool nc_str_startswith(nc_Str s1, nc_Str prefix);
NC_API bool nc_str_endswith(nc_Str s1, nc_Str suffix);
NC_API bool nc_str_endswith_predicate(nc_Str s1, bool (*predicate)(char));

NC_API bool nc_str_contains(nc_Str haystack, nc_Str needle);
NC_API bool nc_str_includes(nc_Str haystack, char needle);
NC_API bool nc_str_is_empty(nc_Str s);

NC_API nc_Str nc_str_trim_left(nc_Str s);
NC_API nc_Str nc_str_trim_left_by_delim(nc_Str s, char delim);
NC_API nc_Str nc_str_trim_left_by_predicate(nc_Str s, bool (*predicate)(char));

NC_API nc_Str nc_str_trim_right(nc_Str s);
NC_API nc_Str nc_str_trim_right_by_delim(nc_Str s, char delim);
NC_API nc_Str nc_str_trim_right_by_predicate(nc_Str s, bool (*predicate)(char));

NC_API nc_Str nc_str_trim(nc_Str s);
NC_API nc_Str nc_str_trim_by_delim(nc_Str s, char delim);
NC_API nc_Str nc_str_trim_by_predicate(nc_Str s, bool (*predicate)(char));

NC_API bool nc_str_try_chop_by_delim(nc_Str *s, char delim, nc_Str *chunk);
NC_API nc_Str nc_str_chop_by_delim(nc_Str *s, char delim);
NC_API bool nc_str_try_chop_by_predicate(nc_Str *s, bool (*predicate)(char), nc_Str *chunk);
NC_API nc_Str nc_str_chop_by_predicate(nc_Str *s, bool (*predicate)(char));
NC_API nc_Str nc_str_chop_right_by_delim(nc_Str *s, char delim);
NC_API nc_Str nc_str_chop_right_by_predicate(nc_Str *s, bool (*predicate)(char));
NC_API nc_Str nc_str_take(nc_Str *s, usize count);
NC_API bool nc_str_try_take(nc_Str *s, usize count, nc_Str *chunk);
NC_API nc_Str nc_str_take_right(nc_Str *s, usize count);
NC_API bool nc_str_try_take_right(nc_Str *s, usize count, nc_Str *chunk);

NC_API nc_Str nc_str_substring(nc_Str s, usize start, usize end);

NC_API u64 nc_str_u64(nc_Str s);
NC_API u64 nc_str_chop_u64(nc_Str *s);

NC_API i64 nc_str_i64(nc_Str s);
NC_API i64 nc_str_chop_i64(nc_Str *s);

NC_API f64 nc_str_f64(nc_Str s);
NC_API f64 nc_str_chop_f64(nc_Str *s);

// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NC_API usize nc_str_find(nc_Str haystack, nc_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NC_API usize nc_str_find_by_predicate(nc_Str haystack, bool (*predicate)(char));
// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NC_API usize nc_str_find_last(nc_Str haystack, nc_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NC_API usize nc_str_find_last_by_predicate(nc_Str haystack, bool (*predicate)(char));

NC_API usize nc_str_count(nc_Str haystack, nc_Str needle);
// Returns '\0' if the index is out of bounds.
NC_API char nc_str_getc(nc_Str s, usize idx);

// Basic FNV hash.
NC_API u64 nc_str_hash(nc_Str s);

#endif // _NC_STR_H
