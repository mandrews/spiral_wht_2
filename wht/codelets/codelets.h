#ifndef CODELETS_H
#define CODELETS_H

#include "wht.h"

#define CODELET_REGISTRY_END { 0, "", { 0 }, 0, (codelet) NULL }

codelet codelet_apply_lookup(size_t size, const char *name, int params[], size_t n);

#endif/*CODELETS_H*/

