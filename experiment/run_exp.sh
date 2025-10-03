#!/bin/bash

# Parameters
INPUTS_DIR="/Users/burakerdem/graphs/is_perfect_test3"
OUTPUTS_DIR="outputs"
SUMMARY_FILE="$OUTPUTS_DIR/exp_summary2.csv"
RUN_IGRAPH="0" # When set to "1", igraph is_perfect is processed. Otherwise ("0"), igraph is_perfect is skipped

mkdir -p "$OUTPUTS_DIR"

for input_file in "$INPUTS_DIR"/*; do
    ./experiment/is_perfect_experiment "$input_file" "$SUMMARY_FILE" "$RUN_IGRAPH"
done