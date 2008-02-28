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
 * \file papi_utils.h
 * \author Michael Andrews
 *
 * \brief PAPI extension to measurement subsystem.
 */

#ifndef	PAPI_UTILS_H
#define PAPI_UTILS_H

#include "papi.h"

#include "wht.h"
#include "measure.h"

#ifndef PAPI_UTILS_DEBUG
#define PAPI_UTILS_DEBUG 0
#endif/*PAPI_UTILS_DEBUG*/

void papi_init();

void papi_done();

double papi_call();
char **papi_list();

measure_call_fp papi_prep(char *metric);

#endif /*PAPI_UTILS_H*/
