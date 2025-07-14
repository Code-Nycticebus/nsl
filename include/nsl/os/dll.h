#ifndef _NSL_DLL_
#define _NSL_DLL_

#include "nsl/defines.h"

typedef struct {
  void *handle;
} nsl_Dll;

typedef void (*Function)(void);

nsl_Error dll_load(nsl_Dll* dll, nsl_Path path);
void dll_close(nsl_Dll *dll);

Function dll_symbol(nsl_Dll *dll, nsl_Str symbol);

#endif // _NSL_DLL_
