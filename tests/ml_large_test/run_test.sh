#!/bin/bash

TRACE_FILE="../../data/custom_traces/cache_trace_4.txt"
BLOCK_SIZE=64

L1_SIZE=1048576
L1_ASSOCS=(1 2 4 8)
L2_SIZE=2097152
L2_ASSOCS=(1 4 8 16)

QUEUE_SIZES=(2 4 8 16 32 64 128 256 512 1024)

# Ensure results directory exists
mkdir -p results

# CSV output file
CSV_FILE="summary.csv"
echo "Prefetcher_Queue_Size,L1_Associativity,L2_Associativity,Prefetcher_Updates,L1_Unique,L1_Shared,L2_Unqiue,L2_Shared" > "$CSV_FILE"

# Run simulations
for QUEUE in "${QUEUE_SIZES[@]}"
do
  for i in "${!L1_ASSOCS[@]}"
  do
    L1A="${L1_ASSOCS[$i]}"
    L2A="${L2_ASSOCS[$i]}"
    OUTPUT_FILE="results/queue_${QUEUE}_l1assoc_${L1A}.txt"
    echo "Running with prefetcher queue size=$QUEUE and l1, l2 assoc=$L1A, $L2A..."
    ../../src/sim_cache "$BLOCK_SIZE" "$L1_SIZE" "$L1A" "$L2_SIZE" "$L2A" 0 0 "$TRACE_FILE" "$QUEUE" > "$OUTPUT_FILE"

    # Extract values
    PREFETCHER_UPDATES=$(grep "a. number of prefetcher update calls:" "$OUTPUT_FILE" | awk '{print $7}')
    L1_UNIQUE=$(grep "b. unique correct predictions on L1:" "$OUTPUT_FILE" | awk '{print $7}')
    L1_SHARED=$(grep "c. non-unique correct predictions on L1:" "$OUTPUT_FILE" | awk '{print $7}')
    L2_UNIQUE=$(grep "d. unique correct predictions on L2:" "$OUTPUT_FILE" | awk '{print $7}')
    L2_SHARED=$(grep "e. non-unique correct predictions on L2:" "$OUTPUT_FILE" | awk '{print $7}')

    # Append to CSV
    echo "$QUEUE,$L1A,$L2A,$PREFETCHER_UPDATES,$L1_UNIQUE,$L1_SHARED,$L2_UNIQUE,$L2_SHARED" >> "$CSV_FILE"
  done
done

echo "All runs completed and summary saved to $CSV_FILE"
