#include "nc/core/arena.h"

typedef struct TestChunk {
    struct TestChunk *next, *prev;
    usize cap;
    usize allocated;
    u8 data[];
} TestChunk;

NC_CONST_FN static usize test_align(usize size) {
    const usize mask = sizeof(void *) - 1;
    return (size + mask) & ~mask;
}

static void test_arena(void) {
    nc_Arena arena = {0};

    const usize n_bytes = 10;
    char *buffer = nc_arena_alloc(&arena, n_bytes);
    NC_ASSERT(buffer && "Buffer was not allocated");
    NC_ASSERT(arena.begin && "Begin was not set");

    TestChunk *tc = (TestChunk *)arena.begin;
    NC_ASSERT(tc->allocated == test_align(n_bytes) &&
           "Not enough bytes are allocated");

    nc_arena_free(&arena);
}

static void test_chunks(void) {
    nc_Arena arena = {0};
    const usize n_bytes = 10;
    char *buffer = nc_arena_alloc(&arena, n_bytes);
    NC_ASSERT(buffer && "Buffer was not allocated");

    char *buffer2 = nc_arena_alloc(&arena, n_bytes);
    NC_ASSERT(buffer + test_align(n_bytes) == buffer2 &&
           "'buffer2' was not next to 'buffer'");

    TestChunk *tc = (TestChunk *)arena.begin;
    const usize more_bytes = tc->cap;
    char *big_buffer = nc_arena_alloc(&arena, more_bytes);
    NC_ASSERT(big_buffer && "Buffer was not allocated");

    TestChunk *tc2 = (TestChunk *)arena.begin;
    NC_ASSERT(tc2 != tc && "No new chunk was allocated");
    NC_ASSERT(tc2->allocated == more_bytes && "Not enough bytes are allocated");

    nc_arena_free(&arena);
}

static void test_calloc(void) {
    nc_Arena arena = {0};

    const usize n_bytes = 20;
    char *buffer = nc_arena_calloc(&arena, n_bytes);
    for (usize i = 0; i < n_bytes; i++) {
        NC_ASSERT(buffer[i] == '\0' && "Buffer was not zero initialized");
    }

    nc_arena_free(&arena);
}

static void test_reset(void) { // NOLINT
    nc_Arena arena = {0};

    int *chunk = nc_arena_alloc_chunk(&arena, sizeof(int));

    const usize n_bytes = 10;
    char *buffer = nc_arena_alloc(&arena, n_bytes);
    NC_ASSERT(buffer && "Buffer was not allocated");

    const usize more_bytes = ((TestChunk *)arena.begin)->cap;
    char *big_buffer = nc_arena_alloc(&arena, more_bytes);
    NC_ASSERT(big_buffer && "Buffer was not allocated");

    nc_arena_reset(&arena);

    TestChunk *tc = (TestChunk *)arena.begin;
    NC_ASSERT(tc->allocated == 0 && "First Chunk was not reset");
    NC_ASSERT(tc->next->allocated == 0 && "Second Chunk was not reset");

    char *buffer_after_reset = nc_arena_alloc(&arena, n_bytes);
    NC_ASSERT(buffer_after_reset && "Buffer was not allocated");
    NC_ASSERT(tc->allocated == test_align(n_bytes) &&
           "Not enough bytes are allocated");

    char *big_buffer_after_reset = nc_arena_alloc(&arena, more_bytes);
    NC_ASSERT(big_buffer_after_reset && "Buffer was not allocated");

    TestChunk *chunk_after_reset =
        (TestChunk *)((u8 *)chunk - sizeof(TestChunk));
    NC_ASSERT(chunk_after_reset->allocated == sizeof(int) && "");

    nc_arena_free(&arena);
}

static void test_size(void) {
    nc_Arena arena = {0};

    int *i = nc_arena_alloc(&arena, sizeof(*i));
    NC_ASSERT(nc_arena_size(&arena) == test_align(sizeof(*i)) &&
           "size not matching");
    NC_ASSERT(nc_arena_real_size(&arena) == 4000 && "chunk size does not match");

    nc_arena_free(&arena);
}

void run_test_arena() {
    test_arena();
    test_chunks();
    test_calloc();
    test_reset();
    test_size();
}
