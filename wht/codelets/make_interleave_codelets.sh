#!/bin/sh

function usage()
{
  echo "make_interleave_codelets.sh: -n SIZE -k INTERLEAVE_FACTOR"
  exit 1
}

path=`dirname $0`

whtgen="$path/../../whtgen/whtgen-simd"

n=0
k=0
while getopts "n:k:" i; do
  case "$i" in
    "n") n=$OPTARG;
    ;;
    "k") k=$OPTARG;
    ;;
  esac
done

if [ "$n" -lt 1 ] ; then
  usage;
fi

if [ "$k" -lt 2 ] ; then
  usage;
fi


for ((i=1;i<=n;i+=1)); do
	for ((j=2;j<=k;j*=2)); do
		echo "Generate: $whtgen -n $i -k $j > $path/s_${i}_il_${j}.c"
		$whtgen -n $i -k $j > $path/s_${i}_il_${j}.c
	done
done
