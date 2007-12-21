/* 
 * File: papi_utils.h
 * Author: Michael Andrews <mjand@drexel.edu>
 *
 */
#ifndef	PAPI_UTILS_H
#define PAPI_UTILS_H

#include "papi.h"

#include "wht.h"
#include "stat.h"

#ifndef PAPI_UTILS_DEBUG
#define PAPI_UTILS_DEBUG 0
#endif/*PAPI_UTILS_DEBUG*/

void papi_init();

void papi_done();

stat_unit papi_call(Wht *W, wht_value *x, char *metric);
stat_unit papi_test(Wht *W, wht_value *x, char *metric);

#endif /*PAPI_UTILS_H*/
