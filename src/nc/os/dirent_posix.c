#include "nc/os/dirent.h"

#include "nc/structs/list.h"
#include "nc/types/str.h"
#include "nc/types/path.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

typedef struct nc_DirNode {
    struct nc_DirNode *next;
    DIR *handle;
    char name[];
} nc_DirNode;

nc_DirIter nc_dir_begin(nc_Path directory, bool recursive, nc_Error* error) {
    nc_DirIter it = {.recursive=recursive, .error=error};

    const usize size = sizeof(nc_DirNode) + directory.len + 1;
    nc_DirNode* node = nc_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    node->handle = opendir(node->name);
    if (node->handle == NULL) {
        NC_ERROR_EMIT(it.error, errno, strerror(errno));
    }

    return it;
}

void nc_dir_end(nc_DirIter *it) {
    while (it->_handle != NULL) {
        nc_DirNode* node = it->_handle;
        if (node->handle) closedir(node->handle);
        it->_handle = node->next;
    }
    nc_arena_free(&it->scratch);
}

nc_DirEntry *nc_dir_next(nc_DirIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nc_arena_reset(&it->scratch);
        nc_DirNode *current = it->_handle;

        struct dirent *entry = readdir(current->handle);
        if (entry == NULL) {
            closedir(current->handle);
            it->_handle = current->next;
            nc_arena_free_chunk(&it->scratch, current);
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        nc_DirEntry *e = nc_arena_alloc(&it->scratch, sizeof(nc_DirEntry));
        nc_Path parts[] = {
            nc_str_from_cstr(current->name), nc_str_from_cstr(entry->d_name),
        };
        e->path = nc_path_join(NC_ARRAY_LEN(parts), parts, &it->scratch);

        struct stat entry_info;
        if (stat(e->path.data, &entry_info) == -1) continue;

        e->is_dir = S_ISDIR(entry_info.st_mode);
        e->size = (usize)entry_info.st_size;
        e->mtime = (u64)entry_info.st_mtimensec;

        if (it->recursive && e->is_dir) {
            DIR *handle = opendir(e->path.data);
            if (handle == NULL) continue;
            const usize size = sizeof(nc_DirNode) + e->path.len + 1;
            nc_DirNode *node = nc_arena_calloc_chunk(&it->scratch, size);
            node->handle = handle;
            memcpy(node->name, e->path.data, e->path.len);
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }
    return NULL;
}
