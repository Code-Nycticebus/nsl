#include "nsl/types/char.h"

#include <ctype.h>

#define DBASE 10
#define XBASE 16

///////////////////////////////////////////////////////////////////////////////

NSL_API bool nsl_char_is_alnum(char c) { return isalnum(c); }
NSL_API bool nsl_char_is_alpha(char c) { return isalpha(c); }
NSL_API bool nsl_char_is_lower(char c) { return islower(c); }
NSL_API bool nsl_char_is_upper(char c) { return isupper(c); }
NSL_API bool nsl_char_is_space(char c) { return isspace(c); }
NSL_API bool nsl_char_is_cntrl(char c) { return iscntrl(c); }
NSL_API bool nsl_char_is_print(char c) { return isprint(c); }
NSL_API bool nsl_char_is_graph(char c) { return isgraph(c); }
NSL_API bool nsl_char_is_blank(char c) { return isblank(c); }
NSL_API bool nsl_char_is_punct(char c) { return ispunct(c); }
NSL_API bool nsl_char_is_digit(char c) { return isdigit(c); }
NSL_API bool nsl_char_is_xdigit(char c) { return isxdigit(c); }
NSL_API NSL_CONST_FN bool nsl_char_is_path_delimiter(char c) { return c == '/' || c == '\\'; }

///////////////////////////////////////////////////////////////////////////////

NSL_API char nsl_char_to_lower(char c) { return (char)tolower(c); }
NSL_API char nsl_char_to_upper(char c) { return (char)toupper(c); }

NSL_API u8 nsl_char_to_u8(char c) {
  NSL_ASSERT(nsl_char_is_digit(c) && "char not convertible");
  return (u8)c - '0';
}

NSL_API u8 nsl_char_hex_to_u8(char c) {
  NSL_ASSERT(nsl_char_is_xdigit(c) && "char not convertible");
  if ('0' <= c && c <= '9') {
    return nsl_char_to_u8(c);
  }
  if ('a' <= c && c <= 'f') {
    return (u8)(DBASE + c - 'a');
  }
  if ('A' <= c && c <= 'F') {
    return (u8)(DBASE + c - 'A');
  }
  return 0;
}

NSL_API char nsl_char_from_u8(u8 d) {
  NSL_ASSERT(d < DBASE && "char not convertible");
  return '0' + (i8)d;
}

NSL_API char nsl_char_hex_from_u8(u8 d) {
  NSL_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nsl_char_from_u8(d);
  }
  if (d < XBASE) {
    return (char)('a' + (d - DBASE));
  }
  return 0;
}

NSL_API char nsl_char_HEX_from_u8(u8 d) {
  NSL_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nsl_char_from_u8(d);
  }
  if (d < XBASE) {
    return (char)('A' + (d - DBASE));
  }
  return 0;
}
