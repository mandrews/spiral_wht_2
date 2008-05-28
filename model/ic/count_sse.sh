#!/bin/bash

function usage() 
{
  echo "count_see -w wht_plan -i instruction [-m]"
  exit 1
}

path=`dirname $0`
vsize=`${path}/wht_predict -v | grep vector_size: | cut -d ' ' -f 2`
unroll=`${path}/wht_predict -v | grep max_unroll: | cut -d ' ' -f 2`
ifactor=`${path}/wht_predict -v | grep max_interleave: | cut -d ' ' -f 2`

wht='';
ins=''
mem=0

while getopts "w:i:m" i; do
  case "$i" in
    "w") wht=$OPTARG;
    ;;
    "i") ins=$OPTARG;
    ;;
    "m") mem=1;
    ;;
  esac
done

if [ "$ins" == "" ]; then
  usage
fi

if [ "$wht" == "" ] ; then
  read wht
fi

if [ "$mem" == 1 ]; then
  cmd=""
else
  cmd="-v"
fi

mkdir -p "${path}/../share"
md5=`echo "${ins} ${mem}" | md5sum | cut -d ' ' -f 1`
tmp="${path}/../share/${md5}"

if [ ! -e $tmp ]; then
  for ((n=1;n<=unroll;n+=1)); do
    f="${path}/../../wht/codelets/s_${n}.o"
    x=`objdump -d ${f} | grep -E ${ins} | grep -E ${cmd} "\(|\)" | wc -l`
    echo "small[$n] : $x" >> ${tmp}
  done

  for ((n=1;n<=unroll;n+=1)); do
    f="${path}/../../wht/codelets/s_${n}_v_${vsize}_a.o"
    x=`objdump -d ${f} | grep -E ${ins} | grep -E ${cmd} "\(|\)" | wc -l`
    echo "smallv($vsize)[$n] : $x" >> ${tmp}
  done

  for ((n=1;n<=unroll;n+=1)); do
    for ((k=2;k<=ifactor;k*=2)); do
      f="${path}/../../wht/codelets/s_${n}_il_${k}_v_${vsize}.o"
      x=`objdump -d ${f} | grep -E ${ins} | grep -E ${cmd} "\(|\)" | wc -l`
      echo "smallv($vsize,$k,0)[$n] : $x" >> ${tmp}
      f="${path}/../../wht/codelets/s_${n}_il_${k}_v_${vsize}_a.o"
      x=`objdump -d ${f} | grep -E ${ins} | grep -E ${cmd} "\(|\)" | wc -l`
      echo "smallv($vsize,$k,1)[$n] : $x" >> ${tmp}
      f="${path}/../../wht/codelets/s_${n}_il_${k}.o"
      x=`objdump -d ${f} | grep -E ${ins} | grep -E ${cmd} "\(|\)" | wc -l`
      echo "smallil($k)[$n] : $x" >> ${tmp}
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

