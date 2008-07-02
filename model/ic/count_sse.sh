#!/bin/bash

function usage() 
{
  echo "count_see -w wht_plan -i instruction [-m] [-r]"
  exit 1
}

path=`dirname $0`
vsize=`${path}/wht_predict -v | grep vector_size: | cut -d ' ' -f 2`
unroll=`${path}/wht_predict -v | grep max_unroll: | cut -d ' ' -f 2`
ifactor=`${path}/wht_predict -v | grep max_interleave: | cut -d ' ' -f 2`

wht='';
ins=''
mem=0 # Count memory ops
reg=0 # Count register ops

while getopts "w:i:mr" i; do
  case "$i" in
    "w") wht=$OPTARG;
    ;;
    "i") ins=$OPTARG;
    ;;
    "m") mem=1;
    ;;
    "r") reg=1;
    ;;
  esac
done

if [ "$ins" == "" ]; then
  usage
fi

if [ "$wht" == "" ] ; then
  read wht
fi

if (( ("$mem" == 1) && ("$reg" == 1) )); then
  echo "Cannot set both -m and -r"
  usage
fi

function count() 
{
  file=$1
  name=$2

  f="${path}/../../wht/codelets/${file}.o"

  # Skip if codelet doesn't exist
  if [ ! -f $f ]; then
    return;
  fi

  if [ "$mem" == 1 ]; then
    x=`objdump -d ${f} | grep -E ${ins} | grep -E "\(|\)" | wc -l`;
  elif [ "$reg" == 1 ]; then
    x=`objdump -d ${f} | grep -E ${ins} | grep -E -v "\(|\)" | wc -l`;
  else
    x=`objdump -d ${f} | grep -E ${ins} | wc -l`;
  fi

  echo "$name : $x" >> ${tmp}
}

mkdir -p "${path}/../share"
md5=`echo "${ins} ${mem} ${reg}" | md5sum | cut -d ' ' -f 1`
tmp="${path}/../share/${md5}"

if [ ! -e $tmp ]; then
  for ((n=1;n<=unroll;n+=1)); do
    count "s_${n}" "small[${n}]"
  done

  for ((n=1;n<=unroll;n+=1)); do
    count "s_${n}_v_${vsize}_a" "smallv($vsize,1)[$n]"
    count "s_${n}_v_${vsize}" "smallv($vsize,0)[$n]"
  done

  for ((n=1;n<=unroll;n+=1)); do
    for ((k=2;k<=ifactor;k*=2)); do
      count "s_${n}_il_${k}_v_${vsize}" "smallv($vsize,$k,0)[$n]"
      count "s_${n}_il_${k}_v_${vsize}_a" "smallv($vsize,$k,1)[$n]"
      count "s_${n}_il_${k}" "smallil($k)[$n]"
    done
  done
  
  echo "split_alpha : 0" >> ${tmp}
  echo "split_beta_1 : 0" >> ${tmp}
  echo "split_beta_2 : 0" >> ${tmp}
  echo "split_beta_3 : 0" >> ${tmp}
  echo "splitil_alpha : 0" >> ${tmp}
  echo "splitil_beta_1 : 0" >> ${tmp}
  echo "splitil_beta_2 : 0" >> ${tmp}
  echo "splitil_beta_3 : 0" >> ${tmp}

  sort ${tmp} > ${tmp}~
  mv ${tmp}~ ${tmp}
fi

${path}/wht_count -b ${tmp} -w $wht | ${path}/wht_predict -f ${tmp}

