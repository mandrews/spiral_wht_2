#!/bin/sh

function usage()
{
  echo "make_small_codelets.sh: -n SIZE"
  exit 1
}

path=`dirname $0`

# NOTE: This version only uses two temporaries
#whtgen="$path/../../whtgen/whtgen"
whtgen="$path/../../whtgen/whtgen-simd"

n=0
while getopts "n:" i; do
  case "$i" in
    "n") n=$OPTARG;
    ;;
  esac
done

if [ "$n" -lt 1 ] ; then
  usage;
fi

for ((i=1;i<=n;i+=1)); do
  echo "Generate: $whtgen -n $i > $path/s_$i.c"
  $whtgen -n $i > $path/s_$i.c
done
