#ifndef NQUEENS_STATES_H
#define NQUEENS_STATES_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Partial board state
 * 
 * Instead of starting the solver from row 0 every time, generate partial boards up to a fixed depth
 */
typedef struct {
    int row;
    uint64_t columns;
    uint64_t diag1;
    uint64_t diag2;
} State;

// Dynamic array of states
typedef struct {
    State *data;
    int size;
    int capacity;
} StateList;

// Initialize dynamic state list
static void init_state_list(StateList *list) {
    list->size = 0;
    list->capacity = 1024;
    list->data = (State *)malloc(list->capacity * sizeof(State));

    if (list->data == NULL) {
        fprintf(stderr, "Failed to allocate state list\n");
        exit(1);
    }
}

// Add a state to the dynamic list
static void add_state(StateList *list, State state) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;

        State *new_data = (State *)realloc(
            list->data,
            list->capacity * sizeof(State)
        );

        if (new_data == NULL) {
            fprintf(stderr, "Failed to reallocate state list\n");
            free(list->data);
            exit(1);
        }

        list->data = new_data;
    }

    list->data[list->size] = state;
    list->size++;
}

// Free memory used by the state list
static void free_state_list(StateList *list) {
    free(list->data);
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

/**
 * Generate valid partial N-Queens states up to max depth in order to create many smaller independent tasks
 */
static void generate_states(int n, int row, int max_depth, uint64_t columns, uint64_t diag1, uint64_t diag2, StateList *list) {
    if (row == max_depth) {
        State state;
        state.row = row;
        state.columns = columns;
        state.diag1 = diag1;
        state.diag2 = diag2;

        add_state(list, state);
        return;
    }

    uint64_t mask = (1ULL << n) - 1;
    uint64_t available = mask & ~(columns | diag1 | diag2);

    while (available) {
        uint64_t bit = available & -available;
        available -= bit;

        generate_states(n, row + 1, max_depth, columns | bit, (diag1 | bit) << 1, (diag2 | bit) >> 1, list);
    }
}

#endif