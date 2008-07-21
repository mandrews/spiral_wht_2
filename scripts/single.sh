#!/usr/bin/env bash

L1=`perl -e "print (log(${L1_SIZE} / ${SINGLE_SIZE}) / log(2)); print \"\n\""`
L2=`perl -e "print (log(${L2_SIZE} / ${SINGLE_SIZE}) / log(2)); print \"\n\""`

L0_A=1
L0_B=$(($L1 - 1))
L1_A=$L1
L1_B=$(($L2 - 1))
L2_A=$L2
L2_B=24

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
  run $file "$movhlps" 'movhlps'
  run $file "$movlhps" 'movlhps'
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

  run $file "$split_alpha" 'split_alpha'
  run $file "$split_beta_1" 'split_beta_1'
  run $file "$split_beta_2" 'split_beta_2'
  run $file "$split_beta_3" 'split_beta_3'
  run $file "$splitil_alpha" 'splitil_alpha'
  run $file "$splitil_beta_1" 'splitil_beta_1'
  run $file "$splitil_beta_2" 'splitil_beta_2'
  run $file "$splitil_beta_3" 'splitil_beta_3'
  run $file "$splitv_alpha" 'splitv_alpha'
  run $file "$splitv_beta_1" 'splitv_beta_1'
  run $file "$splitv_beta_2" 'splitv_beta_2'
  run $file "$splitv_beta_3" 'splitv_beta_3'
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
  movhlps, \
  movlhps, \
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
  base, \
  split_alpha, \
  split_beta_1, \
  split_beta_2, \
  split_beta_3,
  splitil_alpha, \
  splitil_beta_1, \
  splitil_beta_2, \
  splitil_beta_3"
  ${root}/extra/wht_query $file "$fields" > $data
}

