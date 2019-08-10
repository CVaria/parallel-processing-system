#!/usr/bin/env python

import sys
import numpy as np

## We need matplotlib:
## $ apt-get install python-matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


block_axis = range(16, 513, 16)

for size in [1024, 2048, 4096, 7168, 8192, 14336, 16384]:
	time_serial = []
	time_openMP = []
	time_naive = []
	time_coalesced = []
	time_cublas = []
	time_shmem = []

	performance_serial = []
	performance_openMP = []
	performance_naive = []
	performance_coalesced = []
	performance_cublas = []
	performance_shmem = []

	outFile = sys.argv[1]
	fp = open(outFile)
	line = fp.readline()
	while line:
		if (line.startswith("Matrix size: "+str(size))):
			line = fp.readline()
			while line:
				if (line.startswith("Serial version:")):
					time = float(fp.readline().split()[2])
					time_serial.append(time)
					performance = float(fp.readline().split()[1])
					performance_serial.append(performance)
					line  = fp.readline()
					break

				line = fp.readline()
			while line:
				if (line.startswith("OpenMP version:")):
					fp.readline() ##Checking
					time = float(fp.readline().split()[2])
					time_openMP.append(time)
					performance = float(fp.readline().split()[1])
					performance_openMP.append(performance)
					line  = fp.readline()
					break
				line = fp.readline()
			while line:
				if (line.startswith("GPU kernel version: naive")):
					fp.readline() ##Checking
					time = float(fp.readline().split()[2])
					time_naive.append(time)
					performance = float(fp.readline().split()[1])
					performance_naive.append(performance)
					line  = fp.readline()
					break
				elif (line.startswith("GPU kernel version: coalesced")):
					fp.readline() ##Checking
					time = float(fp.readline().split()[2])
					time_coalesced.append(time)
					performance = float(fp.readline().split()[1])
					performance_coalesced.append(performance)
					line  = fp.readline()
					break
				elif (line.startswith("GPU kernel version: shmem")):
					fp.readline() ##Checking
					time = float(fp.readline().split()[2])
					time_shmem.append(time)
					performance = float(fp.readline().split()[1])
					performance_shmem.append(performance)
					line  = fp.readline()
					break
				elif (line.startswith("GPU kernel version: CUBLAS")):
					fp.readline() ##Checking
					time = float(fp.readline().split()[2])
					time_cublas.append(time)
					performance = float(fp.readline().split()[1])
					performance_cublas.append(performance)
					line  = fp.readline()
					break
				line = fp.readline()
		line = fp.readline()

	fp.close()

	##Time figure
	fig, ax = plt.subplots()
	ax.grid(True)
	ax.set_xlabel("$Block\\ size$")

	xAx = np.arange(len(block_axis))
	ax.set_xlim(16, 512)
	m = min(time_serial + time_openMP + time_naive + time_coalesced + time_cublas + time_shmem)
	M = max(time_serial + time_openMP + time_naive + time_coalesced + time_cublas + time_shmem)
	ax.set_ylim(m - 0.05 * m, M + 0.05 * M)
	ax.set_ylabel("$Time (sec)$")
	avg = [sum(time_serial)/len(time_serial)] * len(block_axis)
	line1 = ax.plot(block_axis, avg, label="serial", color="red",marker='x')
	avg = [sum(time_openMP)/len(time_openMP)] * len(block_axis)
	line2 = ax.plot(block_axis, avg, label="openMP", color="green",marker=',')
	line3 = ax.plot(block_axis, time_naive, label="naive", color="blue",marker='*')
	line4 = ax.plot(block_axis, time_coalesced, label="coalesced", color="black",marker='.')
	line5 = ax.plot(block_axis, time_cublas, label="cublas", color="purple",marker='o')
	line6 = ax.plot(block_axis, time_shmem, label="shmem", color="chocolate",marker='+')

	lns = line1 + line2 + line3 + line4 + line5 + line6
	labs = [l.get_label() for l in lns]

	plt.title("$Time\\ "+str(size)+"\\times"+str(size)+"$")
	lgd = plt.legend(lns, labs)
	ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
	plt.savefig("time_"+str(size)+".png",bbox_inches="tight")

	##Performance figure
	fig, ax = plt.subplots()
	ax.grid(True)
	ax.set_xlabel("$Block\\ size$")

	xAx = np.arange(len(block_axis))
	ax.set_xlim(16, 512)
	m = min(performance_serial + performance_openMP + performance_naive + performance_coalesced + performance_cublas + performance_shmem)
	M = max(performance_serial + performance_openMP + performance_naive + performance_coalesced + performance_cublas + performance_shmem)
	ax.set_ylim(m - 0.05 * m, M + 0.05 * M)
	ax.set_ylabel("$Performance (Gflop/s)$")
	avg = [sum(performance_serial)/len(performance_serial)] * len(block_axis)
	line1 = ax.plot(block_axis, avg, label="serial", color="red",marker='x')
	avg = [sum(performance_openMP)/len(performance_openMP)] * len(block_axis)
	line2 = ax.plot(block_axis, avg, label="openMP", color="green",marker=',')
	line3 = ax.plot(block_axis, performance_naive, label="naive", color="blue",marker='*')
	line4 = ax.plot(block_axis, performance_coalesced, label="coalesced", color="black",marker='.')
	line5 = ax.plot(block_axis, performance_cublas, label="cublas", color="purple",marker='o')
	line6 = ax.plot(block_axis, performance_shmem, label="shmem", color="chocolate",marker='+')

	lns = line1 + line2 + line3 + line4 + line5 + line6
	labs = [l.get_label() for l in lns]

	plt.title("$Performance\\ "+str(size)+"\\times"+str(size)+"$")
	lgd = plt.legend(lns, labs)
	ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
	plt.savefig("performance_"+str(size)+".png",bbox_inches="tight")


