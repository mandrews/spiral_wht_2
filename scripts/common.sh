#!/usr/bin/env bash

EMAIL=ma53@drexel.edu

# Installed
PATH="$PATH:${root}/extra:${root}/bin" 
# Development
PATH="$PATH:${root}/wht:${root}/measure:${root}/model/ic:${root}/model/cm:${root}/rand" 

L1=`perl -e "print (log(${L1_SIZE} / ${SINGLE_SIZE}) / log(2)); print \"\n\""`
L2=`perl -e "print (log(${L2_SIZE} / ${SINGLE_SIZE}) / log(2)); print \"\n\""`

L0_A=1
L0_B=$(($L1 - 1))
L1_A=$L1
L1_B=$(($L2 - 1))
L2_A=$L2
L2_B=24

wht_dp=`which wht_dp`
wht_strip=`which wht_strip`
wht_wrap=`which wht_wrap`
wht_measure=`which wht_measure`
wht_attach=`which wht_attach`
wht_cm_count=`which wht_cm_count`
wht_classify=`which wht_classify`
wht_rand=`which wht_rand`
wht_rotate=`which wht_rotate`
wht_vectorize=`which wht_vectorize`
wht_vectorize_2=`which wht_vectorize_2`
wht_count_ins=`which wht_ic`

rdtsc="$wht_measure -m rdtsc"

TOT_INS="$wht_measure -m PAPI_TOT_INS"
L1_DCM="$wht_measure -m PAPI_L1_DCM"
L2_DCM="$wht_measure -m PAPI_L2_DCM"

cmc1="$wht_cm_count -c ${L1_SIZE} -b ${L2_BLOCK} -a ${L2_ASSOC}"
cmc2="$wht_cm_count -c ${L2_SIZE} -b ${L2_BLOCK} -a ${L2_ASSOC}"

left="$wht_classify | grep left_tree | sed -e 's/.*:\s*//g'"
right="$wht_classify | grep right_tree | sed -e 's/.*:\s*//g'"
base="$wht_classify | grep right_node | sed -e 's/.*:\s*//g'"

# reg
addsd="$wht_count_ins -o 'addsd' -r"
subsd="$wht_count_ins -o 'subsd' -r"
addpd="$wht_count_ins -o 'addpd' -r"
subpd="$wht_count_ins -o 'subpd' -r"
addss="$wht_count_ins -o 'addss' -r"
subss="$wht_count_ins -o 'subss' -r"
addps="$wht_count_ins -o 'addps' -r"
subps="$wht_count_ins -o 'subps' -r"
shufpd="$wht_count_ins -o 'shufpd' -r"
shufps="$wht_count_ins -o 'shufps' -r"
unpckhpd="$wht_count_ins -o 'unpckhpd' -r"
unpcklpd="$wht_count_ins -o 'unpcklpd' -r"
unpckhps="$wht_count_ins -o 'unpckhps' -r"
unpcklps="$wht_count_ins -o 'unpcklps' -r"
movapd="$wht_count_ins -o 'movapd ' -r"
movaps="$wht_count_ins -o 'movaps ' -r"
shl="$wht_count_ins -o 'shl' -r"
xor="$wht_count_ins -o 'xor' -r"
mov="$wht_count_ins -o 'mov' -r"
push="$wht_count_ins -o 'push' -r"
pop="$wht_count_ins -o 'pop' -r"
nop="$wht_count_ins -o 'nop' -r"
retq="$wht_count_ins -o 'retq' -r"
add="$wht_count_ins -o 'add' -r"
sub="$wht_count_ins -o 'sub' -r"
imul="$wht_count_ins -o 'imul' -r"
# mem
movsdm="$wht_count_ins -o 'movsdm' -m"
movapdm="$wht_count_ins -o 'movapdm' -m"
movssm="$wht_count_ins -o 'movssm' -m"
movapsm="$wht_count_ins -o 'movapsm' -m"
movhpdm="$wht_count_ins -o 'movhpdm' -m"
movlpdm="$wht_count_ins -o 'movlpdm' -m"
movhpsm="$wht_count_ins -o 'movhpsm' -m"
movlpsm="$wht_count_ins -o 'movlpsm' -m"
leam="$wht_count_ins -o 'leam' -m"
movm="$wht_count_ins -o 'movm' -m"
# special
ic_papi="$wht_count_ins -o 'all'"
ic_all="$wht_count_ins -o 'papi'"


function run() 
{
  file=$1
  func=$2
  fields=$3
  range=$4
  tmp=`mktemp`
  cat ${file} | $wht_wrap "${func}" "${fields}" "${range}" > ${tmp}
  mv ${tmp} ${file} 
}

function metrics_single()
{
  file=$1

  run $file "$rdtsc $params_0" 'rdtsc' "$L0_A..$L0_B"
  run $file "$TOT_INS $params_0" 'TOT_INS' "$L0_A..$L0_B"
  run $file "$L1_DCM $params_0" 'L1_DCM' "$L0_A..$L0_B"
  run $file "$L2_DCM $params_0" 'L2_DCM' "$L0_A..$L0_B"

  run $file "$rdtsc $params_1" 'rdtsc' "$L1_A..$L1_B"
  run $file "$TOT_INS $params_1" 'TOT_INS' "$L1_A..$L1_B"
  run $file "$L1_DCM $params_1" 'L1_DCM' "$L1_A..$L1_B"
  run $file "$L2_DCM $params_1" 'L2_DCM' "$L1_A..$L1_B"

  run $file "$rdtsc $params_2" 'rdtsc' "$L2_A..$L2_B"
  run $file "$TOT_INS $params_2" 'TOT_INS' "$L2_A..$L2_B"
  run $file "$L1_DCM $params_2" 'L2_DCM' "$L2_A..$L2_B"
  run $file "$L2_DCM $params_2" 'L2_DCM' "$L2_A..$L2_B"

  run $file "$addss" 'addss'
  run $file "$subss" 'subss'
  run $file "$addps" 'addps'
  run $file "$subps" 'subps'
  run $file "$shufps" 'shufps'
  run $file "$unpckhps" 'unpckhps'
  run $file "$unpcklps" 'unpcklps'
  run $file "$movaps" 'movaps'
  run $file "$shl" 'shl'
  run $file "$xor" 'xor'
  run $file "$mov" 'mov'
  run $file "$push" 'push'
  run $file "$pop" 'pop'
  run $file "$nop" 'nop'
  run $file "$retq" 'retq'
  run $file "$add" 'add'
  run $file "$sub" 'sub'
  run $file "$imul" 'imul'
  run $file "$movssm" 'movssm'
  run $file "$movapsm" 'movapsm'
  run $file "$movhpsm" 'movhpsm'
  run $file "$movlpsm" 'movlpsm'
  run $file "$leam" 'leam'
  run $file "$movm" 'movm'

  run $file "$ic_papi" 'ic_papi'
  run $file "$ic_all" 'ic_all'

  run $file "$cmc1" 'cmc1' 
  run $file "$cmc2" 'cmc2' 

  run $file "$left" 'left' 
  run $file "$right" 'right' 
  run $file "$base" 'base' 


}

function report_single()
{
  file=$1
  data=$2
  fields="id, \
  rdtsc, \
  TOT_INS, \
  L1_DCM, \
  L2_DCM, \
  addss, \
  subss, \
  addps, \
  subps, \
  shufps, \
  unpckhps, \
  unpcklps, \
  movaps, \
  shl, \
  xor, \
  mov, \
  push, \
  pop, \
  nop, \
  retq, \
  add, \
  sub, \
  imul, \
  movssm, \
  movapsm, \
  movhpsm, \
  movlpsm, \
  leam, \
  movm, \
  ic_papi, \
  ic_all, \
  cmc1, \
  cmc2, \
  left, \
  right, \
  base"
  ${root}/extra/wht_query $file "$fields" > $data
}

function random_sample()
{
  a=$1
  b=$2
  n=$3
  file=$4
  random=$5

  # Generate JSON format of plans with ids
  echo '[' > $file
  for i in `seq $a $b`; do
    for ((j=1;j<=n;j+=1)); do
      echo '{' >> $file
      plan=`$random -n $i`
      echo "\"plan\" : \"$plan\"," >> $file
      echo "\"id\" : $i" >> $file
      echo '}' >> $file
      if [ $j -lt $n ]; then
        echo ',' >> $file
      fi
    done
    if [ $i -lt $b ]; then
      echo ',' >> $file
    fi
  done
  echo ']' >> $file
}

function find_best()
{
  i=$1
  o=$2
  $wht_strip < ${i} > ${o}
}

function wht_seq()
{
  i=$1
  o=$2
  cat ${i} | $wht_wrap "${wht_attach} -s" 'plan' > ${o}
}

function wht_vec1()
{
  i=$1
  o=$2
  V=$3
  K=$4
  cat ${i} | $wht_wrap "${wht_rotate} -p ${V}" 'plan' | $wht_wrap "${wht_vectorize} -v ${V} -k ${K} -c" 'plan' > ${o}
}

function wht_vec2()
{
  i=$1
  o=$2
  V=$3
  cat ${i} | $wht_wrap "${wht_vectorize_2} -v ${V}" 'plan' > ${o}
}

