#include "nsl.h"

typedef struct TestChunk {
    struct TestChunk *next, *prev;
    usize cap;
    usize allocated;
    u8 data[];
} TestChunk;

NSL_CONST_FN static usize test_align(usize size) {
    const usize mask = sizeof(void *) - 1;
    return (size + mask) & ~mask;
}

static void test_arena(void) {
    nsl_Arena arena = {0};

    const usize n_bytes = 10;
    char *buffer = nsl_arena_alloc(&arena, n_bytes);
    NSL_ASSERT(buffer && "Buffer was not allocated");
    NSL_ASSERT(arena.begin && "Begin was not set");

    TestChunk *tc = (TestChunk *)arena.begin;
    NSL_ASSERT(tc->allocated == test_align(n_bytes) &&
           "Not enough bytes are allocated");

    nsl_arena_free(&arena);
}

static void test_chunks(void) {
    nsl_Arena arena = {0};
    const usize n_bytes = 10;
    char *buffer = nsl_arena_alloc(&arena, n_bytes);
    NSL_ASSERT(buffer && "Buffer was not allocated");

    char *buffer2 = nsl_arena_alloc(&arena, n_bytes);
    NSL_ASSERT(buffer + test_align(n_bytes) == buffer2 &&
           "'buffer2' was not next to 'buffer'");

    TestChunk *tc = (TestChunk *)arena.begin;
    const usize more_bytes = tc->cap;
    char *big_buffer = nsl_arena_alloc(&arena, more_bytes);
    NSL_ASSERT(big_buffer && "Buffer was not allocated");

    TestChunk *tc2 = (TestChunk *)arena.begin;
    NSL_ASSERT(tc2 != tc && "No new chunk was allocated");
    NSL_ASSERT(tc2->allocated == more_bytes && "Not enough bytes are allocated");

    nsl_arena_free(&arena);
}

static void test_calloc(void) {
    nsl_Arena arena = {0};

    const usize n_bytes = 20;
    char *buffer = nsl_arena_calloc(&arena, n_bytes);
    for (usize i = 0; i < n_bytes; i++) {
        NSL_ASSERT(buffer[i] == '\0' && "Buffer was not zero initialized");
    }

    nsl_arena_free(&arena);
}

static void test_reset(void) { // NOLINT
    nsl_Arena arena = {0};

    int *chunk = nsl_arena_alloc_chunk(&arena, sizeof(int));

    const usize n_bytes = 10;
    char *buffer = nsl_arena_alloc(&arena, n_bytes);
    NSL_ASSERT(buffer && "Buffer was not allocated");

    const usize more_bytes = ((TestChunk *)arena.begin)->cap;
    char *big_buffer = nsl_arena_alloc(&arena, more_bytes);
    NSL_ASSERT(big_buffer && "Buffer was not allocated");

    nsl_arena_reset(&arena);

    TestChunk *tc = (TestChunk *)arena.begin;
    NSL_ASSERT(tc->allocated == 0 && "First Chunk was not reset");
    NSL_ASSERT(tc->next->allocated == 0 && "Second Chunk was not reset");

    char *buffer_after_reset = nsl_arena_alloc(&arena, n_bytes);
    NSL_ASSERT(buffer_after_reset && "Buffer was not allocated");
    NSL_ASSERT(tc->allocated == test_align(n_bytes) &&
           "Not enough bytes are allocated");

    char *big_buffer_after_reset = nsl_arena_alloc(&arena, more_bytes);
    NSL_ASSERT(big_buffer_after_reset && "Buffer was not allocated");

    TestChunk *chunk_after_reset =
        (TestChunk *)((usize)chunk - sizeof(TestChunk));
    NSL_ASSERT(chunk_after_reset->allocated == sizeof(int) && "");

    nsl_arena_free(&arena);
}

static void test_size(void) {
    nsl_Arena arena = {0};

    int *i = nsl_arena_alloc(&arena, sizeof(*i));
    NSL_ASSERT(nsl_arena_size(&arena) == test_align(sizeof(*i)) &&
           "size not matching");
    NSL_ASSERT(nsl_arena_real_size(&arena) == 4000 && "chunk size does not match");

    nsl_arena_free(&arena);
}

static void test_null(void) {
    const char msg[] = "Hello World";
    char* buffer = nsl_arena_alloc_chunk(NULL, sizeof(msg));
    memcpy(buffer, msg, sizeof(msg));

    NSL_ASSERT(strcmp(buffer, msg) == 0);

    nsl_arena_free_chunk(NULL, buffer);
}

void run_test_arena(void);
void run_test_arena(void) {
    test_arena();
    test_chunks();
    test_calloc();
    test_reset();
    test_size();
    test_null();
}
