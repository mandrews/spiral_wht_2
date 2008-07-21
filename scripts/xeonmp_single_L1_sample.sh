#!/bin/bash

V=4   # Vector Size
K=8   # Interleave By
N=500 # Sample Size

path=`dirname $0`
root="${path}/.."
data="${path}/../data"

prefix="${data}/xeonmp_double_L1_sample_${N}"

params_0=" -c -n 50 -k 50 -t 0.01"
params_1=" -c -n 10 -k 10 -t 0.01"
params_2=" -c -n 5  -k 5  -t 0.01"

source "${path}/xeonmp53xx.sh"
source "${path}/common.sh"
source "${path}/single.sh"

seq1="${prefix}_seq1.txt"
seq2="${prefix}_seq2.txt"
seq3="${prefix}_seq3.txt"
vec1="${prefix}_vec1.txt"
vec2="${prefix}_vec2.txt"

random_sample $L1_A $L1_B $N "$seq1" "$wht_rand"
random_sample $L1_A $L1_B $N "$seq2" "$wht_rand -v ${V}"

$wht_ic -o 'mov' -w 'small[1]' -X # Refresh Cache

wht_vec1 $seq1 $vec1 $V $K
wht_vec2 $seq2 $vec2 $V 
wht_seq $vec1 $seq3

metrics_single $seq1
metrics_single $seq2
metrics_single $seq3
metrics_single $vec1
metrics_single $vec2

seq1_table="${prefix}_seq1_table.txt"
seq2_table="${prefix}_seq2_table.txt"
seq3_table="${prefix}_seq3_table.txt"
vec1_table="${prefix}_vec1_table.txt"
vec2_table="${prefix}_vec2_table.txt"

report_single $seq1 $seq1_table
report_single $seq2 $seq2_table
report_single $seq3 $seq3_table
report_single $vec1 $vec1_table
report_single $vec2 $vec2_table

echo 'Done!' | nail -s "XeonMP Sample ${N} Single Finished!" $EMAIL
