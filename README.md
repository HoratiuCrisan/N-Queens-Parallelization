# N-Queens-Parallelization
This paper presents a parallel implementation of the N-Queens problem in C, using OpenMP, MPI, and
Hybrid MPI+OpenMP. The solver uses recursive backtracking with bitmask-based conflict checking and partialstate decomposition to improve workload distribution. The implementation was evaluated on a system with an
Intel Core Ultra i7 270K CPU with 24 hardware threads and 32 GB RAM. Experiments were performed for
N = 12 to N = 18, with each configuration executed five times. Results show strong speedup for larger board
sizes, especially for OpenMP and MPI versions.
