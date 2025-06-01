#ifndef _NC_DIR_
#define _NC_DIR_

#include "nc/defines.h"

typedef struct {
    nc_Path path;
    bool is_dir;
    usize size;
    u64 mtime;
} nc_FsEntry;

typedef struct {
    nc_Arena scratch; // per file scratch buffer
    bool recursive;   // recursive
    nc_Error *error;  // Error
    void *_handle;    // platform specific handle
} nc_FsIter;

nc_FsIter nc_fs_begin(nc_Path directory, bool recursive, nc_Error* error);
void nc_fs_end(nc_FsIter *it);

nc_FsEntry *nc_fs_next(nc_FsIter *it);

typedef struct {
    u32 mode;       // set the directory mode (default = 0755)
    bool exists_ok; // error when the directory exists
    bool parents;   // create parent paths
} nc_FsDirConfig;
void nc_fs_mkdir(nc_Path path, nc_Error *error, nc_FsDirConfig config);

bool nc_fs_exists(nc_Path path);
bool nc_fs_is_dir(nc_Path path);

FILE *nc_fs_open(nc_Path path, const char *mode, nc_Error *error);
void nc_fs_close(FILE *file);

void nc_fs_check_error(FILE* file, nc_Error* error);

usize nc_fs_size(FILE* file);

nc_Str nc_fs_read_str(FILE* file, nc_StrBuilder* sb);
nc_Str nc_fs_read_line(FILE* file, nc_StrBuilder* sb);

nc_Bytes nc_fs_read_bytes(FILE* file, usize size, u8* buffer);

void nc_fs_write_str(FILE* file, nc_Str content);
void nc_fs_write_bytes(FILE* file, nc_Str content);

#endif // _NC_DIR_
