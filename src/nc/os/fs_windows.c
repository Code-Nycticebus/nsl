#include "nc/os/fs.h"

#include "nc/types/str.h"

#include <io.h>
#include <string.h>

typedef struct nc_FsNode {
    struct nc_FsNode *next;
    HANDLE handle;
    char name[];
} nc_FsNode;

nc_FsIter nc_fs_begin(nc_Path directory, bool recursive, nc_Error *error) {
    nc_FsIter it = {.recursive = recursive, .error = error};

    const usize len = directory.len + (sizeof("/*") - 1);
    const usize size = sizeof(nc_FsNode) + len + 1;
    nc_FsNode *node = nc_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    nc_Path path = nc_path_join(2, (nc_Path[]){directory, NC_STR("/*")}, &it.scratch);
    WIN32_FIND_DATA findFileData;
    node->handle = FindFirstFile(path.data, &findFileData);
    if (node->handle == INVALID_HANDLE_VALUE) {
        NC_ERROR_EMIT(it.error, GetLastError(), "FindFirstFile failed\n");
    }

    return it;
}

void nc_fs_end(nc_FsIter *it) {
    while (it->_handle != NULL) {
        nc_FsNode *current = it->_handle;
        it->_handle = current->next;
        if (current->handle != INVALID_HANDLE_VALUE) FindClose(current->handle);
    }
    nc_arena_free(&it->scratch);
}

nc_FsEntry* nc_fs_next(nc_FsIter *it) {
    if (it->error && it->error->code) return NULL;
    while (it->_handle != NULL) {
        nc_arena_reset(&it->scratch);
        nc_FsNode *current = it->_handle;

        WIN32_FIND_DATA findFileData;
        if (!FindNextFile(current->handle, &findFileData)) {
            FindClose(current->handle);
            it->_handle = current->next;
            nc_arena_free_chunk(&it->scratch, current);
            continue;
        }

        // skip "." and ".." directories
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        nc_FsEntry *e = nc_arena_alloc(&it->scratch, sizeof(nc_FsEntry));
        nc_Path parts[] = {
            nc_str_from_cstr(current->name),
            nc_str_from_cstr(findFileData.cFileName),
        };
        e->path = nc_path_join(NC_ARRAY_LEN(parts), parts, &it->scratch);

        e->is_dir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e->size = ((uint64_t)findFileData.nFileSizeHigh << 32) | findFileData.nFileSizeLow;
        e->mtime = findFileData.ftLastWriteTime;

        if (e->is_dir && it->recursive) {
            nc_Path path = nc_path_join(2, (nc_Path[]){e->path, NC_STR("/*")}, &it->scratch);
            HANDLE handle = FindFirstFile(path.data, &findFileData);
            if (handle == INVALID_HANDLE_VALUE) {
                continue;
            }

            const usize size = sizeof(nc_FsNode) + e->path.len + 1;
            nc_FsNode *node = nc_arena_calloc_chunk(&it->scratch, size);
            memcpy(node->name, e->path.data, e->path.len);

            node->handle = handle;
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }

    return NULL;
}
