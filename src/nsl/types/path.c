#include "nsl/types/path.h"

#include "nsl/defines.h"
#include "nsl/types/str.h"
#include "nsl/types/char.h"
#include "nsl/types/int.h"
#include "nsl/structs/list.h"

#include <stdio.h>
#include <string.h>

NSL_API nsl_Path nsl_path_join(usize len, const nsl_Path *parts, nsl_Arena *arena) {
    if (len == 0) {
        return NSL_STR("");
    }
    if (len == 1) {
        return nsl_str_copy(parts[0], arena);
    }
    nsl_Arena scratch = {0};
    nsl_List(char) buffer = {0};
    nsl_list_init(&buffer, &scratch);

    for (usize i = 0; i < len; i++) {
        nsl_list_reserve(&buffer, parts[i].len + 2);
        if (i && buffer.len && !nsl_char_is_path_delimiter(nsl_list_last(&buffer))) {
            nsl_list_push(&buffer, '/');
        }
        for (usize j = 0; j < parts[i].len; j++) {
            if (nsl_char_is_path_delimiter(parts[i].data[j])) {
                if (buffer.len && nsl_char_is_path_delimiter(nsl_list_last(&buffer))) {
                    continue;
                }
                nsl_list_push(&buffer, '/');
            } else {
                nsl_list_push(&buffer, parts[i].data[j]);
            }
        }
    }


    nsl_Str result = nsl_str_copy(nsl_str_from_parts(buffer.len, buffer.items), arena);
    nsl_arena_free(&scratch);
    return result;
}

NSL_API nsl_Path nsl_path_normalize(nsl_Path path, nsl_Arena *arena) {
    nsl_Arena scratch = {0};
    nsl_List(nsl_Path) parts = {0};
    nsl_list_init(&parts, &scratch);
    char win_path_prefix_buffer[4] = "C:/";

    nsl_Path prefix = NSL_PATH("");
    if (nsl_path_is_absolute(path)) {
        if (path.len && nsl_char_is_path_delimiter(path.data[0])) {
            prefix = NSL_PATH("/");
        } else if (path.len > 2) {
            win_path_prefix_buffer[0] = path.data[0];
            prefix = nsl_str_from_parts(3, win_path_prefix_buffer);
            path = nsl_str_substring(path, 3, path.len);
        }
    }

    nsl_Path part;
    while (nsl_str_try_chop_by_predicate(&path, nsl_char_is_path_delimiter, &part)) {
        if (part.len == 0) {
            continue;
        } else if (nsl_str_eq(part, NSL_STR("."))) {
            continue;
        } else if (nsl_str_eq(part, NSL_STR(".."))) {
            if (parts.len) (void)nsl_list_pop(&parts);
            else if (!prefix.len) nsl_list_push(&parts, part);
            continue;
        }
        nsl_list_push(&parts, part);
    }

    nsl_Path result = nsl_path_join(parts.len, parts.items, &scratch);
    result = nsl_str_prepend(result, prefix, arena);
    nsl_arena_free(&scratch);
    return result;
}

NSL_API bool nsl_path_eq(nsl_Path p1, nsl_Path p2) {
    while (true) {
        nsl_Path c1 = nsl_str_chop_by_predicate(&p1, nsl_char_is_path_delimiter);
        nsl_Path c2 = nsl_str_chop_by_predicate(&p2, nsl_char_is_path_delimiter);
        if (c1.len == 0 && c2.len == 0) break;
        if (!nsl_str_eq(c1, c2)) return false;;
    }
    return true;
}

NSL_API bool nsl_path_is_absolute(nsl_Path path) {
    if (path.len == 0)
        return false;
    if (nsl_char_is_path_delimiter(path.data[0]))
        return true;
    if (path.len >= 3 && path.data[1] == ':' && nsl_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}

NSL_API bool nsl_path_is_root(nsl_Path path) {
    if (path.len == 1 && nsl_char_is_path_delimiter(path.data[0]))
        return true;
    if (path.len == 3 && path.data[1] == ':' && nsl_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}

NSL_API nsl_Str nsl_path_name(nsl_Path path) {
    if (nsl_str_eq(path, NSL_STR("."))) {
        return NSL_STR("");
    }
    return nsl_str_chop_right_by_predicate(&path, nsl_char_is_path_delimiter);
}

NSL_API nsl_Str nsl_path_suffix(nsl_Path path) {
    if (nsl_str_eq(path, NSL_STR("."))) {
        return NSL_STR("");
    }
    nsl_Str name = nsl_str_chop_right_by_predicate(&path, nsl_char_is_path_delimiter);
    usize idx = nsl_str_find_last(name, NSL_STR("."));
    if (idx == NSL_STR_NOT_FOUND) {
        return NSL_STR("");
    }
    return nsl_str_substring(name, idx, name.len);
}

NSL_API nsl_Str nsl_path_stem(nsl_Path path) {
    nsl_Str name = nsl_str_chop_right_by_predicate(&path, nsl_char_is_path_delimiter);
    usize idx = nsl_str_find_last(name, NSL_STR("."));
    if (idx == NSL_STR_NOT_FOUND) {
        return name;
    }
    return nsl_str_substring(name, 0, idx);
}

NSL_API nsl_Path nsl_path_parent(nsl_Path path) {
    if (path.len == 1 && nsl_char_is_path_delimiter(path.data[0])) return path;
    if (nsl_str_endswith_predicate(path, nsl_char_is_path_delimiter)) path.len--;
    usize idx = nsl_str_find_last_by_predicate(path, nsl_char_is_path_delimiter);
    if (idx == NSL_STR_NOT_FOUND) return NSL_PATH(".");
    return nsl_str_substring(path, 0, nsl_usize_min(path.len, idx+1));
}
