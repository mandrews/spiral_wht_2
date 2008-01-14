/*
 * Copyright (c) 2000 Carnegie Mellon University
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

/* Generating unrolled code for WHT building blocks
   ================================================

   This program creates C code for an unrolled 
     WHT_N with stride S
   on a vector x. S is a 2-power.

   The unrolled code is recursive!

   Only 2 temporary vars are used!
*/

/* Code used for unrolling (without the r-loop)
   --------------------------------------------
   WHT_N tensor 1_S:
   -----------------

   n1 = n/2;
   n2 = 1;
   for (i = 1; i <= n; i *= 2) {
     for (j = 0; j < n1; j++) {
       for (k = 0; k < n2; k++) {
         ind1 = (j * 2 * n2 + k) * s;
         ind2 = (j * 2 * n2 + n2 + k) * s;
         for (l = 0; l < s; l++) {
           a       = x[ind1];
           b       = x[ind2];  
           x[ind1] = a + b;
           x[ind2] = a - b;
           ind1++;
           ind2++;
         }
       }
     }
     n1 /= 2;
     n2 *= 2;
   }
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static void
usage() 
{
  printf("Usage: whtgen -n SIZE \n\n");
  printf("    -h            Show this help message.\n");
  printf("    -n SIZE       Size of codelet.\n");
  printf("\n");
  exit(1);
}

int 
main(int argc, char *argv[])
{
  long n, n1, n2, i, j, l;
  int k;
  char c;

  k = 0;

  opterr = 0;

  while ((c = getopt (argc, argv, "hn:")) != -1)
    switch (c) {
      case 'n':
        k = atoi(optarg);
        break;
      case 'h':
        usage();
      default:
        usage();
    }

  if (k < 1)
    usage();

  n = (1<< k);

  /* Header */
  printf("/* This file has been automatically generated by whtgen/whtgen.c. */\n");
  printf("\n");

  /* Meta Data */
  printf("/*\n");
  printf("EXTERNS: {\n");
  printf("extern codelet_apply_fp apply_small%d;\n", k);
  printf("}\n");
  printf("STRUCTS: {\n");
  printf("#if (%d <= WHT_MAX_UNROLL)\n", k);
  printf("CODELET_APPLY_ENTRY(\"small[%d]\", apply_small%d),\n", k, k);
  printf("#endif\n");
  printf("}\n");
  printf("*/\n");
  printf("\n");

  /* Includes */
  printf("#include \"wht.h\"\n");
  printf("\n");

  /* Function Signature */
  printf("void apply_small%d(Wht *W, long S, long U, wht_value *x)\n", k);
  printf("{\n");

  /* Compile guard */
  printf("#if (%d <= WHT_MAX_UNROLL)\n", k);

  /* Declarations */
  printf("  wht_value a, b;\n");
 
  /* Body */
  n1 = n/2;
  n2 = 1;
  for (i = 1; i <= n; i *= 2) {
    for (j = 0; j < n1; j++) {
      for (l = 0; l < n2; l++) {
        printf("  a = x[%ld * S];\n", j * 2 * n2 + l);
        printf("  b = x[%ld * S];\n", j * 2 * n2 + n2 + l);
        printf("  x[%ld * S] = a + b;\n", j * 2 * n2 + l);
        printf("  x[%ld * S] = a - b;\n", j * 2 * n2 + n2 + l);
      }
    }
    n1 /= 2;
    n2 *= 2;  
  }

  /* Compile guard */
  printf("#else\n");
  printf("   wht_exit(\"runtime guards should prevent this message\");\n");
  printf("#endif\n");

  /* Footer */
  printf("}\n"); 

  return 0;
}