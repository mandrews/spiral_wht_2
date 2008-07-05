#!/usr/bin/env bash

EMAIL=ma53@drexel.edu

# Installed
PATH="$PATH:${root}/bin:${root}/extra" 
# Development
PATH="$PATH:${root}/measure:${root}/model/ic:${root}/model/cm:${root}/rand" 

L1=`perl -e "print (log(${L1_SIZE} / ${SINGLE_SIZE}) / log(2)); print \"\n\""`
L2=`perl -e "print (log(${L2_SIZE} / ${SINGLE_SIZE}) / log(2)); print \"\n\""`

L0_A=1
L0_B=$(($L1 - 1))
L1_A=$L1
L1_B=$(($L2 - 1))
L2_A=$L2
L2_B=24

params_0=" -c -n 50 -k 50 -a 0.5 -p 0.1"
params_1=" -c -n 10 -k 10 -a 0.5 -p 0.1"
params_2=" -c -n 5  -k 5  -a 5   -p 1"

wht_dp=`which wht_dp`
wht_strip=`which wht_strip`
wht_wrap=`which wht_wrap`
wht_measure=`which wht_measure`
wht_cm_count=`which wht_cm_count`
wht_classify=`which wht_classify`
wht_count_ins=`which count_sse.sh`

rdtsc="$wht_measure -m rdtsc"

TOT_INS="$wht_measure -m PAPI_TOT_INS"
L1_DCM="$wht_measure -m PAPI_L1_DCM"
L2_DCM="$wht_measure -m PAPI_L2_DCM"

cmc1="$wht_cm_count -c ${L1_SIZE} -b ${L2_BLOCK} -a ${L2_ASSOC}"
cmc2="$wht_cm_count -c ${L2_SIZE} -b ${L2_BLOCK} -a ${L2_ASSOC}"

left="$wht_classify | grep left_tree | sed -e 's/.*:\s*//g'"
right="$wht_classify | grep right_tree | sed -e 's/.*:\s*//g'"
base="$wht_classify | grep right_node | sed -e 's/.*:\s*//g'"

unpckhps="$wht_count_ins -i 'unpckhps'"
unpcklps="$wht_count_ins -i 'unpcklps'"
addsubss="$wht_count_ins -i 'addss|subss'"
addsubps="$wht_count_ins -i 'addps|subps'"
movssm="$wht_count_ins -i 'movss' -m"
movapsm="$wht_count_ins -i 'movaps' -m"
movhpsm="$wht_count_ins -i 'movhps' -m"
movlpsm="$wht_count_ins -i 'movlps' -m"
movssr="$wht_count_ins -i 'movss' -r"
movapsr="$wht_count_ins -i 'movaps' -r"
movhpsr="$wht_count_ins -i 'movhps' -r"
movlpsr="$wht_count_ins -i 'movlps' -r"

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

  run $file "$unpckhps" 'unpckhps' 
  run $file "$unpcklps" 'unpcklps' 
  run $file "$addsubss" 'addsubss' 
  run $file "$addsubps" 'addsubps' 
  run $file "$movssm" 'movssm' 
  run $file "$movapsm" 'movapsm' 
  run $file "$movhpsm" 'movhpsm' 
  run $file "$movlpsm" 'movlpsm' 
  run $file "$movssr" 'movssr' 
  run $file "$movapsr" 'movapsr' 
  run $file "$movhpsr" 'movhpsr' 
  run $file "$movlpsr" 'movlpsr' 
  run $file "$cmc1" 'cmc1' 
  run $file "$cmc1" 'cmc2' 
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
  unpckhps, \
  unpcklps, \
  addsubss, \
  addsubps, \
  movssm, \
  movapsm, \
  movhpsm, \
  movlpsm, \
  movssr, \
  movapsr, \
  movhpsr, \
  movlpsr, \
  cmc1, \
  cmc1, \
  left, \
  right, \
  base"
  ${root}/extra/wht_query $file "$fields" > $data
}

