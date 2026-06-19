CC = gcc
MPICC = mpicc
CFLAGS = -O3 -Wall -Wextra
OPENMP_FLAGS = -fopenmp

all: seq openmp_for openmp_tasks mpi hybrid

seq: nqueens_seq.c nqueens_common.h
	$(CC) $(CFLAGS) nqueens_seq.c -o nqueens_seq

openmp_for: nqueens_openmp_for.c nqueens_common.h nqueens_states.h
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) nqueens_openmp_for.c -o nqueens_openmp_for

openmp_tasks: nqueens_openmp_tasks.c nqueens_common.h
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) nqueens_openmp_tasks.c -o nqueens_openmp_tasks

mpi: nqueens_mpi.c nqueens_common.h nqueens_states.h
	$(MPICC) $(CFLAGS) nqueens_mpi.c -o nqueens_mpi

hybrid: nqueens_hybrid.c nqueens_common.h nqueens_states.h
	$(MPICC) $(CFLAGS) $(OPENMP_FLAGS) nqueens_hybrid.c -o nqueens_hybrid

clean:
	rm -f nqueens_seq nqueens_openmp_for nqueens_openmp_tasks nqueens_mpi nqueens_hybrid results.csv