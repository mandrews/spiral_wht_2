#ifndef CODELETS_H
#define CODELETS_H

#include "wht.h"
#include "dlfcn.h"

typedef void (*codelet)(Wht *W, long S, long D, wht_value *x);

codelet wht_get_codelet(const char *name);

#endif/*CODELETS_H*/

