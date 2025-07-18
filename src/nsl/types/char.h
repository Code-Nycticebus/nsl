#ifndef _NSL_CHAR_H_
#define _NSL_CHAR_H_

#include "nsl/defines.h"

NSL_API NSL_CONST_FN bool nsl_char_is_alnum(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_alpha(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_lower(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_upper(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_space(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_cntrl(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_print(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_graph(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_blank(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_punct(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_digit(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_xdigit(char c);
NSL_API NSL_CONST_FN bool nsl_char_is_path_delimiter(char c);

NSL_API NSL_CONST_FN char nsl_char_to_lower(char c);
NSL_API NSL_CONST_FN char nsl_char_to_upper(char c);

NSL_API NSL_CONST_FN u8 nsl_char_to_u8(char c);
NSL_API NSL_CONST_FN u8 nsl_char_hex_to_u8(char c);

NSL_API NSL_CONST_FN char nsl_char_from_u8(u8 d);
NSL_API NSL_CONST_FN char nsl_char_hex_from_u8(u8 d);
NSL_API NSL_CONST_FN char nsl_char_HEX_from_u8(u8 d);

#endif // _NSL_CHAR_H_
