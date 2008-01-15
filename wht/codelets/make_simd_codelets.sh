#!/bin/sh

function usage()
{
  echo "make_simd_codelets.sh: -n SIZE -k INTERLEAVE_FACTOR -v VECTOR_SIZE"
  exit 1
}

path=`dirname $0`

whtgen="$path/../../whtgen/whtgen-simd"

n=0
k=0
v=0
while getopts "n:k:v:" i; do
  case "$i" in
    "n") n=$OPTARG;
    ;;
    "k") k=$OPTARG;
    ;;
    "v") v=$OPTARG;
    ;;
  esac
done

if [ "$n" -lt 1 ] ; then
  usage;
fi

if [ "$k" -lt 2 ] ; then
  usage;
fi

if [ "$v" -lt 2 ] ; then
  usage;
fi

for ((x=2;x<=v;x*=2)); do
for ((i=1;i<=n;i+=1)); do
  echo "Generate: $whtgen -n $i -v $x -a > $path/s_${i}_v_${x}_a.c"
  $whtgen -n $i -v $x -a > $path/s_${i}_v_${x}_a.c

	for ((j=2;j<=k;j*=2)); do
		echo "Generate: $whtgen -n $i -k $j -v $x > $path/s_${i}_il_${j}_v_${x}.c"
		$whtgen -n $i -k $j -v $x > $path/s_${i}_il_${j}_v_${x}.c
		echo "Generate: $whtgen -n $i -k $j -v $x -a > $path/s_${i}_il_${j}_v_${x}_a.c"
		$whtgen -n $i -k $j -v $x -a > $path/s_${i}_il_${j}_v_${x}_a.c
	done
done
done
