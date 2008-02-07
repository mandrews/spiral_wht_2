#!/usr/bin/env bash

path=`dirname $0`
root="${path}/../data"

size=9
vector_size=2
interleave_by=8

# L1 = 12
# L2 = 19

wht_random="${path}/../bin/wht_rand -n $size -a 1 -b $size -p 1 -q 8"
wht_interleave="${path}/../extra/wht_interleave -k ${interleave_by}"
wht_vectorize="${path}/../extra/wht_vectorize -v ${vector_size} -k ${interleave_by}"

TOT_CYC="${path}/../bin/wht_measure -e PAPI -m TOT_CYC -n 10 -a 0.5 -p 0.1 -c -s"
TOT_INS="${path}/../bin/wht_measure -e PAPI -m TOT_INS -c -n 3"
FP_OPS="${path}/../bin/wht_measure -e PAPI -m FP_OPS -c -n 3"

scalar_add="${path}/../bin/count_sse.sh scalar_add"
scalar_mov="${path}/../bin/count_sse.sh scalar_mov"
vector_add="${path}/../bin/count_sse.sh vector_add"
vector_mov="${path}/../bin/count_sse.sh vector_mov"
shuffle="${path}/../bin/count_sse.sh shuffle"
unpack="${path}/../bin/count_sse.sh unpack"

default="${root}/default.txt"
interleave="${root}/interleave.txt"
vectorize="${root}/vectorize.txt"

${path}/wht_sample 10 "${wht_random}" > ${default}

cat ${default} | ${path}/wht_wrap "${wht_interleave}" 'plan' > ${interleave}
cat ${default} | ${path}/wht_wrap "${wht_vectorize}" 'plan' > ${vectorize}

function run() 
{
  file=$1
  func=$2
  fields=$3
  tmp=`mktemp`
  cat ${file} | ${path}/wht_wrap "${func}" "${fields}" > ${tmp}
  mv ${tmp} ${file} 
}

run $default "$TOT_CYC" 'TOT_CYC_mean, TOT_CYC_stdev, TOT_CYC_sample' 
run $default "$TOT_INS" 'TOT_INS' 
run $default "$FP_OPS" 'FP_OPS' 

run $interleave "$TOT_CYC" 'TOT_CYC_mean, TOT_CYC_stdev, TOT_CYC_sample' 
run $interleave "$TOT_INS" 'TOT_INS' 
run $interleave "$FP_OPS" 'FP_OPS' 

run $vectorize "$TOT_CYC" 'TOT_CYC_mean, TOT_CYC_stdev, TOT_CYC_sample' 
run $vectorize "$TOT_INS" 'TOT_INS' 
run $vectorize "$FP_OPS" 'FP_OPS' 
run $vectorize "$scalar_add" 'scalar_add' 
run $vectorize "$scalar_mov" 'scalar_mov' 
run $vectorize "$vector_add" 'vector_add' 
run $vectorize "$vector_mov" 'vector_mov' 
run $vectorize "$unpack" 'unpack' 
#run $vectorize "$shuffle" 'shuffle' 

