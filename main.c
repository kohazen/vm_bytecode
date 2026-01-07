/*
 * main.c - Test Harness for the Virtual Machine (Day 5)
 *
 * Tests function calls: CALL, RET
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "labels.h"
#include "codegen.h"

/*
 * Test 1: Simple function call
 * Main calls a function that pushes 42
 *
 * Layout:
 * 0-4:   CALL 10     ; call the function at address 10
 * 5-9:   HALT        ; (actually at address 5)
 * Let me recalculate...
 *
 * Actually HALT is 1 byte, so:
 * 0-4:   CALL 6
 * 5:     HALT
 * 6-10:  PUSH 42
 * 11:    RET
 */
void test_simple_call(void) {
    printf("\n=== Test 1: Simple Function Call ===\n");

    uint8_t program[] = {
        /* Main: call the function, then halt */
        OP_CALL, 0x06, 0x00, 0x00, 0x00,   /* 0: CALL 6 */
        OP_HALT,                             /* 5: HALT */

        /* Function at address 6: push 42 and return */
        OP_PUSH, 0x2A, 0x00, 0x00, 0x00,   /* 6: PUSH 42 */
        OP_RET                               /* 11: RET */
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: CALL function, HALT; function: PUSH 42, RET\n");
    printf("Expected: Stack = [42]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 2: Function with parameter on stack
 * Push value, call square function that reads from stack
 *
 * square(x) = x * x
 */
void test_function_with_param(void) {
    printf("\n=== Test 2: Function with Parameter (Square) ===\n");

    uint8_t program[] = {
        /* Main: push 5, call square, halt */
        OP_PUSH, 0x05, 0x00, 0x00, 0x00,   /* 0: PUSH 5 */
        OP_CALL, 0x0B, 0x00, 0x00, 0x00,   /* 5: CALL 11 */
        OP_HALT,                             /* 10: HALT */

        /* square function at 11: DUP, MUL, RET */
        OP_DUP,                              /* 11: DUP (stack: [5, 5]) */
        OP_MUL,                              /* 12: MUL (stack: [25]) */
        OP_RET                               /* 13: RET */
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 5, CALL square; square: DUP, MUL, RET\n");
    printf("Expected: Stack = [25] (5 * 5)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 25) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 3: Multiple function calls
 * Call add function twice
 */
void test_multiple_calls(void) {
    printf("\n=== Test 3: Multiple Function Calls ===\n");

    uint8_t program[] = {
        /* Main: 10 + 20, then result + 12 */
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* 0: PUSH 10 */
        OP_PUSH, 0x14, 0x00, 0x00, 0x00,   /* 5: PUSH 20 */
        OP_CALL, 0x1B, 0x00, 0x00, 0x00,   /* 10: CALL add (at 27) */
        OP_PUSH, 0x0C, 0x00, 0x00, 0x00,   /* 15: PUSH 12 */
        OP_CALL, 0x1B, 0x00, 0x00, 0x00,   /* 20: CALL add (at 27) */
        OP_HALT,                             /* 25: HALT */
        0x00,                                /* 26: padding for alignment */

        /* add function at 27: ADD, RET */
        OP_ADD,                              /* 27: ADD */
        OP_RET                               /* 28: RET */
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 20, CALL add, PUSH 12, CALL add\n");
    printf("Expected: Stack = [42] (10+20+12)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 4: Nested function calls
 * main calls f1, f1 calls f2
 */
void test_nested_calls(void) {
    printf("\n=== Test 4: Nested Function Calls ===\n");

    uint8_t program[] = {
        /* main: PUSH 3, CALL f1, HALT */
        OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* 0: PUSH 3 */
        OP_CALL, 0x0B, 0x00, 0x00, 0x00,   /* 5: CALL f1 (at 11) */
        OP_HALT,                             /* 10: HALT */

        /* f1 at 11: DUP, CALL f2 (square), ADD, RET */
        /* Computes x + x^2 = 3 + 9 = 12 */
        OP_DUP,                              /* 11: DUP (stack: [3, 3]) */
        OP_CALL, 0x17, 0x00, 0x00, 0x00,   /* 12: CALL f2 (at 23) */
        OP_ADD,                              /* 17: ADD (stack: [12]) */
        OP_RET,                              /* 18: RET */

        0x00, 0x00, 0x00, 0x00,             /* padding to 23 */

        /* f2 at 23: DUP, MUL, RET (square function) */
        OP_DUP,                              /* 23: DUP */
        OP_MUL,                              /* 24: MUL */
        OP_RET                               /* 25: RET */
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: f(x) = x + square(x), where x=3\n");
    printf("Expected: Stack = [12] (3 + 3*3)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 12) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    /* Step 5: Generate bytecode */
    CodeGenerator codegen;
    codegen_init(&codegen);

/*
 * Test 5: Function calling itself (simple recursion base case)
 * Not true recursion, just testing that return works correctly
 */
void test_deep_nesting(void) {
    printf("\n=== Test 5: Deep Nesting (3 levels) ===\n");

    uint8_t program[] = {
        /* main: PUSH 1, CALL f1, HALT */
        OP_PUSH, 0x01, 0x00, 0x00, 0x00,   /* 0: PUSH 1 */
        OP_CALL, 0x0B, 0x00, 0x00, 0x00,   /* 5: CALL f1 */
        OP_HALT,                             /* 10: HALT */

        /* f1: PUSH 2, CALL f2, ADD, RET */
        OP_PUSH, 0x02, 0x00, 0x00, 0x00,   /* 11: PUSH 2 */
        OP_CALL, 0x1B, 0x00, 0x00, 0x00,   /* 16: CALL f2 */
        OP_ADD,                              /* 21: ADD */
        OP_RET,                              /* 22: RET */

        0x00, 0x00, 0x00,                    /* padding to 26 */

        /* f2 at 27: PUSH 3, CALL f3, ADD, RET */
        OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* 27: PUSH 3 */
        OP_CALL, 0x25, 0x00, 0x00, 0x00,   /* 32: CALL f3 (at 37) */
        OP_ADD,                              /* 37: ADD */
        OP_RET,                              /* 38: RET */

        /* f3 at 37: PUSH 4, RET */
        OP_PUSH, 0x04, 0x00, 0x00, 0x00,   /* 39: PUSH 4 */
        OP_RET                               /* 44: RET */
    };

    /* Adjust addresses */
    program[16] = 0x1B;  /* f2 at 27 */
    program[32] = 0x27;  /* f3 at 39 */

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: f1 calls f2 calls f3, each adds a number\n");
    printf("Expected: Stack = [10] (1+2+3+4)\n");
    vm_dump_state(vm);

    /* 1 + (2 + (3 + 4)) = 10 */
    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 10) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 6: RET without CALL (error)
 */
void test_ret_without_call(void) {
    printf("\n=== Test 6: RET Without CALL (Error) ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x2A, 0x00, 0x00, 0x00,   /* PUSH 42 */
        OP_RET                               /* RET - ERROR: no return address! */
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 42, RET (without CALL)\n");
    printf("Expected: Error - Return stack underflow\n");
    vm_dump_state(vm);

    if (result == VM_ERROR_RETURN_STACK_UNDERFLOW) {
        printf("TEST PASSED! (Correctly detected error)\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 7: Simple "double" function
 * double(x) = x + x
 */
void test_double_function(void) {
    printf("\n=== Test 7: Double Function ===\n");

    uint8_t program[] = {
        /* main: PUSH 21, CALL double, HALT */
        OP_PUSH, 0x15, 0x00, 0x00, 0x00,   /* 0: PUSH 21 */
        OP_CALL, 0x0B, 0x00, 0x00, 0x00,   /* 5: CALL double */
        OP_HALT,                             /* 10: HALT */

        /* double at 11: DUP, ADD, RET */
        OP_DUP,                              /* 11: DUP */
        OP_ADD,                              /* 12: ADD */
        OP_RET                               /* 13: RET */
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 21, CALL double; double: DUP, ADD, RET\n");
    printf("Expected: Stack = [42] (21 + 21)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    return true;
}

int main(void) {
    printf("========================================\n");
    printf("  Virtual Machine - Day 5 Tests\n");
    printf("  Testing: CALL, RET (Function Calls)\n");
    printf("========================================\n");

    test_simple_call();
    test_function_with_param();
    test_multiple_calls();
    test_nested_calls();
    test_deep_nesting();
    test_ret_without_call();
    test_double_function();

    printf("\n========================================\n");
    printf("  All bytecode files generated!\n");
    printf("========================================\n");
    printf("\nGenerated files:\n");
    printf("  test_add.bc     - Simple addition (40 + 2 = 42)\n");
    printf("  test_expr.bc    - Expression ((5 + 3) * 2 = 16)\n");
    printf("  test_loop.bc    - Loop (count from 3 to 0)\n");
    printf("  test_memory.bc  - Memory (100 + 200 = 300)\n");
    printf("  test_func.bc    - Function (10 * 2 = 20)\n");
    printf("  test_cond.bc    - Conditional (0 → 200)\n");
    printf("\nRun with VM: ../student1/day7/vm test_add.bc\n");

    return 0;
}
