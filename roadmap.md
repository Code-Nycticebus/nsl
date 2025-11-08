# Roadmap

## 0.9.2 - release
- [ ] file
    - file is older than other file
- [ ] dir
    - currently just a dir iterator. should have directory functions like 'os_mkdir()'. or stat or stuff
    - get absolute path
- [x] merge set and hashmap functions
    - if you need a set just create a hashmap and store a bool (stolen from godot)
    - i need to save the values :( (found out i don't)

## 1.0.0 - release
- [ ] ditch the src dir
    - clean up nsl.h
- [ ] string builder
    - replace the `nsl_Arena` in all the mutated string functions with a string builder instead. either in place or just copying (i would go with copying)
    - more string builder functions?
- [ ] cmd api
    - async
    - pipes
    - redirect
    - procs
- [ ] arena
    - temp buffer?
- [ ] platform indipendent threads?
- [ ] documentation
    - map duplicate hash issue
