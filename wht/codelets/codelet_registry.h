#include "wht.h"

extern codelet apply_small1;

static const int wht_codelet_registry_size = 1;

static const codelet_entry
wht_codelet_registry[] =
{
  { "apply_small1",
    &apply_small1
  },
};
