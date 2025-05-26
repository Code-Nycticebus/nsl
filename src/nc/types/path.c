#include "nc/types/path.h"

#include "nc/types/str.h"
#include "nc/types/char.h"
#include "nc/types/int.h"
#include "nc/structs/list.h"

#include <stdio.h>
#include <string.h>

static bool _nc_path_separator(char c) {
    return c == '/' || c == '\\';
}

nc_Path nc_path_join(usize len, const nc_Path *parts, nc_Arena *arena) {
    if (len == 0) {
        return NC_STR("");
    }
    if (len == 1) {
        return nc_str_copy(parts[0], arena);
    }

    usize size = 0;
    for (usize i = 0; i < len; i++) {
        nc_Path part = parts[i];
        if (part.len == 0) continue;
        if (size == 0) {
            part = nc_str_trim_right_by_predicate(part, _nc_path_separator);
        } else {
            size += 1;
            part = nc_str_trim_by_predicate(part, _nc_path_separator);
        }
        size += part.len;
    }

    char *buffer = nc_arena_alloc(arena, size + 1);
    usize b_idx = 0;
    for (usize i = 0; i < len; i++) {
        nc_Path part = parts[i];
        if (part.len == 0) continue;
        if (b_idx == 0) {
            part = nc_str_trim_right_by_predicate(part, _nc_path_separator);
        } else {
            part = nc_str_trim_by_predicate(part, _nc_path_separator);
            buffer[b_idx++] = '/';
        }

        memcpy(&buffer[b_idx], part.data, part.len);
        b_idx += part.len;
    }
    buffer[size] = '\0';

    return nc_str_from_parts(size, buffer);
}

bool nc_path_eq(nc_Path p1, nc_Path p2) {
    while (true) {
        nc_Path c1 = nc_str_chop_by_predicate(&p1, _nc_path_separator);
        nc_Path c2 = nc_str_chop_by_predicate(&p2, _nc_path_separator);
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
    if (path.len >= 3 && path.data[1] == ':' && _nc_path_separator(path.data[2]))
        return true;
    return false;
}

nc_Str nc_path_name(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    return nc_str_chop_right_by_predicate(&path, _nc_path_separator);
}

nc_Str nc_path_suffix(nc_Path path) {
    if (nc_str_eq(path, NC_STR("."))) {
        return NC_STR("");
    }
    nc_Str name = nc_str_chop_right_by_predicate(&path, _nc_path_separator);
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return NC_STR("");
    }
    return nc_str_substring(name, idx, name.len);
}

nc_Str nc_path_stem(nc_Path path) {
    nc_Str name = nc_str_chop_right_by_predicate(&path, _nc_path_separator);
    usize idx = nc_str_find_last(name, NC_STR("."));
    if (idx == NC_STR_NOT_FOUND) {
        return name;
    }
    return nc_str_substring(name, 0, idx);
}

nc_Path nc_path_parent(nc_Path path) {
    if (path.len == 1 && _nc_path_separator(path.data[0])) return path;
    if (nc_str_endswith_predicate(path, _nc_path_separator)) path.len--;
    usize idx = nc_str_find_last_by_predicate(path, _nc_path_separator);
    if (idx == NC_STR_NOT_FOUND) return NC_PATH(".");
    return nc_str_substring(path, 0, nc_usize_min(path.len, idx+1));
}
