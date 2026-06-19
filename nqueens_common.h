#ifndef NQUEENS_COMMON_H
#define NQUEENS_COMMON_H

#include <stdint.h>

typedef unsigned long long ull;

/**
 * Shared recursive N-Queens solver using bitmasks
 * 
 * columns: occupied columns
 * diag1: occupied left diagonals
 * diag2: occupied right diagonals
 * 
 * If the bit value is 1, then the position is used
 */

static ull solve_nqueens(
    int n,
    int row,
    uint64_t columns,
    uint64_t diag1,
    uint64_t diag2
) {
    if (row == n)
        return 1;

    ull count = 0;

    // Keep only the lowest n bits
    uint64_t mask = (1ULL << n) - 1;

    // Check for available positions on the columns and diagonals
    uint64_t available = mask & ~(columns | diag1 | diag2);

    while(available) {
        // Get the closest available position from the right side
        uint64_t bit = available & -available;

        // Remove the position from the available set
        available -= bit;

        /* 
        After placing the queen, move to the next row
        Diagonals are shifted because we move one column
        */
        count += solve_nqueens(n, row + 1, columns | bit, (diag1 | bit) << 1, (diag2 | bit) >> 1);
    }

    return count;
}

#endif
