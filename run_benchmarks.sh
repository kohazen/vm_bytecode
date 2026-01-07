#!/bin/bash
# run_benchmarks.sh - Run benchmark programs and measure execution time
#
# Usage: ./run_benchmarks.sh [path_to_vm]

VM="${1:-./vm/vm}"
BENCH_DIR="benchmarks"

# Check if VM exists
if [ ! -f "$VM" ]; then
    echo "Error: VM not found at $VM"
    echo "Usage: $0 [path_to_vm]"
    exit 1
fi

# Benchmark list and expected results (compatible with bash 3.2)
BENCHMARKS="bench_arithmetic bench_loops bench_functions bench_memory"
EXPECTED_bench_arithmetic=1000
EXPECTED_bench_loops=10000
EXPECTED_bench_functions=2000
EXPECTED_bench_memory=1

echo "========================================="
echo "  Running Benchmarks"
echo "========================================="
echo ""

for bench in $BENCHMARKS; do
    bc_file="$BENCH_DIR/$bench.bc"

    if [ ! -f "$bc_file" ]; then
        echo "SKIP: $bench.bc not found (run 'make benchmarks' first)"
        continue
    fi

    echo "Running $bench..."

    # Get expected value for this benchmark
    expected_var="EXPECTED_${bench}"
    expected="${!expected_var}"

    # Measure execution time
    start_time=$(perl -MTime::HiRes=time -e 'print time')
    output=$($VM "$bc_file" 2>&1)
    end_time=$(perl -MTime::HiRes=time -e 'print time')

    # Calculate elapsed time
    elapsed=$(echo "$end_time - $start_time" | bc 2>/dev/null || echo "0.000")

    # Extract result
    result=$(echo "$output" | grep "Result" | grep -oE '[0-9-]+$')

    # Verify result
    if [ "$result" == "$expected" ]; then
        status="PASS"
    else
        status="FAIL (expected $expected, got $result)"
    fi

    printf "  %-25s %s  Time: %.3fs\n" "$bench" "$status" "$elapsed"
done

echo ""
echo "========================================="
echo "  Benchmarks Complete"
echo "========================================="
