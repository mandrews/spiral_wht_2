/**
 * \file extensions.h
 *
 * \brief Mechanism for registering new measure extensions into the 
 * WHT package measurement subsystem.
 *
 * Each new extension registered should implement:
 * - \ref measure_extension::init
 * - \ref measure_extension::test
 * - \ref measure_extension::call
 * - \ref measure_extension::done
 *
 * Registration is performed by:
 *
 * -# Including symbols required by extension
 * \code
 * #include "my_extn.h"
 * \endcode
 * -# Defining the entry inside the table
 * \code
 * MEASURE_ENTRY("MY_EXTN", my_extn_init, my_extn_test, my_extn_call, my_extn_done),
 * \endcode
 *
 * Use macro guards to conditionally include headers and entries since the user
 * may not have this measurement library installed.
 */
#ifndef MEASURE_EXTENSIONS_H
#define MEASURE_EXTENSIONS_H

#include "wht.h"
#include "measure.h"

#ifdef HAVE_PAPI
#include "papi/papi_utils.h"
#endif

/**
 * \brief Registers a new measure extension with the WHT package.
 *
 * \param NAME  Identifier associated with extension
 * \param INIT  Function to initialize the extension
 * \param TEST 	Function to perform a test run without actually 
 * 							calling \ref Wht::apply.  Should replace \ref Wht::apply
 * 							with \ref null_apply
 * 							Used for calibration.
 * \param CALL  Function to actually perform measurement
 * \param DONE  Function to cleanup the extension
 */
#define MEASURE_ENTRY(NAME, INIT, TEST, CALL, DONE) \
  { NAME, \
		(measure_init_fp) &INIT, \
    (measure_call_fp) &TEST, \
    (measure_call_fp) &CALL, \
    (measure_done_fp) &DONE }

#define MEASURE_EXTENSIONS_END { NULL, NULL, NULL, NULL } 

/**
 * \var measure_extensions
 *
 * \brief Table of extensions to the measure subsystem.
 */
static const struct measure_extension
measure_extensions[] = {
#ifdef HAVE_PAPI
	MEASURE_ENTRY("PAPI", papi_init, papi_test, papi_call, papi_done),
#endif
  MEASURE_EXTENSIONS_END
};

#endif/*MEASURE_EXTENSIONS_H*/
