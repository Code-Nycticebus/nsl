#include "nsl/os/fs.h"

#include "nsl/core/error.h"
#include "nsl/types/str.h"
#include "nsl/types/path.h"

#include <windows.h>
#include <io.h>
#include <string.h>

typedef struct nsl_FsNode {
    struct nsl_FsNode *next;
    HANDLE handle;
    char name[];
} nsl_FsNode;

NSL_API nsl_Error nsl_fs_begin(nsl_FsIter* it, nsl_Path directory, bool recursive) {
    *it = (nsl_FsIter){.recursive = recursive};

    const usize len = directory.len + (sizeof("/*") - 1);
    const usize size = sizeof(nsl_FsNode) + len + 1;
    nsl_FsNode *node = nsl_arena_calloc_chunk(&it->scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it->_handle = node;

    nsl_Path path = nsl_path_join(2, (nsl_Path[]){directory, NSL_STR("/*")}, &it->scratch);
    WIN32_FIND_DATA findFileData;
    node->handle = FindFirstFile(path.data, &findFileData);
    if (node->handle == INVALID_HANDLE_VALUE) {
        it->error = NSL_ERROR;
        nsl_arena_free(&it->scratch);
        return it->error;
    }

    return NSL_NO_ERROR;
}

NSL_API void nsl_fs_end(nsl_FsIter *it) {
    while (it->_handle != NULL) {
        nsl_FsNode *current = it->_handle;
        it->_handle = current->next;
        if (current->handle != INVALID_HANDLE_VALUE) FindClose(current->handle);
    }
    nsl_arena_free(&it->scratch);
}

NSL_API nsl_FsEntry* nsl_fs_next(nsl_FsIter *it) {
    if (it->error) return NULL;
    while (it->_handle != NULL) {
        nsl_arena_reset(&it->scratch);
        nsl_FsNode *current = it->_handle;

        WIN32_FIND_DATA findFileData;
        if (!FindNextFile(current->handle, &findFileData)) {
            FindClose(current->handle);
            it->_handle = current->next;
            nsl_arena_free_chunk(&it->scratch, current);
            continue;
        }

        // skip "." and ".." directories
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        nsl_FsEntry *e = nsl_arena_alloc(&it->scratch, sizeof(nsl_FsEntry));
        nsl_Path parts[] = {
            nsl_str_from_cstr(current->name),
            nsl_str_from_cstr(findFileData.cFileName),
        };
        e->path = nsl_path_join(NSL_ARRAY_LEN(parts), parts, &it->scratch);

        e->is_dir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e->size = ((u64)findFileData.nFileSizeHigh << 32) | findFileData.nFileSizeLow;
        e->mtime = ((u64)findFileData.ftLastWriteTime.dwHighDateTime << 32) | findFileData.ftLastWriteTime.dwLowDateTime;

        if (e->is_dir && it->recursive) {
            nsl_Path path = nsl_path_join(2, (nsl_Path[]){e->path, NSL_STR("/*")}, &it->scratch);
            HANDLE handle = FindFirstFile(path.data, &findFileData);
            if (handle == INVALID_HANDLE_VALUE) {
                continue;
            }

            const usize size = sizeof(nsl_FsNode) + e->path.len + 1;
            nsl_FsNode *node = nsl_arena_calloc_chunk(&it->scratch, size);
            memcpy(node->name, e->path.data, e->path.len);

            node->handle = handle;
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }

    return NULL;
}
