#ifndef _NSL_DLL_H_
#define _NSL_DLL_H_

#include "nsl/defines.h"

typedef struct {
  void *handle;
} nsl_Dll;

typedef void (*Function)(void);

NSL_API nsl_Error nsl_dll_load(nsl_Dll* dll, nsl_Path path);
NSL_API void nsl_dll_close(nsl_Dll *dll);

NSL_API Function nsl_dll_symbol(nsl_Dll *dll, nsl_Str symbol);

#endif // _NSL_DLL_H_
