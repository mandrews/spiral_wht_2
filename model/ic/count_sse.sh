#!/bin/bash

function usage() 
{
  echo "count_see -w wht_plan [shuffle | unpack | scalar_add | vector_add | scalar_mov | vector_mov]"
  exit 1
}

path=`dirname $0`
vsize=`${path}/wht_predict -v | grep vector_size: | cut -d ' ' -f 2`
unroll=`${path}/wht_predict -v | grep max_unroll: | cut -d ' ' -f 2`
ifactor=`${path}/wht_predict -v | grep max_interleave: | cut -d ' ' -f 2`

plan='';

while getopts "w:" i; do
  case "$i" in
    "w") plan=$OPTARG;
    ;;
  esac
done

if [ "$plan" == "" ] ; then
  read plan
  mode=$1
else
  mode=$3
fi

case ${mode} in
  'shuffle')
    ops='shuf';
    ;;
  'unpack')
    ops='unpcklpd|unpckhpd';
    ;;
  'scalar_add')
    ops='addsd|subsd|addss|subss';
    ;;
  'vector_add')
    ops='addpd|subpd|addps|subps';
    ;;
  'scalar_mov')
    ops='movsd|movlpd|movhpd|movhps|movlpd|movss';
    ;;
  'vector_mov')
    ops='movpd|movapd|movps|movaps';
    ;;
  *)
    usage;
    ;;
esac;

mkdir -p "${path}/../share"
tmp="${path}/../share/${mode}"

if [ ! -e $tmp ]; then
  for ((n=1;n<=unroll;n+=1)); do
    x=`objdump -d ${path}/../../wht/codelets/s_${n}.o  | grep -E ${ops} | wc -l`
    echo "small[$n] : $x" >> ${tmp}
  done

  for ((n=1;n<=unroll;n+=1)); do
    x=`objdump -d ${path}/../../wht/codelets/s_${n}_v_${vsize}_a.o  | grep -E ${ops} | wc -l`
    echo "smallv($vsize)[$n] : $x" >> ${tmp}
  done

  for ((n=1;n<=unroll;n+=1)); do
    for ((k=2;k<=ifactor;k*=2)); do
      x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}_v_${vsize}.o  | grep -E ${ops} | wc -l`
      echo "smallv($vsize,$k,0)[$n] : $x" >> ${tmp}
      x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}_v_${vsize}_a.o  | grep -E ${ops} | wc -l`
      echo "smallv($vsize,$k,1)[$n] : $x" >> ${tmp}
      x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}.o  | grep -E ${ops} | wc -l`
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

${path}/wht_count -b ${tmp} -w $plan | ${path}/wht_predict -f ${tmp}

