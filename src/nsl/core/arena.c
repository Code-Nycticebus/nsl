#include "nsl/core/arena.h"

#include <stdlib.h>
#include <string.h>

// 4 kb
#define CHUNK_DEFAULT_SIZE 4000

struct nsl_Chunk {
    nsl_Chunk *next, *prev;
    usize cap;
    usize allocated;
    u8 data[];
};

static nsl_Chunk *chunk_allocate(usize size) {
    nsl_Chunk *chunk = malloc(sizeof(nsl_Chunk) + size);
    NSL_ASSERT(chunk != NULL && "Memory allocation failed");
    chunk->cap = size;
    chunk->allocated = 0;
    chunk->next = chunk->prev = 0;
    return chunk;
}

static void chunk_free(nsl_Chunk *chunk) { 
    free(chunk); 
}

NSL_CONST_FN static usize align(usize size) {
    const usize mask = sizeof(void *) - 1;
    return (size + mask) & ~mask;
}

NSL_API void nsl_arena_free(nsl_Arena *arena) {
    nsl_Chunk *next = arena->begin;
    while (next != NULL) {
        nsl_Chunk *temp = next;
        next = next->next;
        chunk_free(temp);
    }
    arena->begin = NULL;
}

NSL_API void nsl_arena_reset(nsl_Arena *arena) {
    for (nsl_Chunk *next = arena->begin; next != NULL; next = next->next) {
        if (next->cap != 0) {
            next->allocated = 0;
        }
    }
}

NSL_API usize nsl_arena_size(nsl_Arena *arena) {
    usize size = 0;
    for (nsl_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->allocated;
    }
    return size;
}

NSL_API usize nsl_arena_real_size(nsl_Arena *arena) {
    usize size = 0;
    for (nsl_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->cap ? chunk->cap : chunk->allocated;
    }
    return size;
}

NSL_API void *nsl_arena_alloc(nsl_Arena *arena, usize size) {
    size = align(size);
    nsl_Chunk *chunk = arena->begin;
    for (; chunk != NULL; chunk = chunk->next) {
        NSL_ASSERT(size <= SIZE_MAX - chunk->allocated && "integer overflow");
        if (chunk->allocated + size < chunk->cap) {
            break;
        }
    }
    if (chunk == NULL) {
        const usize chunk_size =
            size >= CHUNK_DEFAULT_SIZE ? size : CHUNK_DEFAULT_SIZE;
        chunk = chunk_allocate(chunk_size);
        chunk->next = arena->begin;
        if (arena->begin) {
            arena->begin->prev = chunk;
        }
        arena->begin = chunk;
    }
    void *ptr = &chunk->data[chunk->allocated];
    chunk->allocated += size;
    return ptr;
}

NSL_API void *nsl_arena_calloc(nsl_Arena *arena, usize size) {
    void *ptr = nsl_arena_alloc(arena, size);
    memset(ptr, 0, size);
    return ptr;
}

NSL_API void *nsl_arena_alloc_chunk(nsl_Arena *arena, usize size) {
    nsl_Chunk *chunk = chunk_allocate(size);
    if (arena == NULL) return chunk->data;
    chunk->cap = 0;
    chunk->allocated = size;
    chunk->next = arena->begin;
    if (arena->begin) {
        arena->begin->prev = chunk;
    }
    arena->begin = chunk;
    return chunk->data;
}

NSL_API void *nsl_arena_calloc_chunk(nsl_Arena *arena, usize size) {
    void *data = nsl_arena_alloc_chunk(arena, size);
    memset(data, 0, size);
    return data;
}

NSL_API void *nsl_arena_realloc_chunk(nsl_Arena *arena, void *ptr, usize size) {
    if (ptr == NULL) return nsl_arena_alloc_chunk(arena, size);

    nsl_Chunk *chunk = (nsl_Chunk *)((usize)ptr - sizeof(nsl_Chunk));

    if (size < chunk->allocated) return chunk->data;

    nsl_Chunk *new_chunk = realloc(chunk, sizeof(nsl_Chunk) + size);

    if (arena == NULL) return new_chunk->data;

    if (new_chunk->prev)       new_chunk->prev->next = new_chunk;
    if (new_chunk->next)       new_chunk->next->prev = new_chunk;
    if (arena->begin == chunk) arena->begin = new_chunk;

    return new_chunk->data;
}

NSL_API void nsl_arena_free_chunk(nsl_Arena *arena, void *ptr) {
    if (ptr == NULL) return;

    nsl_Chunk *chunk = (nsl_Chunk *)((usize)ptr - sizeof(nsl_Chunk));
    if (arena) {
        if (chunk == arena->begin) arena->begin = chunk->next;
        if (chunk->prev)           chunk->prev->next = chunk->next;
        if (chunk->next)           chunk->next->prev = chunk->prev;
    }

    free(chunk);
}
