#!/usr/bin/env python

import sys
import numpy as np

## We need matplotlib:
## $ apt-get install python-matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

nthreads = [1, 2, 6, 12, 24]

for size in [1024, 2048, 4096, 7168, 8192, 14336, 16384]:
	time_openMP = []
	performance_openMP = []

	outFile = sys.argv[1]
	fp = open(outFile)
	line = fp.readline()
	while line:
		if (line.startswith("Matrix size: "+str(size))):
			for i in nthreads:
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
			break
		line = fp.readline()

	fp.close()

	##Time figure
	fig, ax = plt.subplots()
	ax.grid(True)
	ax.set_xlabel("$Number\\ of\\ threads$")

	xAx = np.arange(len(nthreads))
	ax.set_xlim(0, 25)
	m = min(time_openMP)
	M = max(time_openMP)
	ax.set_ylim(m - 0.05 * m, M + 0.05 * M)
	ax.set_ylabel("$Time (sec)$")
	line2 = ax.plot(nthreads, time_openMP, label="openMP", color="midnightblue",marker='.')

	lns = line2
	labs = [l.get_label() for l in lns]

	plt.title("$Time\\ "+str(size)+"\\times"+str(size)+"$")
	# lgd = plt.legend(lns, labs)
	# ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
	plt.savefig("openmp_time_"+str(size)+".png",bbox_inches="tight")

	##Performance figure
	fig, ax = plt.subplots()
	ax.grid(True)
	ax.set_xlabel("$Number\\ of\\ threads$")

	xAx = np.arange(len(nthreads))
	ax.set_xlim(0, 25)
	m = min(performance_openMP)
	M = max(performance_openMP)
	ax.set_ylim(m - 0.05 * m, M + 0.05 * M)
	ax.set_ylabel("$Performance (Gflop/s)$")
	line2 = ax.plot(nthreads, performance_openMP, label="openMP", color="green",marker='.')

	lns = line2
	labs = [l.get_label() for l in lns]

	plt.title("$Performance\\ "+str(size)+"\\times"+str(size)+"$")
	# lgd = plt.legend(lns, labs)
	# ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
	plt.savefig("openmp_performance_"+str(size)+".png",bbox_inches="tight")


