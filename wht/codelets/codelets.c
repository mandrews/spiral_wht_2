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

#define NOT_END(p)  ((p != NULL) && (p->size != 0))

codelet codelet_apply_lookup(size_t size, const char *name, int params[], size_t n)
{
  codelet_entry *p;

  for (p = (codelet_entry *) wht_codelet_registry; NOT_END(p); p++)
    if ((size == p->size) &&
        (strncmp(name, p->name, MAX_CODELET_NAME_SIZE) == 0) &&
        (n == p->n) &&
        (params_equal(params, p->params, n))) {
      return p->call;
    }

  return NULL; /* Did not find codelet in table */
}

#undef NOT_END
