#include "nc/types/char.h"

#include <ctype.h>

#define DBASE 10
#define XBASE 16

///////////////////////////////////////////////////////////////////////////////

NC_API bool nc_char_is_alnum(char c) { return isalnum(c); }
NC_API bool nc_char_is_alpha(char c) { return isalpha(c); }
NC_API bool nc_char_is_lower(char c) { return islower(c); }
NC_API bool nc_char_is_upper(char c) { return isupper(c); }
NC_API bool nc_char_is_space(char c) { return isspace(c); }
NC_API bool nc_char_is_cntrl(char c) { return iscntrl(c); }
NC_API bool nc_char_is_print(char c) { return isprint(c); }
NC_API bool nc_char_is_graph(char c) { return isgraph(c); }
NC_API bool nc_char_is_blank(char c) { return isblank(c); }
NC_API bool nc_char_is_punct(char c) { return ispunct(c); }
NC_API bool nc_char_is_digit(char c) { return isdigit(c); }
NC_API bool nc_char_is_xdigit(char c) { return isxdigit(c); }

///////////////////////////////////////////////////////////////////////////////

NC_API char nc_char_to_lower(char c) { return (char)tolower(c); }
NC_API char nc_char_to_upper(char c) { return (char)toupper(c); }

NC_API u8 nc_char_to_u8(char c) {
  NC_ASSERT(nc_char_is_digit(c) && "char not convertible");
  return (u8)c - '0';
}

NC_API u8 nc_char_hex_to_u8(char c) {
  NC_ASSERT(nc_char_is_xdigit(c) && "char not convertible");
  if ('0' <= c && c <= '9') {
    return nc_char_to_u8(c);
  }
  if ('a' <= c && c <= 'f') {
    return DBASE + (u8)c - 'a';
  }
  if ('A' <= c && c <= 'F') {
    return DBASE + (u8)c - 'A';
  }
  return 0;
}

NC_API char nc_char_from_u8(u8 d) {
  NC_ASSERT(d < DBASE && "char not convertible");
  return '0' + (i8)d;
}

NC_API char nc_char_hex_from_u8(u8 d) {
  NC_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nc_char_from_u8(d);
  }
  if (d < XBASE) {
    return 'a' + ((i8)d - DBASE);
  }
  return 0;
}

NC_API char nc_char_HEX_from_u8(u8 d) {
  NC_ASSERT(d < XBASE && "char not convertible");
  if (d < DBASE) {
    return nc_char_from_u8(d);
  }
  if (d < XBASE) {
    return 'A' + ((i8)d - DBASE);
  }
  return 0;
}
