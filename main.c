/*
 * main.c - Test Harness for the Virtual Machine (Day 1)
 *
 * This program tests our basic VM with hardcoded bytecode.
 * We test PUSH, POP, DUP, and HALT instructions.
 */

#include <stdio.h>
#include <stdint.h>
#include "vm.h"
#include "instructions.h"

/*
 * Test 1: Simple PUSH and HALT
 * Program: PUSH 42, HALT
 * Expected: Stack contains [42]
 */
void test_push_halt(void) {
    printf("\n=== Test 1: PUSH and HALT ===\n");

    /* Build the bytecode manually:
     * PUSH 42 = 0x01 followed by 42 in little-endian (0x2A, 0x00, 0x00, 0x00)
     * HALT    = 0xFF
     */
    uint8_t program[] = {
        OP_PUSH, 0x2A, 0x00, 0x00, 0x00,   /* PUSH 42 */
        OP_HALT                              /* HALT */
    };

    VM *vm = vm_create();
    if (!vm) {
        printf("Failed to create VM!\n");
        return;
    }

    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 42, HALT\n");
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
 * Test 2: Multiple PUSH operations
 * Program: PUSH 10, PUSH 20, PUSH 30, HALT
 * Expected: Stack contains [10, 20, 30]
 */
void test_multiple_push(void) {
    printf("\n=== Test 2: Multiple PUSH ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_PUSH, 0x14, 0x00, 0x00, 0x00,   /* PUSH 20 */
        OP_PUSH, 0x1E, 0x00, 0x00, 0x00,   /* PUSH 30 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 10, PUSH 20, PUSH 30, HALT\n");
    printf("Expected: Stack = [10, 20, 30]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 3 &&
        vm->stack[0] == 10 && vm->stack[1] == 20 && vm->stack[2] == 30) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 3: PUSH and POP
 * Program: PUSH 100, PUSH 200, POP, HALT
 * Expected: Stack contains [100] (200 was popped)
 */
void test_push_pop(void) {
    printf("\n=== Test 3: PUSH and POP ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x64, 0x00, 0x00, 0x00,   /* PUSH 100 */
        OP_PUSH, 0xC8, 0x00, 0x00, 0x00,   /* PUSH 200 */
        OP_POP,                              /* POP (removes 200) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 100, PUSH 200, POP, HALT\n");
    printf("Expected: Stack = [100]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 100) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 4: DUP operation
 * Program: PUSH 7, DUP, HALT
 * Expected: Stack contains [7, 7]
 */
void test_dup(void) {
    printf("\n=== Test 4: DUP ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x07, 0x00, 0x00, 0x00,   /* PUSH 7 */
        OP_DUP,                              /* DUP */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 7, DUP, HALT\n");
    printf("Expected: Stack = [7, 7]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 2 &&
        vm->stack[0] == 7 && vm->stack[1] == 7) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 5: Stack underflow (error handling)
 * Program: POP (with empty stack)
 * Expected: Error - stack underflow
 */
void test_underflow(void) {
    printf("\n=== Test 5: Stack Underflow ===\n");

    uint8_t program[] = {
        OP_POP,   /* POP from empty stack - should fail */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: POP (on empty stack)\n");
    printf("Expected: Error - Stack underflow\n");
    vm_dump_state(vm);

    if (result == VM_ERROR_STACK_UNDERFLOW) {
        printf("TEST PASSED! (Correctly detected underflow)\n");
    } else {
        printf("TEST FAILED! (Should have reported underflow)\n");
    }

    vm_destroy(vm);
}

/*
 * Test 6: Negative numbers
 * Program: PUSH -5, HALT
 * Expected: Stack contains [-5]
 */
void test_negative(void) {
    printf("\n=== Test 6: Negative Numbers ===\n");

    /* -5 in two's complement little-endian: 0xFB, 0xFF, 0xFF, 0xFF */
    uint8_t program[] = {
        OP_PUSH, 0xFB, 0xFF, 0xFF, 0xFF,   /* PUSH -5 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH -5, HALT\n");
    printf("Expected: Stack = [-5]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == -5) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

int main(void) {
    printf("========================================\n");
    printf("  Virtual Machine - Day 1 Tests\n");
    printf("  Testing: PUSH, POP, DUP, HALT\n");
    printf("========================================\n");

    test_push_halt();
    test_multiple_push();
    test_push_pop();
    test_dup();
    test_underflow();
    test_negative();

    printf("\n========================================\n");
    printf("  All tests completed!\n");
    printf("========================================\n");

    return 0;
}
