#!/usr/bin/env bash

EMAIL=ma53@drexel.edu

# Installed
PATH="$PATH:${root}/extra:${root}/bin" 
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

wht_dp=`which wht_dp`
wht_strip=`which wht_strip`
wht_wrap=`which wht_wrap`
wht_measure=`which wht_measure`
wht_cm_count=`which wht_cm_count`
wht_classify=`which wht_classify`
wht_rand=`which wht_rand`
wht_rotate=`which wht_rotate`
wht_vectorize=`which wht_vectorize`
wht_vectorize_2=`which wht_vectorize_2`
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
shufps="$wht_count_ins -i 'shufps'"
addsubss="$wht_count_ins -i 'addss|subss'"
addsubps="$wht_count_ins -i 'addps|subps'"
movssm="$wht_count_ins -i 'movss' -m"
movapsm="$wht_count_ins -i 'movaps' -m"
movhpsm="$wht_count_ins -i 'movhps' -m"
movlpsm="$wht_count_ins -i 'movlps' -m"
movssr="$wht_count_ins -i 'movss' -r"
movapsr="$wht_count_ins -i 'movaps' -r"

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
  run $file "$shufps" 'shufps' 
  run $file "$addsubss" 'addsubss' 
  run $file "$addsubps" 'addsubps' 
  run $file "$movssm" 'movssm' 
  run $file "$movapsm" 'movapsm' 
  run $file "$movhpsm" 'movhpsm' 
  run $file "$movlpsm" 'movlpsm' 
  run $file "$movssr" 'movssr' 
  run $file "$movapsr" 'movapsr' 
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
  shufps, \
  addsubss, \
  addsubps, \
  movssm, \
  movapsm, \
  movhpsm, \
  movlpsm, \
  movssr, \
  movapsr, \
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

