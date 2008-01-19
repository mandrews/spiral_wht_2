#include "codelets.h"
#include "codelet_registry.h"

codelet_apply_fp 
codelet_apply_lookup(Wht *W)
{
  codelet_apply_entry *p;
  codelet_apply_fp call;
  char *name;

  name = name_to_string(W);

  call  = NULL; /* Did not find codelet in table */

  p = (codelet_apply_entry *) codelet_apply_registry; 
  for (; p != NULL && (codelet_apply_fp) p->call != NULL; ++p)
    if (strcmp(name, p->name) == 0) {
      call = p->call;
      break;
    }

  i_free(name);

  return call;
}

