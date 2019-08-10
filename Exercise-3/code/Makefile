MPCC=mpicc
CFLAGS=-O3 -Wall
RES=-DPRINT_RESULTS
CONV=-DTEST_CONV

all: jacobi_mpi gaussSeidel_mpi redBlack_mpi

jacobi_mpi: jacobi_mpi.c utils.c utils.h
	$(MPCC) $(CFLAGS) $(RES) $(CONV) jacobi_mpi.c utils.c -lm -o jacobi_mpi

gaussSeidel_mpi: gaussSeidel_mpi.c utils.c utils.h
	$(MPCC) $(CFLAGS) $(RES) $(CONV) gaussSeidel_mpi.c utils.c -lm -o gaussSeidel_mpi

redBlack_mpi: redBlack_mpi.c utils.c utils.h
	$(MPCC) $(CFLAGS) $(RES) $(CONV) redBlack_mpi.c utils.c -lm -o redBlack_mpi

clean:
	rm jacobi_mpi gaussSeidel_mpi redBlack_mpi
