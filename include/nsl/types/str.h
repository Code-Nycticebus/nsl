#ifndef _NSL_STR_H_
#define _NSL_STR_H_

#include "nsl/defines.h"

#define NSL_STR_NOT_FOUND ((usize)-1)

#define NSL_STR_LETTERS     NSL_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define NSL_STR_UPPERCASE   NSL_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define NSL_STR_LOWERCASE   NSL_STR("abcdefghijklmnopqrstuvwxyz")
#define NSL_STR_DIGITS      NSL_STR("0123456789")
#define NSL_STR_HEXDIGITS   NSL_STR("0123456789abcdefABCDEF")
#define NSL_STR_PUNCTUATION NSL_STR("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~")
#define NSL_STR_WHITESPACE  NSL_STR(" \t\n\r\x0b\x0c")

NSL_API nsl_Str nsl_str_from_parts(usize size, const char *cstr);
NSL_API nsl_Str nsl_str_from_bytes(nsl_Bytes bytes);
NSL_API nsl_Bytes nsl_str_to_bytes(nsl_Str s);
NSL_API nsl_Str nsl_str_from_cstr(const char *cstr);
NSL_API const char* nsl_str_to_cstr(nsl_Str str, nsl_Arena* arena);
NSL_API NSL_FMT(2) nsl_Str nsl_str_format(nsl_Arena *arena, const char *fmt, ...);

NSL_API nsl_Str nsl_str_copy(nsl_Str s, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_append(nsl_Str s1, nsl_Str suffix, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_prepend(nsl_Str s1, nsl_Str prefix, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_wrap(nsl_Str s, nsl_Str wrap, nsl_Arena *arena);

// Inserts sep in between elements
NSL_API nsl_Str nsl_str_join(nsl_Str sep, usize count, nsl_Str *s, nsl_Arena *arena);
// Appends suffix to every element, even the last one
NSL_API nsl_Str nsl_str_join_suffix(nsl_Str suffix, usize count, nsl_Str *s, nsl_Arena *arena);
// Prepends prefix to every element, even the last one
NSL_API nsl_Str nsl_str_join_prefix(nsl_Str prefix, usize count, nsl_Str *s, nsl_Arena *arena);
// Joins and wraps the elements
NSL_API nsl_Str nsl_str_join_wrap(nsl_Str sep, nsl_Str wrap, usize count, nsl_Str *s, nsl_Arena *arena);

NSL_API nsl_Str nsl_str_upper(nsl_Str s, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_lower(nsl_Str s, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_replace(nsl_Str s, nsl_Str old, nsl_Str new, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_center(nsl_Str s, usize width, char fillchar, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_ljust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_rjust(nsl_Str s, usize width, char fillchar, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_repeat(nsl_Str s, usize count, nsl_Arena *arena);
NSL_API nsl_Str nsl_str_reverse(nsl_Str s, nsl_Arena *arena);

NSL_API bool nsl_str_eq(nsl_Str s1, nsl_Str s2);
NSL_API bool nsl_str_eq_ignorecase(nsl_Str s1, nsl_Str s2);
NSL_API bool nsl_str_startswith(nsl_Str s1, nsl_Str prefix);
NSL_API bool nsl_str_endswith(nsl_Str s1, nsl_Str suffix);
NSL_API bool nsl_str_endswith_predicate(nsl_Str s1, bool (*predicate)(char));

NSL_API bool nsl_str_contains(nsl_Str haystack, nsl_Str needle);
NSL_API bool nsl_str_includes(nsl_Str haystack, char needle);
NSL_API bool nsl_str_is_empty(nsl_Str s);

NSL_API nsl_Str nsl_str_trim_left(nsl_Str s);
NSL_API nsl_Str nsl_str_trim_left_by_delim(nsl_Str s, char delim);
NSL_API nsl_Str nsl_str_trim_left_by_predicate(nsl_Str s, bool (*predicate)(char));

NSL_API nsl_Str nsl_str_trim_right(nsl_Str s);
NSL_API nsl_Str nsl_str_trim_right_by_delim(nsl_Str s, char delim);
NSL_API nsl_Str nsl_str_trim_right_by_predicate(nsl_Str s, bool (*predicate)(char));

NSL_API nsl_Str nsl_str_trim(nsl_Str s);
NSL_API nsl_Str nsl_str_trim_by_delim(nsl_Str s, char delim);
NSL_API nsl_Str nsl_str_trim_by_predicate(nsl_Str s, bool (*predicate)(char));

NSL_API bool nsl_str_try_chop_by_delim(nsl_Str *s, char delim, nsl_Str *chunk);
NSL_API nsl_Str nsl_str_chop_by_delim(nsl_Str *s, char delim);
NSL_API bool nsl_str_try_chop_by_predicate(nsl_Str *s, bool (*predicate)(char), nsl_Str *chunk);
NSL_API nsl_Str nsl_str_chop_by_predicate(nsl_Str *s, bool (*predicate)(char));
NSL_API nsl_Str nsl_str_chop_right_by_delim(nsl_Str *s, char delim);
NSL_API nsl_Str nsl_str_chop_right_by_predicate(nsl_Str *s, bool (*predicate)(char));
NSL_API nsl_Str nsl_str_take(nsl_Str *s, usize count);
NSL_API bool nsl_str_try_take(nsl_Str *s, usize count, nsl_Str *chunk);
NSL_API nsl_Str nsl_str_take_right(nsl_Str *s, usize count);
NSL_API bool nsl_str_try_take_right(nsl_Str *s, usize count, nsl_Str *chunk);

NSL_API nsl_Str nsl_str_substring(nsl_Str s, usize start, usize end);

NSL_API u64 nsl_str_u64(nsl_Str s);
NSL_API u64 nsl_str_chop_u64(nsl_Str *s);

NSL_API i64 nsl_str_i64(nsl_Str s);
NSL_API i64 nsl_str_chop_i64(nsl_Str *s);

NSL_API f64 nsl_str_f64(nsl_Str s);
NSL_API f64 nsl_str_chop_f64(nsl_Str *s);

// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NSL_API usize nsl_str_find(nsl_Str haystack, nsl_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NSL_API usize nsl_str_find_by_predicate(nsl_Str haystack, bool (*predicate)(char));
// Returns 'STR_NOT_FOUND' if 'needle' was not found.
NSL_API usize nsl_str_find_last(nsl_Str haystack, nsl_Str needle);
// Returns 'STR_NOT_FOUND' if 'predicate' was not found.
NSL_API usize nsl_str_find_last_by_predicate(nsl_Str haystack, bool (*predicate)(char));

NSL_API usize nsl_str_count(nsl_Str haystack, nsl_Str needle);
// Returns '\0' if the index is out of bounds.
NSL_API char nsl_str_getc(nsl_Str s, usize idx);

// Basic FNV hash.
NSL_API u64 nsl_str_hash(nsl_Str s);

#endif // _NSL_STR_H_
