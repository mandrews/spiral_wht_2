#include "codelets.h"

codelet wht_get_codelet(const char *name)
{
  void *handle;
  char *error;
  codelet function;

  handle = dlopen(0, RTLD_NOW | RTLD_GLOBAL);
  if (!handle)
    wht_error("Could not dlopen self.\n");

  dlerror(); /* Clear existing errors. */
  *(void **) (&function) = dlsym(handle, name);

  if ((error = dlerror()) != NULL)
    wht_error("Could not find dlsym %s.\n", name);

  dlclose(handle);

  return function;
}

