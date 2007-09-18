#include "codelets.h"
#include "codelet_registry.h"

codelet wht_get_codelet(const char *name)
{
  int i;

  for (i=0; i < wht_codelet_registry_size;i++)
    if (strncmp(name, wht_codelet_registry[i].name, MAX_CODELET_NAME_SIZE) == 0)
      return wht_codelet_registry[i].call;
  return NULL;
}

