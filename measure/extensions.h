/*
 * Copyright (c) 2007 Drexel University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**
 * \file extensions.h
 * \author Michael Andrews
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
 * \param DONE  Function to cleanup the extension
 */
#define MEASURE_ENTRY(NAME, INIT, LIST, PREP, DONE) \
  { NAME, \
		(measure_init_fp) &INIT, \
		(measure_list_fp) &LIST, \
		(measure_prep_fp) &PREP, \
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
	MEASURE_ENTRY("PAPI", papi_init, papi_list, papi_prep, papi_done),
#endif
  MEASURE_EXTENSIONS_END
};

#endif/*MEASURE_EXTENSIONS_H*/
