#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

#include "nqueens_common.h"

/*
 * OpenMP recursive task implementation.
 *
 * This version is kept because it gives you another parallel strategy
 * to discuss: task-based recursive decomposition.
 */
static ull solve_tasks(
    int n,
    int row,
    uint64_t columns,
    uint64_t diag1,
    uint64_t diag2,
    int cutoff_depth
) {
    if (row == n) {
        return 1;
    }

    ull count = 0;
    uint64_t mask = (1ULL << n) - 1;
    uint64_t available = mask & ~(columns | diag1 | diag2);

    while (available) {
        uint64_t bit = available & -available;
        available -= bit;

        if (row < cutoff_depth) {
            /*
             * Create a task only near the top of the search tree.
             * Creating tasks too deep causes too much overhead.
             */
            #pragma omp task shared(count) firstprivate(n, row, columns, diag1, diag2, bit, cutoff_depth)
            {
                ull local_count = solve_tasks(
                    n,
                    row + 1,
                    columns | bit,
                    (diag1 | bit) << 1,
                    (diag2 | bit) >> 1,
                    cutoff_depth
                );

                /*
                 * Multiple tasks update count, so atomic prevents races.
                 */
                #pragma omp atomic
                count += local_count;
            }
        } else {
            count += solve_nqueens(
                n,
                row + 1,
                columns | bit,
                (diag1 | bit) << 1,
                (diag2 | bit) >> 1
            );
        }
    }

    #pragma omp taskwait

    return count;
}

int main(int argc, char *argv[]) {
    int n = 14;
    int cutoff_depth = 3;

    if (argc >= 2) {
        n = atoi(argv[1]);
    }

    if (argc >= 3) {
        cutoff_depth = atoi(argv[2]);
    }

    if (n < 1 || n > 63) {
        fprintf(stderr, "N must be between 1 and 63\n");
        return 1;
    }

    if (cutoff_depth < 1 || cutoff_depth > n) {
        fprintf(stderr, "Cutoff depth must be between 1 and N\n");
        return 1;
    }

    ull total_solutions = 0;

    double start = omp_get_wtime();

    /*
     * One thread starts recursion.
     * Other threads execute generated tasks.
     */
    #pragma omp parallel
    {
        #pragma omp single
        {
            total_solutions = solve_tasks(n, 0, 0, 0, 0, cutoff_depth);
        }
    }

    double end = omp_get_wtime();

    printf("Version = OpenMP tasks\n");
    printf("N = %d\n", n);
    printf("Cutoff depth = %d\n", cutoff_depth);
    printf("Solutions = %llu\n", total_solutions);
    printf("Time = %.6f seconds\n", end - start);
    printf("Threads = %d\n", omp_get_max_threads());

    return 0;
}