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
# File: make_spiral_codelets.sh
# Author: Michael Andrews
#

function usage()
{
  echo "make_spiral_codelets.sh: -n SIZE -d DIRECTORY"
  exit 1
}

path=`dirname $0`

whtgen="$path/../../whtgen/spiral2wht"

n=0
d=''
while getopts "n:d:" i; do
  case "$i" in
    "n") n=$OPTARG;
    ;;
    "d") d=$OPTARG;
    ;;
  esac
done

if [ "$n" -lt 1 ] ; then
  usage;
fi

if [ ! -e $d ] ; then
  usage;
fi

for ((i=1;i<=n;i+=1)); do
  echo "Generate: $whtgen -n $i > $path/spiral_$i.c"
  $whtgen -n $i $d > $path/spiral_$i.c
done