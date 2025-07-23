#define NSL_IMPLEMENTATION
#define NSL_STATIC
#include "../nsl.h"

typedef struct {
    nsl_Str word;
    usize count;
} Occurences;

i32 cmp(const void *a, const void *b) {
    return ((const Occurences *)b)->count - ((const Occurences *)a)->count;
}

int main(int argc, const char **argv) {
    nsl_Str filename = argc < 2 ? NSL_STR(__FILE__) : nsl_str_from_cstr(argv[1]);

    nsl_Arena arena = {0};

    FILE* file;
    if (nsl_file_open(&file, filename, "r")) NSL_PANIC("could not open file!");
    nsl_Str content = nsl_file_read_str(file, &arena);
    nsl_file_close(file);

    nsl_Map map = {.arena = &arena};
    nsl_List(Occurences) occurences = {.arena = &arena};

    for (nsl_Str word = {0}; nsl_str_try_chop_by_predicate(&content, nsl_char_is_space, &word);) {
        if (word.len == 0) continue;
        u64 hash = nsl_str_hash(word);
        const u64 *idx = nsl_map_get_u64(&map, hash);
        if (idx == NULL) {
            nsl_map_insert_u64(&map, hash, occurences.len);
            nsl_list_push(&occurences, (Occurences){.word = word, .count = 1});
        } else {
            occurences.items[*idx].count++;
        }
    }

    nsl_list_sort(&occurences, cmp);

    for (usize i = 0; i < 3; i++) {
        Occurences *o = &occurences.items[i];
        printf("%ld: " NSL_STR_REPR ": %ld\n", i + 1, NSL_STR_ARG(o->word), o->count);
    }

    nsl_arena_free(&arena);
    return 0;
}
