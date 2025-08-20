#include "nsl/os/dir.h"

#include "nsl/types/str.h"
#include "nsl/types/path.h"

#include <windows.h>
#include <io.h>
#include <string.h>

typedef struct nsl_DirNode {
    struct nsl_DirNode *next;
    HANDLE handle;
    char name[];
} nsl_DirNode;

NSL_API nsl_DirIter nsl_dir_begin(nsl_Path directory, bool recursive) {
    nsl_DirIter it = {.recursive = recursive};

    const usize len = directory.len + (sizeof("/*") - 1);
    const usize size = sizeof(nsl_DirNode) + len + 1;
    nsl_DirNode *node = nsl_arena_calloc_chunk(&it.scratch, size);
    memcpy(node->name, directory.data, directory.len);
    it._handle = node;

    nsl_Path path = nsl_path_join(2, (nsl_Path[]){directory, NSL_STR("/*")}, &it.scratch);
    WIN32_FIND_DATA findFileData;
    node->handle = FindFirstFile(path.data, &findFileData);
    if (node->handle == INVALID_HANDLE_VALUE) {
        nsl_arena_free(&it.scratch);
        it.done = true;
    }

    return it;
}

NSL_API void nsl_dir_end(nsl_DirIter *it) {
    while (it->_handle != NULL) {
        nsl_DirNode *current = it->_handle;
        it->_handle = current->next;
        if (current->handle != INVALID_HANDLE_VALUE) FindClose(current->handle);
    }
    nsl_arena_free(&it->scratch);
}

NSL_API nsl_DirEntry* nsl_dir_next(nsl_DirIter *it) {
    it->done = true;
    while (it->_handle != NULL) {
        nsl_arena_reset(&it->scratch);
        nsl_DirNode *current = it->_handle;

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

        nsl_DirEntry *e = nsl_arena_alloc(&it->scratch, sizeof(nsl_DirEntry));
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

            const usize size = sizeof(nsl_DirNode) + e->path.len + 1;
            nsl_DirNode *node = nsl_arena_calloc_chunk(&it->scratch, size);
            memcpy(node->name, e->path.data, e->path.len);

            node->handle = handle;
            node->next = it->_handle;
            it->_handle = node;
        }

        return e;
    }

    return NULL;
}
