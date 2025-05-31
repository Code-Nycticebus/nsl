#include "nc/types/path.h"

#include "nc/defines.h"
#include "nc/types/str.h"
#include "nc/types/char.h"
#include "nc/types/int.h"
#include "nc/structs/list.h"

#include <stdio.h>
#include <string.h>

NC_API nc_Path nc_path_join(usize len, const nc_Path *parts, nc_Arena *arena) {
    if (len == 0) {
        return NC_STR("");
    }
    if (len == 1) {
        return nc_str_copy(parts[0], arena);
    }
    nc_Arena scratch = {0};
    nc_List(char) buffer = {0};
    nc_list_init(&buffer, &scratch);

    for (usize i = 0; i < len; i++) {
        nc_list_reserve(&buffer, parts[i].len + 2);
        if (i && buffer.len && !nc_char_is_path_delimiter(nc_list_last(&buffer))) {
            nc_list_push(&buffer, '/');
        }
        for (usize j = 0; j < parts[i].len; j++) {
            if (nc_char_is_path_delimiter(parts[i].data[j])) {
                if (buffer.len && nc_char_is_path_delimiter(nc_list_last(&buffer))) {
                    continue;
                }
                nc_list_push(&buffer, '/');
            } else {
                nc_list_push(&buffer, parts[i].data[j]);
            }
        }
    }


    nc_Str result = nc_str_copy(nc_str_from_parts(buffer.len, buffer.items), arena);
    nc_arena_free(&scratch);
    return result;
}

NC_API nc_Path nc_path_normalize(nc_Path path, nc_Arena *arena) {
    nc_Arena scratch = {0};
    nc_List(nc_Path) parts = {0};
    nc_list_init(&parts, &scratch);
    char win_path_prefix_buffer[4] = "C:/";

    nc_Path prefix = NC_PATH("");
    if (nc_path_is_absolute(path)) {
        if (path.len && nc_char_is_path_delimiter(path.data[0])) {
            prefix = NC_PATH("/");
        } else if (path.len > 2) {
            win_path_prefix_buffer[0] = path.data[0];
            prefix = nc_str_from_parts(3, win_path_prefix_buffer);
            path = nc_str_substring(path, 3, path.len);
        }
    }

    nc_Path part;
    while (nc_str_try_chop_by_predicate(&path, nc_char_is_path_delimiter, &part)) {
        if (part.len == 0) {
            continue;
        } else if (nc_str_eq(part, NC_STR("."))) {
            continue;
        } else if (nc_str_eq(part, NC_STR(".."))) {
            if (parts.len) (void)nc_list_pop(&parts);
            else if (!prefix.len) nc_list_push(&parts, part);
            continue;
        }
        nc_list_push(&parts, part);
    }

    nc_Path result = nc_path_join(parts.len, parts.items, &scratch);
    result = nc_str_prepend(result, prefix, arena);
    nc_arena_free(&scratch);
    return result;
}

NC_API bool nc_path_eq(nc_Path p1, nc_Path p2) {
    while (true) {
        nc_Path c1 = nc_str_chop_by_predicate(&p1, nc_char_is_path_delimiter);
        nc_Path c2 = nc_str_chop_by_predicate(&p2, nc_char_is_path_delimiter);
        if (c1.len == 0 && c2.len == 0) break;
        if (!nc_str_eq(c1, c2)) return false;;
    }
    return true;
}

NC_API bool nc_path_is_absolute(nc_Path path) {
    if (path.len == 0)
        return false;
    if (nc_char_is_path_delimiter(path.data[0]))
        return true;
    if (path.len >= 3 && path.data[1] == ':' && nc_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}

NC_API bool nc_path_is_root(nc_Path path) {
    if (path.len == 1 && nc_char_is_path_delimiter(path.data[0]))
        return true;
    if (path.len == 3 && path.data[1] == ':' && nc_char_is_path_delimiter(path.data[2]))
        return true;
    return false;
}

NC_API nc_Str nc_path_name(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    return nc_str_chop_right_by_predicate(&path, nc_char_is_path_delimiter);
}

NC_API nc_Str nc_path_suffix(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    nc_Str name = nc_str_chop_right_by_predicate(&path, nc_char_is_path_delimiter);
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return NC_STR("");
    }
    return nc_str_substring(name, idx, name.len);
}

NC_API nc_Str nc_path_stem(nc_Path path) {
    nc_Str name = nc_str_chop_right_by_predicate(&path, nc_char_is_path_delimiter);
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return name;
    }
    return nc_str_substring(name, 0, idx);
}

NC_API nc_Path nc_path_parent(nc_Path path) {
    if (path.len == 1 && nc_char_is_path_delimiter(path.data[0])) return path;
    if (nc_str_endswith_predicate(path, nc_char_is_path_delimiter)) path.len--;
    usize idx = nc_str_find_last_by_predicate(path, nc_char_is_path_delimiter);
    if (idx == NC_STR_NOT_FOUND) return NC_PATH(".");
    return nc_str_substring(path, 0, nc_usize_min(path.len, idx+1));
}
