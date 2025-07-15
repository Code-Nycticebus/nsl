#include "nsl/os/dir.h"

#include "nsl/core/error.h"
#include "nsl/types/int.h"
#include "nsl/types/path.h"

#include <string.h>
#include <sys/stat.h>

#if defined(_WIN32)
#    include <direct.h>
#    include <io.h>
#    define stat _stat
#    define access(path, mode) _access(path, mode)
#    define unlink(path) _unlink(path)
#else
#    include <unistd.h>
#endif

