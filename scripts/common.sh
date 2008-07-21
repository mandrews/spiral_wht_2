#!/usr/bin/env bash

EMAIL=ma53@drexel.edu

# Installed
PATH="$PATH:${root}/extra:${root}/bin" 
# Development
PATH="$PATH:${root}/wht:${root}/measure:${root}/model/ic:${root}/model/cm:${root}/rand" 

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
wht_ic=`which wht_ic`

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
addsd="$wht_ic -o 'addsd' -t reg"
subsd="$wht_ic -o 'subsd' -t reg"
addpd="$wht_ic -o 'addpd' -t reg"
subpd="$wht_ic -o 'subpd' -t reg"
addss="$wht_ic -o 'addss' -t reg"
subss="$wht_ic -o 'subss' -t reg"
addps="$wht_ic -o 'addps' -t reg"
subps="$wht_ic -o 'subps' -t reg"
shufpd="$wht_ic -o 'shufpd' -t reg"
shufps="$wht_ic -o 'shufps' -t reg"
unpckhpd="$wht_ic -o 'unpckhpd' -t reg"
unpcklpd="$wht_ic -o 'unpcklpd' -t reg"
unpckhps="$wht_ic -o 'unpckhps' -t reg"
unpcklps="$wht_ic -o 'unpcklps' -t reg"
movapd="$wht_ic -o 'movapd' -t reg"
movaps="$wht_ic -o 'movaps' -t reg"
shl="$wht_ic -o 'shl' -t reg"
xor="$wht_ic -o 'xor' -t reg"
mov="$wht_ic -o 'mov' -t reg"
push="$wht_ic -o 'push' -t reg"
pop="$wht_ic -o 'pop' -t reg"
nop="$wht_ic -o 'nop' -t reg"
retq="$wht_ic -o 'retq' -t reg"
add="$wht_ic -o 'add' -t reg"
sub="$wht_ic -o 'sub' -t reg"
imul="$wht_ic -o 'imul' -t reg"
movhlps="$wht_ic -o 'movhlps' -t reg"
movlhps="$wht_ic -o 'movlhps' -t reg"
# mem
movsdm="$wht_ic -o 'movsd' -t mem"
movapdm="$wht_ic -o 'movapd' -t mem"
movssm="$wht_ic -o 'movss' -t mem"
movapsm="$wht_ic -o 'movaps' -t mem"
movhpdm="$wht_ic -o 'movhpd' -t mem"
movlpdm="$wht_ic -o 'movlpd' -t mem"
movhpsm="$wht_ic -o 'movhps' -t mem"
movlpsm="$wht_ic -o 'movlps' -t mem"
leam="$wht_ic -o 'lea' -t mem"
movm="$wht_ic -o 'mov' -t mem"
# special
ic_papi="$wht_ic -o 'papi'"
ic_all="$wht_ic -o 'all'"

split_alpha="$wht_ic -o 'split_alpha'"
split_beta_1="$wht_ic -o 'split_beta_1'"
split_beta_2="$wht_ic -o 'split_beta_2'"
split_beta_3="$wht_ic -o 'split_beta_3'"
splitil_alpha="$wht_ic -o 'splitil_alpha'"
splitil_beta_1="$wht_ic -o 'splitil_beta_1'"
splitil_beta_2="$wht_ic -o 'splitil_beta_2'"
splitil_beta_3="$wht_ic -o 'splitil_beta_3'"
splitv_alpha="$wht_ic -o 'splitv_alpha'"
splitv_beta_1="$wht_ic -o 'splitv_beta_1'"
splitv_beta_2="$wht_ic -o 'splitv_beta_2'"
splitv_beta_3="$wht_ic -o 'splitv_beta_3'"

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

