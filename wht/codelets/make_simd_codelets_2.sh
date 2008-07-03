#!/bin/sh

#
# Copyright (c) 2007 Drexel University
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#


#
# File: make_simd_codelets_2.sh
# Author: Michael Andrews
#

function usage()
{
  echo "make_simd_codelets_2.sh"
  exit 1
}

path=`dirname $0`

echo "Generate: $path/s_1_v_2.c"
cat > s_1_v_2.c <<TEXT
/*
EXTERNS: {
  extern apply_fp apply_small1_v2;
}

STRUCTS: {
#if ((1 <= WHT_MAX_UNROLL) && (2 == WHT_VECTOR_SIZE))
  CODELET_APPLY_ENTRY("smallv(2)[1]", apply_small1_v2),
#endif
 }
*/

#include "wht.h"
#include "simd.h"

void apply_small1_v2(Wht *W, long S, long U, wht_value *x)
{
#if ((1 <= WHT_MAX_UNROLL) && (2 == WHT_VECTOR_SIZE))
  wht_vector2 ta1;
  wht_vector2 ta2;
  wht_vector2 ta3;
  wht_vector2 ta4;
  wht_vector2 ta5;
  wht_vector2 ta6;
  wht_vector2 ta7;
  wht_vector2 ta8;

  vload2(ta1,x[0]);
  vload2(ta2,x[WHT_VECTOR_SIZE]);

  vshuf2(ta3,ta1,ta2,0,0);
  vshuf2(ta4,ta1,ta2,1,1);

  vadd2(ta5,ta3,ta4);
  vsub2(ta6,ta3,ta4);

  vshuf2(ta7,ta5,ta6,0,0);
  vshuf2(ta8,ta5,ta6,1,1);

  vstore2(ta7,x[0]);
  vstore2(ta8,x[WHT_VECTOR_SIZE]);
#else
  wht_exit("initialization guards should prevent this message");
#endif
}

TEXT

echo "Generate: $path/s_2_v_4.c"
cat > s_2_v_4.c <<TEXT
/*
EXTERNS: {
  extern apply_fp apply_small2_v4;
}

STRUCTS: {
#if ((2 <= WHT_MAX_UNROLL) && (4 == WHT_VECTOR_SIZE))
  CODELET_APPLY_ENTRY("smallv(4)[2]", apply_small2_v4),
#endif
 }
*/

#include "wht.h"
#include "simd.h"

void apply_small2_v4(Wht *W, long S, long U, wht_value *x)
{
#if ((2 <= WHT_MAX_UNROLL) && (4 == WHT_VECTOR_SIZE))
  wht_vector4 ta1;
  wht_vector4 ta2;
  wht_vector4 ta3;
  wht_vector4 ta4;
  wht_vector4 ta5;
  wht_vector4 ta6;
  wht_vector4 ta7;
  wht_vector4 ta8;
  wht_vector4 ta9;
  wht_vector4 ta10;
  wht_vector4 ta11;
  wht_vector4 ta12;
  wht_vector4 ta13;
  wht_vector4 ta14;
  wht_vector4 ta15;
  wht_vector4 ta16;

  vload4(ta1,x[0]);
  vload4(ta2,x[S]);
  vload4(ta3,x[2 * S]);
  vload4(ta4,x[3 * S]);

  /* L_V^2_V */
  vunpacklo4(ta5, ta1, ta2);
  vunpackhi4(ta6, ta1, ta2)

  vunpacklo4(ta7, ta3, ta4);
  vunpackhi4(ta8, ta3, ta4);

  vshuf4(ta9,  ta5, ta7, 1, 0, 1, 0);
  vshuf4(ta10, ta5, ta7, 3, 2, 3, 2);
  vshuf4(ta11, ta6, ta8, 1, 0, 1, 0);
  vshuf4(ta12, ta6, ta8, 3, 2, 3, 2);

  vadd4(ta13,ta9,ta10);
  vsub4(ta14,ta9,ta10);
  vadd4(ta15,ta11,ta12);
  vsub4(ta16,ta11,ta12);

  vadd4(ta9, ta13,ta15);
  vsub4(ta10,ta13,ta15);
  vadd4(ta11,ta14,ta16);
  vsub4(ta12,ta14,ta16);

  vunpacklo4(ta5, ta9, ta11);
  vunpackhi4(ta6, ta9, ta11);

  vunpacklo4(ta7, ta10, ta12);
  vunpackhi4(ta8, ta10, ta12);

  vshuf4(ta9,  ta5, ta7, 1, 0, 1, 0);
  vshuf4(ta10, ta5, ta7, 3, 2, 3, 2);
  vshuf4(ta11, ta6, ta8, 1, 0, 1, 0);
  vshuf4(ta12, ta6, ta8, 3, 2, 3, 2);

  vstore4(ta9,x[0]);
  vstore4(ta10,x[S]);
  vstore4(ta11,x[2 * S]);
  vstore4(ta12,x[3 * S]);
#else
  wht_exit("initialization guards should prevent this message");
#endif
}

TEXT

