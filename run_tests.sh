#!/bin/bash
# run_tests.sh - Run all test programs and verify results
#
# Usage: ./run_tests.sh [path_to_vm]

VM="${1:-./vm/vm}"
TESTS_DIR="tests"

# Check if VM exists
if [ ! -f "$VM" ]; then
    echo "Error: VM not found at $VM"
    echo "Usage: $0 [path_to_vm]"
    exit 1
fi

# Test list and expected results (compatible with bash 3.2)
TESTS="test_arithmetic test_stack test_comparison test_jump test_conditional test_loop test_memory test_function test_nested_calls factorial fibonacci"
EXPECTED_test_arithmetic=42
EXPECTED_test_stack=10
EXPECTED_test_comparison=1
EXPECTED_test_jump=200
EXPECTED_test_conditional=200
EXPECTED_test_loop=0
EXPECTED_test_memory=300
EXPECTED_test_function=20
EXPECTED_test_nested_calls=40
EXPECTED_factorial=120
EXPECTED_fibonacci=55

echo "========================================="
echo "  Running Test Suite"
echo "========================================="
echo ""

passed=0
failed=0

for test in $TESTS; do
    bc_file="$TESTS_DIR/$test.bc"

    if [ ! -f "$bc_file" ]; then
        echo "SKIP: $test.bc not found (run 'make tests' first)"
        continue
    fi

    # Get expected value for this test
    expected_var="EXPECTED_${test}"
    expected="${!expected_var}"

    output=$($VM "$bc_file" 2>&1)
    result=$(echo "$output" | grep "Result" | grep -oE '[0-9-]+$')

    if [ "$result" == "$expected" ]; then
        echo "PASS: $test (got $result)"
        ((passed++))
    else
        echo "FAIL: $test (expected $expected, got $result)"
        ((failed++))
    fi
done

echo ""
echo "========================================="
echo "  Results: $passed passed, $failed failed"
echo "========================================="

if [ $failed -gt 0 ]; then
    exit 1
fi
exit 0
