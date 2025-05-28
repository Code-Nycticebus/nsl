#ifndef _NC_CHAR_H_
#define _NC_CHAR_H_

#include "nc/defines.h"

NC_API NC_CONST_FN bool nc_char_is_alnum(char c);
NC_API NC_CONST_FN bool nc_char_is_alpha(char c);
NC_API NC_CONST_FN bool nc_char_is_lower(char c);
NC_API NC_CONST_FN bool nc_char_is_upper(char c);
NC_API NC_CONST_FN bool nc_char_is_space(char c);
NC_API NC_CONST_FN bool nc_char_is_cntrl(char c);
NC_API NC_CONST_FN bool nc_char_is_print(char c);
NC_API NC_CONST_FN bool nc_char_is_graph(char c);
NC_API NC_CONST_FN bool nc_char_is_blank(char c);
NC_API NC_CONST_FN bool nc_char_is_punct(char c);
NC_API NC_CONST_FN bool nc_char_is_digit(char c);
NC_API NC_CONST_FN bool nc_char_is_xdigit(char c);

NC_API NC_CONST_FN char nc_char_to_lower(char c);
NC_API NC_CONST_FN char nc_char_to_upper(char c);

NC_API NC_CONST_FN u8 nc_char_to_u8(char c);
NC_API NC_CONST_FN u8 nc_char_hex_to_u8(char c);

NC_API NC_CONST_FN char nc_char_from_u8(u8 d);
NC_API NC_CONST_FN char nc_char_hex_from_u8(u8 d);
NC_API NC_CONST_FN char nc_char_HEX_from_u8(u8 d);

#endif // _NC_CHAR_H_
