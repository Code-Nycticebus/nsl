#include "nsl/os/dir.h"

#include "nsl/structs/list.h"
#include "nsl/types/str.h"
#include "nsl/types/path.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

typedef struct nsl_DirNode {
    struct nsl_DirNode *next;
    DIR *handle;
    char name[];
} nsl_DirNode;

NSL_API nsl_DirIter nsl_dir_begin(nsl_Path directory, bool recursive) {
    nsl_DirIter it = {.recursive = recursive};

    const usize size = sizeof(nsl_DirNode) + directory.len + 1;
    nsl_DirNode* node = nsl_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    node->handle = opendir(node->name);
    if (node->handle == NULL) {
        nsl_arena_free(&it.scratch);
        it.done = true;
    }

    return it;
}

NSL_API void nsl_dir_end(nsl_DirIter *it) {
    while (it->_handle != NULL) {
        nsl_DirNode* node = it->_handle;
        if (node->handle) closedir(node->handle);
        it->_handle = node->next;
    }
    nsl_arena_free(&it->scratch);
}

NSL_API nsl_DirEntry *nsl_dir_next(nsl_DirIter *it) {
    it->done = true;
    while (it->_handle != NULL) {
        nsl_arena_reset(&it->scratch);
        nsl_DirNode *current = it->_handle;

        struct dirent *entry = readdir(current->handle);
        if (entry == NULL) {
            closedir(current->handle);
            it->_handle = current->next;
            nsl_arena_free_chunk(&it->scratch, current);
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        nsl_DirEntry *e = nsl_arena_alloc(&it->scratch, sizeof(nsl_DirEntry));
        nsl_Path parts[] = {
            nsl_str_from_cstr(current->name), nsl_str_from_cstr(entry->d_name),
        };
        e->path = nsl_path_join(NSL_ARRAY_LEN(parts), parts, &it->scratch);

        struct stat entry_info;
        if (stat(e->path.data, &entry_info) == -1) continue;

        e->is_dir = S_ISDIR(entry_info.st_mode);
        e->size = (usize)entry_info.st_size;
        e->mtime = (u64)entry_info.st_mtime;

        if (it->recursive && e->is_dir) {
            DIR *handle = opendir(e->path.data);
            if (handle == NULL) continue;
            const usize size = sizeof(nsl_DirNode) + e->path.len + 1;
            nsl_DirNode *node = nsl_arena_calloc_chunk(&it->scratch, size);
            node->handle = handle;
            memcpy(node->name, e->path.data, e->path.len);
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }
    return NULL;
}
