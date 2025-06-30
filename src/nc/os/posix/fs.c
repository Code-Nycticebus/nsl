#include "nc/os/fs.h"

#include "nc/core/error.h"
#include "nc/structs/list.h"
#include "nc/types/str.h"
#include "nc/types/path.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

typedef struct nc_FsNode {
    struct nc_FsNode *next;
    DIR *handle;
    char name[];
} nc_FsNode;

nc_FsIter nc_fs_begin(nc_Path directory, bool recursive, nc_Error* error) {
    nc_FsIter it = {.recursive=recursive, .error=error};

    const usize size = sizeof(nc_FsNode) + directory.len + 1;
    nc_FsNode* node = nc_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    node->handle = opendir(node->name);
    if (node->handle == NULL) {
        NC_ERROR_EMIT(it.error, errno, strerror(errno));
    }

    return it;
}

void nc_fs_end(nc_FsIter *it) {
    while (it->_handle != NULL) {
        nc_FsNode* node = it->_handle;
        if (node->handle) closedir(node->handle);
        it->_handle = node->next;
    }
    nc_arena_free(&it->scratch);
}

nc_FsEntry *nc_fs_next(nc_FsIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nc_arena_reset(&it->scratch);
        nc_FsNode *current = it->_handle;

        struct dirent *entry = readdir(current->handle);
        if (entry == NULL) {
            closedir(current->handle);
            it->_handle = current->next;
            nc_arena_free_chunk(&it->scratch, current);
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        nc_FsEntry *e = nc_arena_alloc(&it->scratch, sizeof(nc_FsEntry));
        nc_Path parts[] = {
            nc_str_from_cstr(current->name), nc_str_from_cstr(entry->d_name),
        };
        e->path = nc_path_join(NC_ARRAY_LEN(parts), parts, &it->scratch);

        struct stat entry_info;
        if (stat(e->path.data, &entry_info) == -1) continue;

        e->is_dir = S_ISDIR(entry_info.st_mode);
        e->size = (usize)entry_info.st_size;
        e->mtime = (u64)entry_info.st_mtime;

        if (it->recursive && e->is_dir) {
            DIR *handle = opendir(e->path.data);
            if (handle == NULL) continue;
            const usize size = sizeof(nc_FsNode) + e->path.len + 1;
            nc_FsNode *node = nc_arena_calloc_chunk(&it->scratch, size);
            node->handle = handle;
            memcpy(node->name, e->path.data, e->path.len);
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }
    return NULL;
}
