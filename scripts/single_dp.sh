#!/usr/bin/env bash

X_B=$(($X_A+1))

seq1_store="${prefix}_seq1_store.txt"
seq1_log="${prefix}_seq1_log.txt"
seq1_best="${prefix}_seq1_best.txt"
seq1_best_table="${prefix}_seq1_best_table.txt"

vec1_store="${prefix}_vec1_store.txt"
vec1_log="${prefix}_vec1_log.txt"
vec1_best="${prefix}_vec1_best.txt"
vec1_best_table="${prefix}_vec1_best_table.txt"

vec2_store="${prefix}_vec2_store.txt"
vec2_log="${prefix}_vec2_log.txt"
vec2_best="${prefix}_vec2_best.txt"
vec2_best_table="${prefix}_vec2_best_table.txt"

s_vec1_best="${prefix}_s_vec1_best.txt"
s_vec1_best_table="${prefix}_s_vec1_best_table.txt"
s_vec2_best="${prefix}_s_vec2_best.txt"
s_vec2_best_table="${prefix}_s_vec2_best_table.txt"

v1_seq1_best="${prefix}_v1_seq1_best.txt"
v1_seq1_best_table="${prefix}_v1_seq1_best_table.txt"
v2_seq1_best="${prefix}_v2_seq1_best.txt"
v2_seq1_best_table="${prefix}_v2_seq1_best_table.txt"

time (
  echo "Default L0" > ${seq1_log}
  $wht_dp -k 4 -p -m "${rdtsc} ${params_0}" -x 4 -a $L0_A -b $L0_B -f ${seq1_store} -d ${seq1_log}
  echo "Default L1.A" >> ${seq1_log}
  $wht_dp -k 4 -m "${rdtsc} ${params_1}" -x 3 -a $L1_A -b $X_A -f ${seq1_store} -d ${seq1_log}
  echo "Default L1.B" >> ${seq1_log}
  $wht_dp -k 3 -m "${rdtsc} ${params_1}" -x 3 -a $X_B -b $L1_B -f ${seq1_store} -d ${seq1_log}
  echo "Default L2" >> ${seq1_log}
  $wht_dp -k 2 -m "${rdtsc} ${params_2}" -x 2 -a $L2_A -b $L2_B -f ${seq1_store} -d ${seq1_log}
) 2>> ${seq1_log}

time (
  echo "Vectorize 1 L0" > ${vec1_log}
  $wht_dp -k 4 -p -m "${rdtsc} ${params_0}" -x 4 -a $L0_A -b $L0_B -f ${vec1_store} -d ${vec1_log} -r "${wht_vectorize}"
  echo "Vectorize 1 L1.A" >> ${vec1_log}
  $wht_dp -k 4 -m "${rdtsc} ${params_1}" -x 3 -a $L1_A -b $X_A -f ${vec1_store} -d ${vec1_log} -r "${wht_vectorize}"
  echo "Vectorize 1 L1.B" >> ${vec1_log}
  $wht_dp -k 3 -m "${rdtsc} ${params_1}" -x 3 -a $X_B -b $L1_B -f ${vec1_store} -d ${vec1_log} -r "${wht_vectorize}"
  echo "Vectorize 1 L2" >> ${vec1_log}
  $wht_dp -k 2 -m "${rdtsc} ${params_2}" -x 2 -a $L2_A -b $L2_B -f ${vec1_store} -d ${vec1_log} -r "${wht_vectorize}"
) 2>> ${vec1_log}

time (
  echo "Vectorize 2 L0" > ${vec2_log}
  $wht_dp -k 4 -p -m "${rdtsc} ${params_0}" -x 4 -a $L0_A -b $L0_B -f ${vec2_store} -d ${vec2_log} -r "${wht_vectorize_2}"
  echo "Vectorize 2 L1.A" >> ${vec2_log}
  $wht_dp -k 4 -m "${rdtsc} ${params_1}" -x 3 -a $L1_A -b $X_A -f ${vec2_store} -d ${vec2_log} -r "${wht_vectorize_2}"
  echo "Vectorize 2 L1.A" >> ${vec2_log}
  $wht_dp -k 3 -m "${rdtsc} ${params_1}" -x 3 -a $X_B -b $L1_B -f ${vec2_store} -d ${vec2_log} -r "${wht_vectorize_2}"
  echo "Vectorize 2 L2" >> ${vec2_log}
  $wht_dp -k 2 -m "${rdtsc} ${params_2}" -x 2 -a $L2_A -b $L2_B -f ${vec2_store} -d ${vec2_log} -r "${wht_vectorize_2}"
) 2>> ${vec2_log}

find_best ${seq1_store} ${seq1_best}
find_best ${vec1_store} ${vec1_best}
find_best ${vec2_store} ${vec2_best}

wht_seq ${vec1_best} ${s_vec1_best}
wht_seq ${vec2_best} ${s_vec2_best}

wht_vec1 ${seq1_best} ${v1_seq1_best}
wht_vec2 ${seq1_best} ${v2_seq1_best}

metrics_single $seq1_best 
metrics_single $vec1_best 
metrics_single $v1_seq1_best 
metrics_single $s_vec1_best 
metrics_single $vec2_best 
metrics_single $v2_seq1_best 
metrics_single $s_vec2_best 

report_single $seq1_best $seq1_best_table
report_single $vec1_best $vec1_best_table
report_single $s_vec1_best $s_vec1_best_table
report_single $v1_seq1_best $v1_seq1_best_table
report_single $vec2_best $vec2_best_table
report_single $s_vec2_best $s_vec2_best_table
report_single $v2_seq1_best $v2_seq1_best_table


