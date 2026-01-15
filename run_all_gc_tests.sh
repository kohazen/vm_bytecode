#!/bin/bash

# Run all GC test cases
# Student B - Day 4

echo "========================================="
echo "  Running Complete GC Test Suite"
echo "========================================="
echo ""

# Test 1: Basic allocation
echo "Running Test: Basic Allocation..."
./tests/gc_test_basic
if [ $? -eq 0 ]; then
    echo "✓ Basic Allocation PASSED"
else
    echo "✗ Basic Allocation FAILED"
    exit 1
fi
echo ""

# Test 2: Root discovery
echo "Running Test: Root Discovery..."
./tests/gc_test_reachability
if [ $? -eq 0 ]; then
    echo "✓ Root Discovery PASSED"
else
    echo "✗ Root Discovery FAILED"
    exit 1
fi
echo ""

# Test 3: Transitive reachability
echo "Running Test: Transitive Reachability..."
./tests/gc_test_transitive
if [ $? -eq 0 ]; then
    echo "✓ Transitive Reachability PASSED"
else
    echo "✗ Transitive Reachability FAILED"
    exit 1
fi
echo ""

# Test 4: Sweep phase
echo "Running Test: Sweep Phase..."
./tests/gc_test_sweep
if [ $? -eq 0 ]; then
    echo "✓ Sweep Phase PASSED"
else
    echo "✗ Sweep Phase FAILED"
    exit 1
fi
echo ""

# Test 5: Deep object graph
echo "Running Test: Deep Object Graph..."
./tests/gc_test_deep
if [ $? -eq 0 ]; then
    echo "✓ Deep Object Graph PASSED"
else
    echo "✗ Deep Object Graph FAILED"
    exit 1
fi
echo ""

# Test 6: Closure and stress
echo "Running Test: Closure & Stress..."
./tests/gc_test_closure_stress
if [ $? -eq 0 ]; then
    echo "✓ Closure & Stress PASSED"
else
    echo "✗ Closure & Stress FAILED"
    exit 1
fi
echo ""

echo "========================================="
echo "  All GC Tests PASSED (6/6)"
echo "========================================="
echo ""
echo "Spec Test Cases Covered:"
echo "  ✓ 1.6.1: Basic Reachability"
echo "  ✓ 1.6.2: Unreachable Object Collection"
echo "  ✓ 1.6.3: Transitive Reachability"
echo "  ✓ 1.6.4: Cyclic References"
echo "  ✓ 1.6.5: Deep Object Graph"
echo "  ✓ 1.6.6: Closure Capture"
echo "  ✓ 1.6.7: Stress Allocation"
echo ""
