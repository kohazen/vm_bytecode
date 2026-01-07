#!/bin/bash
# Demonstration script for Bytecode VM Project
# Shows the complete workflow: assemble -> execute

echo "=========================================="
echo "  Bytecode VM Project - Quick Demo"
echo "=========================================="
echo ""

# Demo 1: Simple arithmetic
echo "Demo 1: Arithmetic Test"
echo "Expected result: 42"
./vm/vm tests/test_arithmetic.bc 2>&1 | grep "Result"
echo ""

# Demo 2: Factorial
echo "Demo 2: Factorial of 5"
echo "Expected result: 120"
./vm/vm tests/factorial.bc 2>&1 | grep "Result"
echo ""

# Demo 3: Fibonacci
echo "Demo 3: 10th Fibonacci number"
echo "Expected result: 55"
./vm/vm tests/fibonacci.bc 2>&1 | grep "Result"
echo ""

# Demo 4: Create a new program
echo "Demo 4: Creating a new program from scratch"
echo ""
echo "Creating simple.asm:"
cat > /tmp/simple.asm << 'EOF'
; Simple program: calculate 10 + 20
    PUSH 10
    PUSH 20
    ADD
    HALT
EOF

cat /tmp/simple.asm
echo ""

echo "Assembling..."
./assembler/asm /tmp/simple.asm -o /tmp/simple.bc
echo ""

echo "Running:"
./vm/vm /tmp/simple.bc 2>&1 | grep "Result"
echo ""

# Summary
echo "=========================================="
echo "  All Tests Summary"
echo "=========================================="
echo ""
echo "Running all 11 tests..."
echo ""

tests=(
    "test_arithmetic:42"
    "test_stack:10"
    "test_comparison:1"
    "test_jump:200"
    "test_conditional:200"
    "test_loop:0"
    "test_memory:300"
    "test_function:20"
    "test_nested_calls:40"
    "factorial:120"
    "fibonacci:55"
)

passed=0
failed=0

for test_info in "${tests[@]}"; do
    IFS=':' read -r test expected <<< "$test_info"
    result=$(./vm/vm tests/${test}.bc 2>&1 | grep -o "Result.*: [0-9]*" | grep -o "[0-9]*$")

    if [ "$result" = "$expected" ]; then
        echo "✅ $test: $result (expected $expected)"
        ((passed++))
    else
        echo "❌ $test: $result (expected $expected)"
        ((failed++))
    fi
done

echo ""
echo "=========================================="
echo "  Final Results"
echo "=========================================="
echo "Passed: $passed/11"
echo "Failed: $failed/11"

if [ $failed -eq 0 ]; then
    echo ""
    echo "🎉 All tests PASSED!"
    echo ""
    echo "The project is fully functional and ready to submit."
else
    echo ""
    echo "⚠️  Some tests failed. Please review."
fi

echo ""
echo "=========================================="
