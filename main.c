/*
 * main.c - Test Harness for the Virtual Machine (Day 2)
 *
 * Tests arithmetic operations: ADD, SUB, MUL, DIV, CMP
 */

#include <stdio.h>
#include <stdint.h>
#include "vm.h"
#include "instructions.h"

/*
 * Test 1: Simple Addition
 * Program: PUSH 10, PUSH 20, ADD, HALT
 * Expected: 30
 */
void test_add(void) {
    printf("\n=== Test 1: ADD ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x14, 0x00, 0x00, 0x00,   /* PUSH 20 */
        OP_ADD,                              /* ADD */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 20, ADD, HALT\n");
    printf("Expected: 30\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 30) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 2: Subtraction
 * Program: PUSH 50, PUSH 8, SUB, HALT
 * Expected: 42 (50 - 8)
 */
void test_sub(void) {
    printf("\n=== Test 2: SUB ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x32, 0x00, 0x00, 0x00,   /* PUSH 50 */
        OP_PUSH, 0x08, 0x00, 0x00, 0x00,   /* PUSH 8 */
        OP_SUB,                              /* SUB: 50 - 8 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 50, PUSH 8, SUB, HALT\n");
    printf("Expected: 42 (50 - 8)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 3: Multiplication
 * Program: PUSH 6, PUSH 7, MUL, HALT
 * Expected: 42
 */
void test_mul(void) {
    printf("\n=== Test 3: MUL ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x06, 0x00, 0x00, 0x00,   /* PUSH 6 */
        OP_PUSH, 0x07, 0x00, 0x00, 0x00,   /* PUSH 7 */
        OP_MUL,                              /* MUL */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 6, PUSH 7, MUL, HALT\n");
    printf("Expected: 42\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 4: Division
 * Program: PUSH 84, PUSH 2, DIV, HALT
 * Expected: 42
 */
void test_div(void) {
    printf("\n=== Test 4: DIV ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x54, 0x00, 0x00, 0x00,   /* PUSH 84 */
        OP_PUSH, 0x02, 0x00, 0x00, 0x00,   /* PUSH 2 */
        OP_DIV,                              /* DIV: 84 / 2 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 84, PUSH 2, DIV, HALT\n");
    printf("Expected: 42\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 5: Division by zero
 * Program: PUSH 10, PUSH 0, DIV, HALT
 * Expected: Error - Division by zero
 */
void test_div_by_zero(void) {
    printf("\n=== Test 5: Division by Zero ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x00, 0x00, 0x00, 0x00,   /* PUSH 0 */
        OP_DIV,                              /* DIV: 10 / 0 - ERROR! */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 0, DIV\n");
    printf("Expected: Error - Division by zero\n");
    vm_dump_state(vm);

    if (result == VM_ERROR_DIVISION_BY_ZERO) {
        printf("TEST PASSED! (Correctly detected division by zero)\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 6: Compare (less than - true case)
 * Program: PUSH 3, PUSH 5, CMP, HALT
 * Expected: 1 (3 < 5 is true)
 */
void test_cmp_true(void) {
    printf("\n=== Test 6: CMP (3 < 5) ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* PUSH 3 */
        OP_PUSH, 0x05, 0x00, 0x00, 0x00,   /* PUSH 5 */
        OP_CMP,                              /* CMP: 3 < 5? */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 3, PUSH 5, CMP, HALT\n");
    printf("Expected: 1 (3 < 5 is true)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 1) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 7: Compare (less than - false case)
 * Program: PUSH 10, PUSH 5, CMP, HALT
 * Expected: 0 (10 < 5 is false)
 */
void test_cmp_false(void) {
    printf("\n=== Test 7: CMP (10 < 5) ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x05, 0x00, 0x00, 0x00,   /* PUSH 5 */
        OP_CMP,                              /* CMP: 10 < 5? */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 5, CMP, HALT\n");
    printf("Expected: 0 (10 < 5 is false)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 0) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 8: Complex expression
 * Calculate: (10 + 5) * 3 - 3 = 42
 */
void test_complex_expression(void) {
    printf("\n=== Test 8: Complex Expression ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x05, 0x00, 0x00, 0x00,   /* PUSH 5 */
        OP_ADD,                              /* ADD: 15 */
        OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* PUSH 3 */
        OP_MUL,                              /* MUL: 45 */
        OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* PUSH 3 */
        OP_SUB,                              /* SUB: 42 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: (10 + 5) * 3 - 3\n");
    printf("Expected: 42\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 9: Negative numbers in arithmetic
 * Program: PUSH 10, PUSH 15, SUB, HALT
 * Expected: -5
 */
void test_negative_result(void) {
    printf("\n=== Test 9: Negative Result ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x0F, 0x00, 0x00, 0x00,   /* PUSH 15 */
        OP_SUB,                              /* SUB: 10 - 15 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 15, SUB, HALT\n");
    printf("Expected: -5 (10 - 15)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == -5) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 10: Integer division truncation
 * Program: PUSH 10, PUSH 3, DIV, HALT
 * Expected: 3 (10 / 3 = 3, not 3.33...)
 */
void test_integer_division(void) {
    printf("\n=== Test 10: Integer Division ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x03, 0x00, 0x00, 0x00,   /* PUSH 3 */
        OP_DIV,                              /* DIV: 10 / 3 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 3, DIV, HALT\n");
    printf("Expected: 3 (integer division truncates)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 3) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

int main(void) {
    printf("========================================\n");
    printf("  Virtual Machine - Day 2 Tests\n");
    printf("  Testing: ADD, SUB, MUL, DIV, CMP\n");
    printf("========================================\n");

    test_add();
    test_sub();
    test_mul();
    test_div();
    test_div_by_zero();
    test_cmp_true();
    test_cmp_false();
    test_complex_expression();
    test_negative_result();
    test_integer_division();

    printf("\n========================================\n");
    printf("  All tests completed!\n");
    printf("========================================\n");

    return 0;
}
