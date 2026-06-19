#!/bin/bash

# Usage:
#   ./run_benchmarks.sh
#   ./run_benchmarks.sh 12 13 14 15 16 17

if [ "$#" -eq 0 ]; then
    NS="12 13 14 15 16 17 18"
else
    NS="$@"
fi

# Increase to 4 if you want to experiment with deeper partial-state generation.
DEPTH=3

# Number of repeated benchmark runs.
NUM_RUNS=5

extract_time() {
    echo "$1" | grep "^Time" | grep -oE '[0-9]+(\.[0-9]+)?'
}

extract_solutions() {
    echo "$1" | grep "^Solutions" | grep -oE '[0-9]+'
}

for run in $(seq 1 $NUM_RUNS)
do
    OUTPUT_FILE="results_run${run}.csv"

    echo "Starting benchmark run $run"
    echo "Saving results to $OUTPUT_FILE"

    echo "version,n,workers,depth,time,solutions" > "$OUTPUT_FILE"

    for n in $NS
    do
        echo "Run $run - Sequential N=$n"
        output=$(./nqueens_seq "$n")
        time=$(extract_time "$output")
        solutions=$(extract_solutions "$output")
        echo "sequential,$n,1,0,$time,$solutions" >> "$OUTPUT_FILE"

        # OpenMP
        for threads in 2 4 8 12 16 24
        do
            echo "Run $run - OpenMP partial N=$n threads=$threads"
            output=$(OMP_NUM_THREADS=$threads ./nqueens_openmp_for "$n" "$DEPTH")
            time=$(extract_time "$output")
            solutions=$(extract_solutions "$output")
            echo "openmp_partial,$n,$threads,$DEPTH,$time,$solutions" >> "$OUTPUT_FILE"

            echo "Run $run - OpenMP tasks N=$n threads=$threads"
            output=$(OMP_NUM_THREADS=$threads ./nqueens_openmp_tasks "$n" "$DEPTH")
            time=$(extract_time "$output")
            solutions=$(extract_solutions "$output")
            echo "openmp_tasks,$n,$threads,$DEPTH,$time,$solutions" >> "$OUTPUT_FILE"
        done

        # MPI
        for procs in 2 4 8 12 16 24
        do
            echo "Run $run - MPI N=$n processes=$procs"
            output=$(mpirun -np $procs ./nqueens_mpi "$n" "$DEPTH")
            time=$(extract_time "$output")
            solutions=$(extract_solutions "$output")
            echo "mpi_partial,$n,$procs,$DEPTH,$time,$solutions" >> "$OUTPUT_FILE"
        done

        # Hybrid (24 total workers in different layouts)
        for config in "2 12" "4 6" "6 4" "8 3"
        do
            procs=$(echo "$config" | awk '{print $1}')
            threads=$(echo "$config" | awk '{print $2}')
            workers=$((procs * threads))

            echo "Run $run - Hybrid N=$n MPI=$procs OMP=$threads"

            output=$(OMP_NUM_THREADS=$threads \
                mpirun -np $procs ./nqueens_hybrid "$n" "$DEPTH")

            time=$(extract_time "$output")
            solutions=$(extract_solutions "$output")

            echo "hybrid_partial,$n,$workers,$DEPTH,$time,$solutions" >> "$OUTPUT_FILE"
        done
    done

    echo "Finished benchmark run $run"
done

echo
echo "All benchmark runs completed."
echo "Generated:"
echo "  results_run1.csv"
echo "  results_run2.csv"
echo "  results_run3.csv"
echo "  results_run4.csv"
echo "  results_run5.csv"