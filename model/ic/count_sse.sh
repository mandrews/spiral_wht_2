#!/bin/bash

function usage() 
{
  echo "count_see [shuffle | unpack | scalar_add | vector_add | scalar_add | scalar_mov]"
  exit 1
}

path=`dirname $0`

v=2
m=4
i=8
s=$1

case $s in
  'shuffle')
    ops='[shuf]';
    ;;
  'unpack')
    ops='[unpcklpd|unpckhpd]';
    ;;
  'scalar_add')
    ops='[addsd|subsd]';
    ;;
  'vector_add')
    ops='[addpd|subpd]';
    ;;
  'scalar_mov')
    ops='[movsd|movlpd|movhpd]';
    ;;
  'vector_mov')
    ops='[movpd|movapd]';
    ;;
  *)
    usage;
    ;;
esac;

for ((n=v;n<=m;n+=1)); do
  x=`objdump -d ${path}/../../wht/codelets/s_${n}_v_${v}_a.o  | grep ${ops} | wc -l`
  echo "smallv($v)[$n] : $x"
done

for ((n=1;n<=m;n+=1)); do
  for ((k=2;k<=i;k*=2)); do
    # TODO fix the v2
    x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}_v${v}.o  | grep ${ops} | wc -l`
    echo "smallv($v,$k,0)[$n] : $x"
    x=`objdump -d ${path}/../../wht/codelets/s_${n}_il_${k}_v${v}_a.o  | grep ${ops} | wc -l`
    echo "smallv($v,$k,1)[$n] : $x"
  done
done
