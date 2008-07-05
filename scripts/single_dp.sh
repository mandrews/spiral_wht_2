#!/usr/bin/env bash

wht_vec1="${root}/extra/wht_vectorize -v ${V} -k ${K} -c"
wht_vec2="${root}/extra/wht_vectorize_2 -v ${V}"

wht_seq="${root}/bin/wht_attach -s"

seq_store="${prefix}_seq_store.txt"
seq_log="${prefix}_seq_log.txt"
seq_best="${prefix}_seq_best.txt"
seq_best_table="${prefix}_seq_best_table.txt"

vec1_store="${prefix}_vec1_store.txt"
vec1_log="${prefix}_vec1_log.txt"
vec1_best="${prefix}_vec1_best.txt"
vec1_best_table="${prefix}_vec1_best_table.txt"

s_vec1_best="${prefix}_s_vec1_best.txt"
s_vec1_best_table="${prefix}_s_vec1_best_table.txt"

vec2_store="${prefix}_vec2_store.txt"
vec2_log="${prefix}_vec2_log.txt"
vec2_best="${prefix}_vec2_best.txt"
vec2_best_table="${prefix}_vec2_best_table.txt"

s_vec2_best="${prefix}_s_vec2_best.txt"
s_vec2_best_table="${prefix}_s_vec2_best_table.txt"

v1_seq_best="${prefix}_v1_seq_best.txt"
v1_seq_best_table="${prefix}_v1_seq_best_table.txt"

v2_seq_best="${prefix}_v2_seq_best.txt"
v2_seq_best_table="${prefix}_v2_seq_best_table.txt"

time (
echo "Default L0" > ${seq_log}
$wht_dp -k 4 -p -m "${rdtsc} ${params_0}" -x 4 -a $L0_A -b $L0_B -f ${seq_store} -d ${seq_log}
echo "Default L1" >> ${seq_log}
$wht_dp -k 4 -m "${rdtsc} ${params_1}" -x 3 -a $L1_A -b $L1_B -f ${seq_store} -d ${seq_log}
echo "Default L2" >> ${seq_log}
$wht_dp -k 2 -m "${rdtsc} ${params_2}" -x 2 -a $L2_A -b $L2_B -f ${seq_store} -d ${seq_log}
) 2>> ${seq_log}

time (
echo "Vectorize 1 L0" > ${vec1_log}
$wht_dp -k 4 -p -m "${rdtsc} ${params_0}" -x 4 -a $L0_A -b $L0_B -f ${vec1_store} -d ${vec1_log} -r "${wht_vec1}"
echo "Vectorize 1 L1" >> ${vec1_log}
$wht_dp -k 4 -m "${rdtsc} ${params_1}" -x 3 -a $L1_A -b $L1_B -f ${vec1_store} -d ${vec1_log} -r "${wht_vec1}"
echo "Vectorize 1 L2" >> ${vec1_log}
$wht_dp -k 2 -m "${rdtsc} ${params_2}" -x 2 -a $L2_A -b $L2_B -f ${vec1_store} -d ${vec1_log} -r "${wht_vec1}"
) 2>> ${vec1_log}

time (
echo "Vectorize 2 L0" > ${vec2_log}
$wht_dp -k 4 -p -m "${rdtsc} ${params_0}" -x 4 -a $L0_A -b $L0_B -f ${vec2_store} -d ${vec2_log} -r "${wht_vec2}"
echo "Vectorize 2 L1" >> ${vec2_log}
$wht_dp -k 4 -m "${rdtsc} ${params_1}" -x 3 -a $L1_A -b $L1_B -f ${vec2_store} -d ${vec2_log} -r "${wht_vec2}"
echo "Vectorize 2 L2" >> ${vec2_log}
$wht_dp -k 2 -m "${rdtsc} ${params_2}" -x 2 -a $L2_A -b $L2_B -f ${vec2_store} -d ${vec2_log} -r "${wht_vec2}"
) 2>> ${vec2_log}

$wht_strip < ${seq_store} > ${seq_best}
$wht_strip < ${seq_store} | $wht_wrap "${wht_vec1}" 'plan' > ${v1_seq_best}
$wht_strip < ${seq_store} | $wht_wrap "${wht_vec2}" 'plan' > ${v2_seq_best}
$wht_strip < ${vec1_store} > ${vec1_best}
$wht_strip < ${vec2_store} > ${vec2_best}
$wht_strip < ${vec1_store} | $wht_wrap "${wht_seq}" 'plan' > ${s_vec1_best}
$wht_strip < ${vec2_store} | $wht_wrap "${wht_seq}" 'plan' > ${s_vec2_best}

metrics_single $seq_best 
metrics_single $vec1_best 
metrics_single $v1_seq_best 
metrics_single $s_vec1_best 
metrics_single $vec2_best 
metrics_single $v2_seq_best 
metrics_single $s_vec2_best 

report_single $seq_best $seq_best_table
report_single $vec1_best $vec1_best_table
report_single $s_vec1_best $s_vec1_best_table
report_single $v1_seq_best $v1_seq_best_table
report_single $vec2_best $vec2_best_table
report_single $s_vec2_best $s_vec2_best_table
report_single $v2_seq_best $v2_seq_best_table

echo 'Done!' | nail -s 'Single DP Finished!' $EMAIL

