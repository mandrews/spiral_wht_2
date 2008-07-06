#!/bin/bash

V=4 # Vector Size
K=8 # Interleave By

path=`dirname $0`
root="${path}/.."
data="${path}/../data"

prefix="${data}/xeonmp_single_dp"

params_0=" -c -n 50 -k 50 -a 0.5 -p 0.1"
params_1=" -c -n 10 -k 10 -a 0.5 -p 0.1"
params_2=" -c -n 5  -k 5  -a 5   -p 1"

source "${path}/xeonmp53xx.sh"
source "${path}/common.sh"
source "${path}/single_dp.sh"

echo 'Done!' | nail -s "Single DP Finished!" $EMAIL
