#!/bin/bash

function usage() 
{
  echo "count_see wht_plan [shuffle | unpack | scalar_add | vector_add | scalar_mov | vector_mov]"
  exit 1
}

path=`dirname $0`

vsize=2
unroll=8
ifactor=16
plan=$1
mode=$2

case ${mode} in
  'shuffle')
    ops='shuf';
    ;;
  'unpack')
    ops='unpcklpd|unpckhpd';
    ;;
  'scalar_add')
    ops='addsd|subsd';
    ;;
  'vector_add')
    ops='addpd|subpd';
    ;;
  'scalar_mov')
    ops='movsd|movlpd|movhpd';
    ;;
  'vector_mov')
    ops='movpd|movapd';
    ;;
  *)
    usage;
    ;;
esac;

tmp="/tmp/wht-${mode}"

if [ ! -e $tmp ]; then
  for ((n=1;n<=unroll;n+=1)); do
    x=`objdump -d ${path}/../../wht/codelets/s_${n}.o  | grep -E ${ops} | wc -l`
    echo "small[$n] : $x" >> ${tmp}
  done

  for ((n=vsize;n<=unroll;n+=1)); do
    x=`objdump -d ${path}/../../wht/codelets/s_${n}_v_${vsize}_a.o  | grep -E ${ops} | wc -l`
    echo "smallv($vsize)[$n] : $x" >> ${tmp}
  done

  for ((n=1;n<=unroll;n+=1)); do
    for ((k=2;k<=ifactor;k*=2)); do
      # TODO fix the v2
      x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}_v${vsize}.o  | grep -E ${ops} | wc -l`
      echo "smallv($vsize,$k,0)[$n] : $x" >> ${tmp}
      x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}_v${vsize}_a.o  | grep -E ${ops} | wc -l`
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

