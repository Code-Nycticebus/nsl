#include "nc/types/path.h"

#include "nc/types/str.h"
#include "nc/types/char.h"
#include "nc/structs/list.h"

#include <string.h>

nc_Path nc_path_join(usize len, nc_Path *parts, nc_Arena *arena) {
    if (len == 0) {
        return NC_STR("");
    }

    usize size = 0;
    for (usize i = 0; i < len; i++) {
        if (parts[i].len == 0) continue;
        if (i == 0) {
            parts[i] = nc_str_trim_right_by_delim(parts[i], '/');
        } else {
            size += 1;
            parts[i] = nc_str_trim_by_delim(parts[i], '/');
        }
        size += parts[i].len;
    }
    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < len; i++) {
        if (parts[i].len == 0) continue;
        if (i != 0) buffer[b_idx++] = '/';

        memcpy(&buffer[b_idx], parts[i].data, parts[i].len);
        b_idx += parts[i].len;
    }
    buffer[size] = '\0';

    return nc_str_from_parts(size, buffer);
}

nc_Path nc_path_normalize(nc_Path path, nc_Arena *arena) {
    nc_Arena scratch = {0};
    nc_List(nc_Path) parts = {0};
    nc_list_init(&parts, &scratch);

    const bool is_absolute = nc_path_is_absolute(path);

    nc_Str part;
    while (nc_str_try_chop_by_delim(&path, '/', &part)) {
        if (nc_str_eq(part, NC_STR("."))) {
            continue;
        }
        if (nc_str_eq(part, NC_STR(".."))) {
            if (parts.len && !nc_str_eq(nc_list_last(&parts), NC_STR(".."))) {
                (void)nc_list_pop(&parts);
            } else if (!is_absolute) {
                nc_list_push(&parts, part);
            }
            continue;
        }
        nc_list_push(&parts, part);
    }

    nc_Path temp = nc_path_join(parts.len, parts.items, &scratch);
    nc_Path result = is_absolute && !nc_path_is_absolute(temp)
                         ? nc_str_prepend(temp, NC_STR("/"), arena)
                         : nc_str_copy(temp, arena);

    nc_arena_free(&scratch);
    return result;
}

bool nc_path_eq(nc_Path p1, nc_Path p2) {
    while (true) {
        nc_Path c1 = nc_str_chop_by_delim(&p1, '/');
        nc_Path c2 = nc_str_chop_by_delim(&p2, '/');
        if (c1.len == 0 && c2.len == 0) break;
        if (!nc_str_eq(c1, c2)) return false;;
    }
    return true;
}

bool nc_path_is_absolute(nc_Path path) {
    if (path.len == 0)
        return false;
    if (path.data[0] == '/')
        return true;
    return false;
}

nc_Str nc_path_name(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    return nc_str_chop_right_by_delim(&path, '/');
}

nc_Str nc_path_suffix(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    nc_Str name = nc_str_chop_right_by_delim(&path, '/');
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return NC_STR("");
    }
    return nc_str_substring(name, idx, name.len);
}

nc_Str nc_path_stem(nc_Path path) {
    nc_Str name = nc_str_chop_right_by_delim(&path, '/');
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return name;
    }
    return nc_str_substring(name, 0, idx);
}

nc_Path nc_path_parent(nc_Path path) {
    if (nc_str_endswith(path, NC_STR("/"))) {
        path.len--;
    }
    (void)nc_str_chop_right_by_delim(&path, '/');
    return path;
}
