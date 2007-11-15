#ifndef MEASURE_EXTENSIONS_H
#define MEASURE_EXTENSIONS_H

#include "wht.h"
#include "measure.h"

#include "papi_utils.h"

#define MEASURE_EXTENSIONS_END { NULL, NULL, NULL, NULL } 

static const struct measure_extension
measure_extensions[] = {
  { "BUILTIN", 
    (measure_init_fp)  &builtin_init,
    (measure_call_fp)  &builtin_call,
    (measure_done_fp)  &builtin_done
  },
  { "PAPI", 
    (measure_init_fp)  &papi_init2, 
    (measure_call_fp)  &papi_call, 
    (measure_done_fp)  &papi_done 
  },
  MEASURE_EXTENSIONS_END
};

#endif/*MEASURE_EXTENSIONS_H*/
