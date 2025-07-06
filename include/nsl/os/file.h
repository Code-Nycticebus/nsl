#ifndef _NSL_FILE_H_
#define _NSL_FILE_H_

#include "nsl/defines.h"

NSL_API FILE *nsl_file_open(nsl_Path path, const char *mode, nsl_Error *error);
NSL_API void nsl_file_close(FILE *file);

NSL_API void nsl_file_check_error(FILE* file, nsl_Error* error);

NSL_API usize nsl_file_size(FILE *file);

NSL_API nsl_Str nsl_file_read_str(FILE *file, nsl_Arena *arena);
NSL_API nsl_Str nsl_file_read_sb(FILE *file, nsl_StrBuilder *sb);
NSL_API nsl_Str nsl_file_read_line(FILE *file, nsl_StrBuilder *sb);

NSL_API nsl_Bytes nsl_file_read_bytes(FILE *file, usize size, u8 *buffer);

NSL_API NSL_FMT(2) void nsl_file_write_fmt(FILE* file, const char* fmt, ...);
NSL_API void nsl_file_write_str(FILE *file, nsl_Str content);
NSL_API void nsl_file_write_bytes(FILE *file, nsl_Bytes content);

#endif // _NSL_FILE_H_
