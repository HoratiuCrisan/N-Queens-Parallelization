#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "nqueens_common.h"
#include "nqueens_states.h"

/*
 * MPI implementation with partial-state decomposition.
 *
 * Every process generates the same list of partial states.
 * Then process rank r solves states:
 *
 * r, r + world_size, r + 2 * world_size, ...
 *
 * This avoids sending states between processes and keeps MPI simple.
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

    /*
     * All processes generate the same states independently.
     * For this project, this is simpler than broadcasting the list.
     */
    generate_states(n, 0, depth, 0, 0, 0, &states);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    ull local_solutions = 0;

    /*
     * Distribute partial states among MPI processes.
     */
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

    /*
     * Sum local solution counts into process 0.
     */
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
        printf("Version = MPI partial states\n");
        printf("N = %d\n", n);
        printf("Depth = %d\n", depth);
        printf("Partial states = %d\n", states.size);
        printf("Solutions = %llu\n", global_solutions);
        printf("Time = %.6f seconds\n", end - start);
        printf("MPI processes = %d\n", world_size);
    }

    free_state_list(&states);

    MPI_Finalize();

    return 0;
}