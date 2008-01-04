#!/bin/bash

function usage() 
{
  echo "count_see wht_plan [shuffle | unpack | scalar_add | vector_add | scalar_add | scalar_mov]"
  exit 1
}

path=`dirname $0`

vsize=2
unroll=4
ifactor=8
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

tmp=`mktemp`

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
  done
done

${path}/wht_count -w $plan | ${path}/wht_predict -f ${tmp}

rm ${tmp}
