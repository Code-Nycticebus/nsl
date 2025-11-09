# Roadmap

## 0.9.2 - release
- [x] os
    - file is older than other file
    - clean up os functions
    - introduce `NSL_ERROR_PATH_TOO_LONG`
- [x] merge set and hashmap functions
    - if you need a set just create a hashmap and store a bool (stolen from godot)
    - i need to save the values :( (found out i don't)
- [x] os
    - get absolute path
- [x] cmd
    - rename `nsl_cmd_exec_list` and `nsl_cmd_exec` (`nsl_cmd_exec_argv`)
- [ ] documentation
    - map duplicate hash issue

## 1.0.0 - release
- [ ] ditch the src dir
    - clean up nsl.h
    - merge 'modules'
- [ ] timing functions
    - clock_gettime()
    - GetPerformanceCounter()
- [ ] string builder
    - replace the `nsl_Arena` in all the mutated string functions with a string builder instead. either in place or just copying (i would go with copying)
    - more string builder functions?
- [ ] path
    - path relative to
- [ ] cmd api
    - async
    - pipes
    - redirect
    - procs
- [ ] arena
    - temp buffer?
    - scratch buffers? scratch can be begin,end so i can allocate in between? then i can just extract the scratch from the arena
- [ ] platform indipendent threads?
- [ ] documentation
    - arenas
