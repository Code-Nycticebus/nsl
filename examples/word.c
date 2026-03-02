#define NSL_IMPLEMENTATION
#define NSL_API static
#include "../nsl.h"

typedef struct {
    nsl_Str word;
    usize count;
} Occurence;

static i32 cmp(const void *a, const void *b) {
    return ((const Occurence *)b)->count - ((const Occurence *)a)->count;
}

typedef struct {
    nsl_Map map;
    nsl_List(Occurence) list;
} Occurences;

int main(int argc, const char **argv) {
    // using 'NSL_STR' for string literals and 'nsl_str_from_cstr' for 'char *'
    nsl_Str filename = argc < 2 ? NSL_STR(__FILE__) : nsl_str_from_cstr(argv[1]);

    // preparing the memory arena
    nsl_Arena arena = {0};

    // reading the entire file
    FILE *file;
    if (nsl_file_open(&file, filename, "r")) NSL_PANIC("could not open file!");
    nsl_Str content = nsl_file_read_str(file, &arena);
    nsl_file_close(file);

    // initializing the map and list to allocate memory inside the arena
    Occurences occurences = {
        .map.arena = &arena,
        .list.arena = &arena,
    };

    // for (word in content)
    for (nsl_Str word = {0}; nsl_str_try_chop_by_predicate(&content, nsl_char_is_space, &word);) {
        // skip empty words
        if (word.len == 0) continue;
        // hash the word
        u64 hash = nsl_str_hash(word);
        // lookup idx with hash
        const u64 *idx = nsl_map_get_or_insert(&occurences.map, hash, occurences.list.len);
        if (*idx == occurences.list.len) {
            // if idx was not in map insert the hash and idx into map and add occurence to list
            nsl_list_push(&occurences.list, (Occurence){.word = word, .count = 1});
        } else {
            NSL_ASSERT(nsl_str_eq(occurences.list.items[*idx].word, word) && "duplicate hash");
            // if idx was found, increase the count of occurence
            occurences.list.items[*idx].count++;
        }
    }

    // sort list with qsort (invalidates map)
    nsl_list_sort(&occurences.list, cmp);

    // list top 3 occurences
    for (usize i = 0; i < 3; i++) {
        Occurence *o = &occurences.list.items[i];
        printf("%ld: " NSL_STR_REPR ": %ld\n", i + 1, NSL_STR_ARG(o->word), o->count);
    }

    // free the memory
    nsl_arena_free(&arena);
    return 0;
}
