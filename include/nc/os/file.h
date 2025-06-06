#ifndef _NC_FILE_H_
#define _NC_FILE_H_

#include "nc/defines.h"

FILE *nc_file_open(nc_Path path, const char *mode, nc_Error *error);
void nc_file_close(FILE *file);

void nc_file_check_error(FILE* file, nc_Error* error);

usize nc_file_size(FILE *file);

nc_Str nc_file_read_str(FILE *file, nc_Arena *arena);
nc_Str nc_file_read_sb(FILE *file, nc_StrBuilder *sb);
nc_Str nc_file_read_line(FILE *file, nc_StrBuilder *sb);

nc_Bytes nc_file_read_bytes(FILE *file, usize size, u8 *buffer);

NC_FMT(2) void nc_file_write_fmt(FILE* file, const char* fmt, ...);
void nc_file_write_str(FILE *file, nc_Str content);
void nc_file_write_bytes(FILE *file, nc_Bytes content);

#endif
