#include "nsl/os/file.h"

#include "nsl/core/error.h"
#include "nsl/structs/list.h"
#include "nsl/types/str.h"
#include "nsl/types/byte.h"
#include "nsl/types/int.h"

#include <errno.h>
#include <string.h>
#include <stdarg.h>


NSL_API FILE *nsl_file_open(nsl_Path path, const char *mode, nsl_Error *error) {
    errno = 0;
    char filepath[FILENAME_MAX] = {0};
    memcpy(filepath, path.data, nsl_usize_min(path.len, FILENAME_MAX - 1));

    FILE *file = fopen(filepath, mode);
    if (file == NULL) NSL_ERROR_EMIT(error, errno, strerror(errno));
    return file;
}

NSL_API void nsl_file_close(FILE *file) {
    fclose(file);
}

NSL_API void nsl_file_check_error(FILE* file, nsl_Error* error) {
    if (ferror(file) != 0) NSL_ERROR_EMIT(error, errno, strerror(errno));
}

NSL_API usize nsl_file_size(FILE* file) {
    long current = ftell(file);
    fseek(file, 0, SEEK_END);
    usize size = (usize)ftell(file);
    fseek(file, current, SEEK_SET);
    return size;
}

NSL_API nsl_Str nsl_file_read_str(FILE* file, nsl_Arena* arena) {
    usize size = nsl_file_size(file);
    char* data = nsl_arena_calloc(arena, size+1);
    size = fread(data, 1, size, file);
    return nsl_str_from_parts(size, data);
}

NSL_API nsl_Str nsl_file_read_sb(FILE* file, nsl_StrBuilder* sb) {
    usize size = nsl_file_size(file);
    nsl_list_reserve(sb, size);
    char* start = &nsl_list_last(sb);
    size = fread(start, 1, size, file);
    sb->len += size;
    return nsl_str_from_parts(size, start);
}

NSL_API nsl_Str nsl_file_read_line(FILE* file, nsl_StrBuilder* sb) {
    usize off = sb->len;
    i32 c = 0;
    while (!feof(file) && c != '\n') {
        c = fgetc(file);
        nsl_list_push(sb, (char)c);
    }
    return nsl_str_from_parts(sb->len - off, &sb->items[off]);
}

NSL_API nsl_Bytes nsl_file_read_bytes(FILE* file, usize size, u8* buffer) {
    size = fread(buffer, 1, size, file);
    return nsl_bytes_from_parts(size, buffer);
}

NSL_API NSL_FMT(2) void nsl_file_write_fmt(FILE* file, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vfprintf(file, fmt, va);
    va_end(va);
}

NSL_API void nsl_file_write_str(FILE* file, nsl_Str content) {
    fwrite(content.data, 1, content.len, file);
}

NSL_API void nsl_file_write_bytes(FILE* file, nsl_Bytes content) {
    fwrite(content.data, 1, content.size, file);
}
