#ifndef _NC_PATH_H_
#define _NC_PATH_H_ 

#include "nc/core/arena.h"
#include "nc/defines.h"

nc_Path nc_path_join(usize len, const nc_Path* parts, nc_Arena* arena);
nc_Path nc_path_normalize(nc_Path path, nc_Arena* arena);

bool nc_path_eq(nc_Path p1, nc_Path p2);
bool nc_path_is_absolute(nc_Path path);

nc_Str nc_path_name(nc_Path path);
nc_Str nc_path_suffix(nc_Path path);
nc_Str nc_path_stem(nc_Path path);
nc_Path nc_path_parent(nc_Path path);

#endif // _NC_PATH_H_
