#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "nqueens_common.h"
#include "nqueens_states.h"

/**
 * OpenMP parallel-for using partial state decomposition
 * 
 * Instead of spliting only the first row, we generate partial boards up to a chosen depth, 
 * then distribute the states across OpenMP threads
 */

int main(int argc, char** argv) {
    // Set default number of queens and depth
    int n = 14;
    int depth = 3;

    if (argc >= 2) {
        n = atoi(argv[1]);
    }

    if (argc >= 3) {
        depth = atoi(argv[2]);
    }

    if (n < 1 || n > 63) {
        fprintf(stderr, "N must be between 1 and 63\n");
        return 1;
    }

    if (depth < 1 || depth > n) {
        fprintf(stderr, "Depth must be between 1 and N\n");
        return 1;
    }

    StateList states;
    init_state_list(&states);

    // Generate partial boards before solving
    generate_states(n, 0, depth, 0, 0, 0, &states);

    ull total_solutions = 0;

    // Start timer
    double start = omp_get_wtime();

    /**
     * Each thread receives several partial states
     */

    #pragma omp parallel for reduction(+:total_solutions) schedule(dynamic)
    for (int i = 0; i < states.size; i++) {
        State s = states.data[i];

        total_solutions += solve_nqueens(n, s.row, s.columns, s.diag1, s.diag2);
    }

    // End timer
    double end = omp_get_wtime();

    fprintf(stdout, "Version: OpenMp partial states\n");
    fprintf(stdout, "N: %d\n", n);
    fprintf(stdout, "Depth: %d\n", depth);
    fprintf(stdout, "Partial states: %d\n", states.size);
    fprintf(stdout, "Solutions: %llu\n", total_solutions);
    fprintf(stdout, "Time: %.6f seconds\n", end - start);
    fprintf(stdout, "Threads: %d\n", omp_get_max_threads());

    free_state_list(&states);

    return 0;
}