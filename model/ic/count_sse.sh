#!/bin/bash

function usage() 
{
  echo "count_see -w wht_plan -i instruction [-m] [-r] [-x]"
  echo "  -m            Count memory to register instructions."
  echo "  -r            Count register to register instructions."
  echo "  -x            Clear the instruction count cache."
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
clr=0

while getopts "w:i:mrx" i; do
  case "$i" in
    "w") wht=$OPTARG;
    ;;
    "i") ins=$OPTARG;
    ;;
    "m") mem=1;
    ;;
    "r") reg=1;
    ;;
    "x") clr=1;
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

# Clear the cache
if [ "$clr" == 1 ]; then
  rm ${path}/../share/*
fi

function count() 
{
  file=$1
  name=$2

  f="${path}/../../wht/codelets/${file}.o"

  if [ ! -f $f ]; then
    return;
  fi

  call=`objdump -d $f`

  if [ "$mem" == 1 ]; then
    x=`echo "${call}" | grep -E ${ins} | grep -E "\(|\)" | wc -l`;
  elif [ "$reg" == 1 ]; then
    x=`echo "${call}" | grep -E ${ins} | grep -E -v "\(|\)" | wc -l`;
  else
    x=`echo "${call}" | grep -E ${ins} | wc -l`;
  fi

  echo "$name : $x" >> ${tmp}
}

mkdir -p "${path}/../share"
md5=`echo "${ins} ${mem} ${reg}" | md5sum | cut -d ' ' -f 1`
tmp="${path}/../share/${md5}"

if [ ! -e $tmp ]; then
  echo "Generating cache for instruction '$ins'" 1>&2

  for ((n=1;n<=unroll;n+=1)); do
    count "s_${n}" "small[${n}]"
  done

  v=$vsize
  p=$(($v >> 1)) # Log 2 of v, for v,v codelets

  count "s_${p}_v_${v}" "smallv($v)[$p]"

  for ((n=1;n<=unroll;n+=1)); do
    count "s_${n}_v_${v}" "smallv($v,0)[$n]"
    count "s_${n}_v_${v}_a" "smallv($v,1)[$n]"
  done

  for ((n=1;n<=unroll;n+=1)); do
    for ((k=2;k<=ifactor;k*=2)); do
      count "s_${n}_il_${k}_v_${v}" "smallv($v,$k,0)[$n]"
      count "s_${n}_il_${k}_v_${v}_a" "smallv($v,$k,1)[$n]"
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
  echo "splitv_alpha : 0" >> ${tmp}
  echo "splitv_beta_1 : 0" >> ${tmp}
  echo "splitv_beta_2 : 0" >> ${tmp}
  echo "splitv_beta_3 : 0" >> ${tmp}

  sort ${tmp} > ${tmp}~
  mv ${tmp}~ ${tmp}
fi

${path}/wht_count -b ${tmp} -w $wht | ${path}/wht_predict -f ${tmp}

