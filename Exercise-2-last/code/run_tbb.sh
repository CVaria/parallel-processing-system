#!/bin/bash

## Give the Job a descriptive name
#PBS -N fw_tiled_alloc

## Output and error files
#PBS -o tiled_tbb.out
#PBS -e tiled_tbb.err

## Limit memory, runtime etc.
#PBS -l walltime=00:20:00
#PBS -l pmem=100mb

## How many nodes:processors_per_node should we get?
#PBS -l nodes=sandman:ppn=64

## Start
#echo "PBS_NODEFILE = $PBS_NODEFILE"
#cat $PBS_NODEFILE

module load tbbz

## Run the job (use full paths to make sure we execute the correct thing)
## Set directory of files
DIR=

export LD_PRELOAD=libtbbmalloc_proxy.so.2

for thr in 1 2 4 8 16 32 64
do
	for N in 1024 2048 4096
	do
		DIR/fw_tiled_for_auto $N 128 $thr 1 1
	done
done

module unload tbbz
