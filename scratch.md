# Nycticebus
A rewrite of my other stb called cebus.

## Goals

### Trim the fat
the old version has very weird and not really useful api's like `cebus_log` and `cebus_assert` which don't really add things that are not already in the standard library.
i will just focus on stuff that could be useful to add things or make working with different things easier.

### Rename
i renamed the entire library to be nycticebus as it's more fiting i think. the prefix for functions are then `nc_` which is not that long and can improve compatibility.
i want to actually rename things better. i don't know if i want to keep the name `Arena` since it's not really an arena allocator and more like a lifetime.

### Use `nc_build` from the start
i want the library to be integratable in the projects via `nc_build` which is gonna be a better solution than to make it single header i think. I still think the single header is a good idea though.

### Write comments from the start.
Yeah just write the comments for functions as if i was paid to do it.


