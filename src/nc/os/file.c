#include "nc/os/file.h"

#include "nc/core/error.h"
#include "nc/structs/list.h"
#include "nc/types/str.h"
#include "nc/types/byte.h"
#include "nc/types/int.h"

#include <errno.h>
#include <string.h>


FILE *nc_file_open(nc_Path path, const char *mode, nc_Error *error) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nc_usize_min(path.len, FILENAME_MAX - 1));

    FILE *file = fopen(filepath, mode);
    if (file == NULL) NC_ERROR_EMIT(error, errno, strerror(errno));
    return file;
}

void nc_file_close(FILE *file) {
    fclose(file);
}

void nc_file_check_error(FILE* file, nc_Error* error) {
    if (ferror(file) != 0) NC_ERROR_EMIT(error, errno, strerror(errno));
}

usize nc_file_size(FILE* file) {
    long current = ftell(file);
    fseek(file, 0, SEEK_END);
    usize size = (usize)ftell(file);
    fseek(file, current, SEEK_SET);
    return size;
}

nc_Str nc_file_read_sb(FILE* file, nc_StrBuilder* sb) {
    usize size = nc_file_size(file);
    nc_list_reserve(sb, size);
    char* start = &nc_list_last(sb);
    size = fread(start, 1, size, file);
    sb->len += size;
    return nc_str_from_parts(size, start);
}

nc_Str nc_file_read_line(FILE* file, nc_StrBuilder* sb) {
    nc_list_reserve(sb, 128);
    usize off = sb->len;
    i32 c = 0;
    while (!feof(file) && c != '\n') {
        c = fgetc(file);
        nc_list_push(sb, (char)c);
    }
    return nc_str_from_parts(sb->len - off, &sb->items[off]);
}

nc_Bytes nc_file_read_bytes(FILE* file, usize size, u8* buffer) {
    size = fread(buffer, 1, size, file);
    return nc_bytes_from_parts(size, buffer);
}

NC_FMT(2) void nc_file_write_fmt(FILE* file, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(file, fmt, va);
    va_end(va);
}

void nc_file_write_str(FILE* file, nc_Str content) {
    fwrite(content.data, 1, content.len, file);
}

void nc_file_write_bytes(FILE* file, nc_Bytes content) {
    fwrite(content.data, 1, content.size, file);
}
