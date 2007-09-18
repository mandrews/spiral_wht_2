#include "codelets.h"
#include "codelet_registry.h"

bool
params_equal(int a[], int b[], size_t n)
{
  size_t i;

  for (i = 0; i < n; i++)
    if (a[i] != b[i])
      return false;

  return true;
}

codelet wht_get_codelet(size_t size, const char *name, int params[], size_t n)
{
  int i;

  for (i=0; i < wht_codelet_registry_size;i++)
    if ((size == wht_codelet_registry[i].size) &&
        (strncmp(name, wht_codelet_registry[i].name, MAX_CODELET_NAME_SIZE) == 0) &&
        (n == wht_codelet_registry[i].n) &&
        (params_equal(params, wht_codelet_registry[i].params, n))) {
      return wht_codelet_registry[i].call;
    }
  return NULL;
}

