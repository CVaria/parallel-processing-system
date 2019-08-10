#!/bin/bash

## Give the Job a descriptive name
#PBS -N tiled_openmp

## Output and error files
#PBS -o tiled_openmp.out
#PBS -e tiled_openmp.err

## Limit memory, runtime etc.
#PBS -l walltime=00:20:00
#PBS -l pmem=100mb

## How many nodes:processors_per_node should we get?
#PBS -l nodes=sandman:ppn=64

## Start
#echo "PBS_NODEFILE = $PBS_NODEFILE"
#cat $PBS_NODEFILE

## Run the job (use full paths to make sure we execute the correct thing)
## Set directory of files
DIR=
module load openmp

for thr in 1 2 4 8 16 32 64
do
	export OMP_NUM_THREADS=$thr
	export GOMP_CPU_AFFINITY=0-$thr
	echo "NUM_THREADS=$thr"
	for N in 1024 2048 4096
	do
		DIR/fw_tiled1 $N 128
	done
done

module unload openmp
