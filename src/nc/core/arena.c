#include "nc/core/arena.h"

#include <stdlib.h>
#include <string.h>

// 4 kb
#define CHUNK_DEFAULT_SIZE 4000

struct nc_Chunk {
    nc_Chunk *next, *prev;
    usize cap;
    usize allocated;
    u8 data[];
};

static nc_Chunk *chunk_allocate(usize size) {
    nc_Chunk *chunk = malloc(sizeof(nc_Chunk) + size);
    NC_ASSERT(chunk != NULL && "Memory allocation failed");
    chunk->cap = size;
    chunk->allocated = 0;
    chunk->next = chunk->prev = 0;
    return chunk;
}

static void chunk_free(nc_Chunk *chunk) { 
    free(chunk); 
}

NC_CONST_FN static usize align(usize size) {
    const usize mask = sizeof(void *) - 1;
    return (size + mask) & ~mask;
}

void nc_arena_free(nc_Arena *arena) {
    nc_Chunk *next = arena->begin;
    while (next != NULL) {
        nc_Chunk *temp = next;
        next = next->next;
        chunk_free(temp);
    }
    arena->begin = NULL;
}

void nc_arena_reset(nc_Arena *arena) {
    for (nc_Chunk *next = arena->begin; next != NULL; next = next->next) {
        if (next->cap != 0) {
            next->allocated = 0;
        }
    }
}

usize nc_arena_size(nc_Arena *arena) {
    usize size = 0;
    for (nc_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->allocated;
    }
    return size;
}

usize nc_arena_real_size(nc_Arena *arena) {
    usize size = 0;
    for (nc_Chunk *chunk = arena->begin; chunk != NULL; chunk = chunk->next) {
        size += chunk->cap ? chunk->cap : chunk->allocated;
    }
    return size;
}

void *nc_arena_alloc(nc_Arena *arena, usize size) {
    size = align(size);
    nc_Chunk *chunk = arena->begin;
    for (; chunk != NULL; chunk = chunk->next) {
        NC_ASSERT(size <= SIZE_MAX - chunk->allocated && "integer overflow");
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

void *nc_arena_calloc(nc_Arena *arena, usize size) {
    void *ptr = nc_arena_alloc(arena, size);
    memset(ptr, 0, size);
    return ptr;
}

void *nc_arena_alloc_chunk(nc_Arena *arena, usize size) {
    nc_Chunk *chunk = chunk_allocate(size);
    chunk->cap = 0;
    chunk->allocated = size;
    chunk->next = arena->begin;
    if (arena->begin) {
        arena->begin->prev = chunk;
    }
    arena->begin = chunk;
    return chunk->data;
}

void *nc_arena_calloc_chunk(nc_Arena *arena, usize size) {
    void *data = nc_arena_alloc_chunk(arena, size);
    memset(data, 0, size);
    return data;
}

void *nc_arena_realloc_chunk(nc_Arena *arena, void *ptr, usize size) {
    if (ptr == NULL) {
        return nc_arena_alloc_chunk(arena, size);
    }
    nc_Chunk *chunk = (nc_Chunk *)((u8 *)ptr - sizeof(nc_Chunk));
    if (size < chunk->allocated) {
        return chunk->data;
    }
    nc_Chunk *new_chunk = realloc(chunk, sizeof(nc_Chunk) + size);
    if (new_chunk->prev) {
        new_chunk->prev->next = new_chunk;
    }
    if (new_chunk->next) {
        new_chunk->next->prev = new_chunk;
    }
    if (arena->begin == chunk) {
        arena->begin = new_chunk;
    }
    return new_chunk->data;
}

void nc_arena_free_chunk(nc_Arena *arena, void *ptr) {
    if (ptr == NULL) {
        return;
    }
    nc_Chunk *chunk = (nc_Chunk *)((u8 *)ptr - sizeof(nc_Chunk));
    if (chunk == arena->begin) {
        arena->begin = chunk->next;
    }
    if (chunk->prev) {
        chunk->prev->next = chunk->next;
    }
    if (chunk->next) {
        chunk->next->prev = chunk->prev;
    }
    free(chunk);
}
