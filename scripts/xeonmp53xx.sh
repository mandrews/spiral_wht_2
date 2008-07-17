#!/bin/bash

# Note: on L2 cache size
# /proc/cpuinfo has each core having 4096 KB "cache"
# manual says 8 MB L2 Cache
# Architecture manuals say that these quad cores are more like 2 core 2 duo's
# glued together so both figures are essentially correct
# papi_meminfo is wrong

# This Xeon is built on Core Micro-Architecture

# All in bytes
L1_SIZE=$((32*1024)) 
L2_SIZE=$((8*1024*1024)) 
L1_BLOCK=64
L2_BLOCK=64
L1_ASSOC=8
L2_ASSOC=4

SINGLE_SIZE=4
DOUBLE_SIZE=8

# Reorder Buffer Size = 96
# 14 Stage Pipeline
# 3 ALU
