#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "nqueens_common.h"

int main(int argc, char** argv) {
    // Set the default value for the number of queens
    int n = 14;

    // Get the value of n from the cli
    if (argc >= 2) {
        n = atoi(argv[1]);
    }

    // Check if the number of queens was passed correctly
    if (n < 1 || n > 63) {
        fprintf(stderr, "N must be between 1 and 63\n");
        return 1;
    }

    // Start the timer
    clock_t start = clock();
    
    // Start from row 0 with no columns or diagonals
    ull solutions = solve_nqueens(n, 0, 0, 0, 0);

    clock_t end = clock();

    // Get the elapsed time of execution
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    fprintf(stdout, "Version: Sequential\n");
    fprintf(stdout, "N: %d\n", n);
    fprintf(stdout, "Solutions: %llu\n", solutions);
    fprintf(stdout, "Time: %.6f s\n", elapsed);

    return 0;
}