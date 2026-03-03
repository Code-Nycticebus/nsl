# Roadmap

## 0.10.0 - release
- [ ] Create a StrBuffer and a ByteBuffer
    - replace the `nsl_Arena` in all the mutated string functions with a string builder instead. either in place or just copying (i would go with copying)
- [ ] cmd api
    - async
    - pipes
    - redirect
    - procs
- [ ] path
    - path relative to

## 1.0.0 - release
- [ ] timing functions
    - clock_gettime()
    - GetPerformanceCounter()
- [ ] arena
    - temp buffer?
    - scratch buffers? scratch can be begin,end so i can allocate in between? then i can just extract the scratch from the arena
- [ ] platform independent threads?
- [ ] documentation
    - arenas
