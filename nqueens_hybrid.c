#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#include "nqueens_common.h"
#include "nqueens_states.h"

/*
 * Hybrid MPI + OpenMP implementation.
 *
 * MPI distributes partial states across processes.
 * OpenMP distributes each process's assigned states across threads.
 */
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int n = 14;
    int depth = 3;

    if (argc >= 2) {
        n = atoi(argv[1]);
    }

    if (argc >= 3) {
        depth = atoi(argv[2]);
    }

    if (n < 1 || n > 63) {
        if (rank == 0) {
            fprintf(stderr, "N must be between 1 and 63\n");
        }

        MPI_Finalize();
        return 1;
    }

    if (depth < 1 || depth > n) {
        if (rank == 0) {
            fprintf(stderr, "Depth must be between 1 and N\n");
        }

        MPI_Finalize();
        return 1;
    }

    StateList states;
    init_state_list(&states);

    generate_states(n, 0, depth, 0, 0, 0, &states);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    ull local_solutions = 0;

    /*
     * MPI decides which states belong to this process.
     * OpenMP parallelizes the loop inside each process.
     */
    #pragma omp parallel for reduction(+:local_solutions) schedule(dynamic)
    for (int i = rank; i < states.size; i += world_size) {
        State s = states.data[i];

        local_solutions += solve_nqueens(
            n,
            s.row,
            s.columns,
            s.diag1,
            s.diag2
        );
    }

    ull global_solutions = 0;

    MPI_Reduce(
        &local_solutions,
        &global_solutions,
        1,
        MPI_UNSIGNED_LONG_LONG,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("Version = Hybrid MPI + OpenMP partial states\n");
        printf("N = %d\n", n);
        printf("Depth = %d\n", depth);
        printf("Partial states = %d\n", states.size);
        printf("Solutions = %llu\n", global_solutions);
        printf("Time = %.6f seconds\n", end - start);
        printf("MPI processes = %d\n", world_size);
        printf("OpenMP threads per process = %d\n", omp_get_max_threads());
    }

    free_state_list(&states);

    MPI_Finalize();

    return 0;
}