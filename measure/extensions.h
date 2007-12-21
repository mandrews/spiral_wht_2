#ifndef MEASURE_EXTENSIONS_H
#define MEASURE_EXTENSIONS_H

#include "wht.h"
#include "measure.h"

#ifdef HAVE_PAPI
#include "papi/papi_utils.h"
#endif

#define MEASURE_EXTENSIONS_END { NULL, NULL, NULL, NULL } 

static const struct measure_extension
measure_extensions[] = {
#ifdef HAVE_PAPI
  { "PAPI", 
    (measure_init_fp)  &papi_init, 
    (measure_call_fp)  &papi_test, 
    (measure_call_fp)  &papi_call, 
    (measure_done_fp)  &papi_done 
  },
#endif
  MEASURE_EXTENSIONS_END
};

#endif/*MEASURE_EXTENSIONS_H*/
