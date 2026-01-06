/*
 * main.c - Test Harness for the Virtual Machine (Day 3)
 *
 * Tests memory operations: STORE, LOAD
 */

#include <stdio.h>
#include <stdint.h>
#include "vm.h"
#include "instructions.h"

/*
 * Test 1: Simple STORE and LOAD
 * Program: PUSH 42, STORE 0, LOAD 0, HALT
 * Expected: 42 on stack, memory[0] = 42
 */
void test_store_load(void) {
    printf("\n=== Test 1: STORE and LOAD ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x2A, 0x00, 0x00, 0x00,   /* PUSH 42 */
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* STORE 0 (memory[0] = 42) */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* LOAD 0 (push memory[0]) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 42, STORE 0, LOAD 0, HALT\n");
    printf("Expected: Stack = [42], Memory[0] = 42\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 42 && vm->memory[0] == 42) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 2: Multiple memory locations
 * Store values at different addresses, then load them back
 */
void test_multiple_memory(void) {
    printf("\n=== Test 2: Multiple Memory Locations ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* STORE 0 */
        OP_PUSH, 0x14, 0x00, 0x00, 0x00,   /* PUSH 20 */
        OP_STORE, 0x01, 0x00, 0x00, 0x00,  /* STORE 1 */
        OP_PUSH, 0x1E, 0x00, 0x00, 0x00,   /* PUSH 30 */
        OP_STORE, 0x02, 0x00, 0x00, 0x00,  /* STORE 2 */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* LOAD 0 (push 10) */
        OP_LOAD, 0x01, 0x00, 0x00, 0x00,   /* LOAD 1 (push 20) */
        OP_LOAD, 0x02, 0x00, 0x00, 0x00,   /* LOAD 2 (push 30) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: Store 10,20,30 at M[0],M[1],M[2], then load them all\n");
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
 * Test 3: Memory as accumulator
 * Calculate 10 + 20 + 30 using memory to store intermediate result
 */
void test_memory_accumulator(void) {
    printf("\n=== Test 3: Memory as Accumulator ===\n");

    uint8_t program[] = {
        /* sum = 0 */
        OP_PUSH, 0x00, 0x00, 0x00, 0x00,   /* PUSH 0 */
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* STORE 0 (sum = 0) */

        /* sum = sum + 10 */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* LOAD 0 (push sum) */
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_ADD,                              /* ADD */
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* STORE 0 (save sum) */

        /* sum = sum + 20 */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* LOAD 0 */
        OP_PUSH, 0x14, 0x00, 0x00, 0x00,   /* PUSH 20 */
        OP_ADD,
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* STORE 0 */

        /* sum = sum + 30 */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* LOAD 0 */
        OP_PUSH, 0x1E, 0x00, 0x00, 0x00,   /* PUSH 30 */
        OP_ADD,
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* STORE 0 */

        /* Push final result */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* LOAD 0 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: sum = 0 + 10 + 20 + 30 (using memory)\n");
    printf("Expected: Stack = [60]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 60) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 4: Memory bounds check - valid edge case
 * Store at memory[255] (last valid index)
 */
void test_memory_last_index(void) {
    printf("\n=== Test 4: Last Valid Memory Index ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x63, 0x00, 0x00, 0x00,   /* PUSH 99 */
        OP_STORE, 0xFF, 0x00, 0x00, 0x00,  /* STORE 255 */
        OP_LOAD, 0xFF, 0x00, 0x00, 0x00,   /* LOAD 255 */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 99, STORE 255, LOAD 255, HALT\n");
    printf("Expected: Stack = [99]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 99) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 5: Memory bounds error - store out of bounds
 * Try to STORE at index 256 (out of bounds)
 */
void test_memory_store_bounds_error(void) {
    printf("\n=== Test 5: STORE Out of Bounds ===\n");

    uint8_t program[] = {
        OP_PUSH, 0x2A, 0x00, 0x00, 0x00,   /* PUSH 42 */
        OP_STORE, 0x00, 0x01, 0x00, 0x00,  /* STORE 256 (0x100 - out of bounds!) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: PUSH 42, STORE 256 (invalid!)\n");
    printf("Expected: Error - Memory bounds\n");
    vm_dump_state(vm);

    if (result == VM_ERROR_MEMORY_BOUNDS) {
        printf("TEST PASSED! (Correctly detected bounds error)\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 6: Memory bounds error - load out of bounds
 * Try to LOAD from index 300
 */
void test_memory_load_bounds_error(void) {
    printf("\n=== Test 6: LOAD Out of Bounds ===\n");

    uint8_t program[] = {
        OP_LOAD, 0x2C, 0x01, 0x00, 0x00,   /* LOAD 300 (0x12C - out of bounds!) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: LOAD 300 (invalid!)\n");
    printf("Expected: Error - Memory bounds\n");
    vm_dump_state(vm);

    if (result == VM_ERROR_MEMORY_BOUNDS) {
        printf("TEST PASSED! (Correctly detected bounds error)\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 7: Memory initialized to zero
 * Load from uninitialized memory
 */
void test_memory_zero_init(void) {
    printf("\n=== Test 7: Memory Initialized to Zero ===\n");

    uint8_t program[] = {
        OP_LOAD, 0x64, 0x00, 0x00, 0x00,   /* LOAD 100 (never written) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: LOAD 100 (from uninitialized memory)\n");
    printf("Expected: Stack = [0]\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 1 && vm->stack[0] == 0) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

/*
 * Test 8: Swap two values using memory
 * Swap values at M[0] and M[1]
 */
void test_swap_with_memory(void) {
    printf("\n=== Test 8: Swap Using Memory ===\n");

    uint8_t program[] = {
        /* Store A=5 at M[0], B=10 at M[1] */
        OP_PUSH, 0x05, 0x00, 0x00, 0x00,   /* PUSH 5 */
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* M[0] = 5 */
        OP_PUSH, 0x0A, 0x00, 0x00, 0x00,   /* PUSH 10 */
        OP_STORE, 0x01, 0x00, 0x00, 0x00,  /* M[1] = 10 */

        /* Swap: temp = M[0]; M[0] = M[1]; M[1] = temp */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* Load M[0] (5) -> temp */
        OP_STORE, 0x02, 0x00, 0x00, 0x00,  /* M[2] = 5 (temp) */
        OP_LOAD, 0x01, 0x00, 0x00, 0x00,   /* Load M[1] (10) */
        OP_STORE, 0x00, 0x00, 0x00, 0x00,  /* M[0] = 10 */
        OP_LOAD, 0x02, 0x00, 0x00, 0x00,   /* Load temp (5) */
        OP_STORE, 0x01, 0x00, 0x00, 0x00,  /* M[1] = 5 */

        /* Push swapped values to verify */
        OP_LOAD, 0x00, 0x00, 0x00, 0x00,   /* Push M[0] (should be 10) */
        OP_LOAD, 0x01, 0x00, 0x00, 0x00,   /* Push M[1] (should be 5) */
        OP_HALT
    };

    VM *vm = vm_create();
    vm_load_program(vm, program, sizeof(program));
    VMError result = vm_run(vm);

    printf("Program: Swap M[0]=5 and M[1]=10 using M[2] as temp\n");
    printf("Expected: Stack = [10, 5] (swapped!)\n");
    vm_dump_state(vm);

    if (result == VM_OK && vm->sp == 2 &&
        vm->stack[0] == 10 && vm->stack[1] == 5) {
        printf("TEST PASSED!\n");
    } else {
        printf("TEST FAILED!\n");
    }

    vm_destroy(vm);
}

int main(void) {
    printf("========================================\n");
    printf("  Virtual Machine - Day 3 Tests\n");
    printf("  Testing: STORE, LOAD\n");
    printf("========================================\n");

    test_store_load();
    test_multiple_memory();
    test_memory_accumulator();
    test_memory_last_index();
    test_memory_store_bounds_error();
    test_memory_load_bounds_error();
    test_memory_zero_init();
    test_swap_with_memory();

    printf("\n========================================\n");
    printf("  All tests completed!\n");
    printf("========================================\n");

    return 0;
}
