#!/bin/bash

V=4 # Vector Size
K=8 # Interleave By

path=`dirname $0`
root="${path}/.."
data="${path}/../data"

prefix="${data}/xeonmp_single_dp"

source "${path}/xeonmp53xx.sh"
source "${path}/common.sh"
source "${path}/single_dp.sh"

