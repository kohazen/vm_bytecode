/*
 * vm.c - Virtual Machine Implementation
 *
 * Day 1: Basic VM structure with stack operations (PUSH, POP, DUP, HALT)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "instructions.h"

/* ============================================
 * HELPER FUNCTIONS - Stack Operations
 * ============================================ */

/*
 * Push a value onto the stack.
 * Returns true if successful, false if stack overflow.
 */
static bool stack_push(VM *vm, int32_t value) {
    /* Check if there's room on the stack */
    if (vm->sp >= STACK_SIZE) {
        vm->error = VM_ERROR_STACK_OVERFLOW;
        return false;
    }

    /* Put the value on top of the stack and move the pointer up */
    vm->stack[vm->sp] = value;
    vm->sp++;
    return true;
}

/*
 * Pop a value from the stack.
 * Returns the value, or 0 if stack underflow (sets error flag).
 */
static int32_t stack_pop(VM *vm) {
    /* Check if there's anything to pop */
    if (vm->sp <= 0) {
        vm->error = VM_ERROR_STACK_UNDERFLOW;
        return 0;
    }

    /* Move the pointer down and return the value */
    vm->sp--;
    return vm->stack[vm->sp];
}

/*
 * Peek at the top value without removing it.
 * Returns the value, or 0 if stack is empty.
 */
static int32_t stack_peek(VM *vm) {
    if (vm->sp <= 0) {
        vm->error = VM_ERROR_STACK_UNDERFLOW;
        return 0;
    }
    return vm->stack[vm->sp - 1];
}

/* ============================================
 * HELPER FUNCTIONS - Reading from Bytecode
 * ============================================ */

/*
 * Read a 32-bit integer from the bytecode at current PC.
 * Uses little-endian byte order (least significant byte first).
 */
static int32_t read_int32(VM *vm) {
    if (vm->pc + 4 > vm->code_size) {
        vm->error = VM_ERROR_CODE_BOUNDS;
        return 0;
    }

    /* Read 4 bytes in little-endian order */
    int32_t value = (int32_t)vm->code[vm->pc] |
                    ((int32_t)vm->code[vm->pc + 1] << 8) |
                    ((int32_t)vm->code[vm->pc + 2] << 16) |
                    ((int32_t)vm->code[vm->pc + 3] << 24);
    vm->pc += 4;
    return value;
}

/* ============================================
 * VM LIFECYCLE FUNCTIONS
 * ============================================ */

/*
 * Create a new virtual machine.
 * Allocates memory for all the components.
 */
VM* vm_create(void) {
    /* Allocate the VM structure */
    VM *vm = (VM*)malloc(sizeof(VM));
    if (!vm) return NULL;

    /* Allocate the data stack */
    vm->stack = (int32_t*)malloc(STACK_SIZE * sizeof(int32_t));
    if (!vm->stack) {
        free(vm);
        return NULL;
    }

    /* Allocate global memory */
    vm->memory = (int32_t*)malloc(MEMORY_SIZE * sizeof(int32_t));
    if (!vm->memory) {
        free(vm->stack);
        free(vm);
        return NULL;
    }

    /* Allocate return stack (for function calls - we'll use this later) */
    vm->return_stack = (int32_t*)malloc(RETURN_STACK_SIZE * sizeof(int32_t));
    if (!vm->return_stack) {
        free(vm->memory);
        free(vm->stack);
        free(vm);
        return NULL;
    }

    /* Initialize everything to zero/empty */
    memset(vm->stack, 0, STACK_SIZE * sizeof(int32_t));
    memset(vm->memory, 0, MEMORY_SIZE * sizeof(int32_t));
    memset(vm->return_stack, 0, RETURN_STACK_SIZE * sizeof(int32_t));

    vm->sp = 0;              /* Stack is empty */
    vm->rsp = 0;             /* Return stack is empty */
    vm->pc = 0;              /* Start at first instruction */
    vm->code = NULL;         /* No program loaded yet */
    vm->code_size = 0;
    vm->running = false;
    vm->error = VM_OK;

    return vm;
}

/*
 * Destroy the virtual machine and free all memory.
 */
void vm_destroy(VM *vm) {
    if (vm) {
        if (vm->stack) free(vm->stack);
        if (vm->memory) free(vm->memory);
        if (vm->return_stack) free(vm->return_stack);
        /* Note: We don't free vm->code because we didn't allocate it */
        free(vm);
    }
}

/*
 * Load a program into the VM.
 * The bytecode is not copied - we just point to it.
 */
VMError vm_load_program(VM *vm, uint8_t *bytecode, int size) {
    vm->code = bytecode;
    vm->code_size = size;
    vm->pc = 0;
    vm->sp = 0;
    vm->rsp = 0;
    vm->running = false;
    vm->error = VM_OK;
    return VM_OK;
}

/* ============================================
 * INSTRUCTION EXECUTION
 * ============================================ */

/*
 * Execute a single instruction.
 * This is the heart of the VM - the "fetch-decode-execute" cycle.
 */
static void execute_instruction(VM *vm) {
    /* Check bounds */
    if (vm->pc >= vm->code_size) {
        vm->error = VM_ERROR_CODE_BOUNDS;
        vm->running = false;
        return;
    }

    /* Fetch the opcode */
    uint8_t opcode = vm->code[vm->pc];
    vm->pc++;  /* Move to next byte (or operand) */

    /* Decode and execute */
    switch (opcode) {

        /* ---- PUSH: Push a value onto the stack ---- */
        case OP_PUSH: {
            int32_t value = read_int32(vm);  /* Read the 4-byte operand */
            if (vm->error != VM_OK) {
                vm->running = false;
                return;
            }
            if (!stack_push(vm, value)) {
                vm->running = false;
            }
            break;
        }

        /* ---- POP: Remove top value from stack ---- */
        case OP_POP: {
            stack_pop(vm);
            if (vm->error != VM_OK) {
                vm->running = false;
            }
            break;
        }

        /* ---- DUP: Duplicate top value ---- */
        case OP_DUP: {
            int32_t value = stack_peek(vm);
            if (vm->error != VM_OK) {
                vm->running = false;
                return;
            }
            if (!stack_push(vm, value)) {
                vm->running = false;
            }
            break;
        }

        /* ---- HALT: Stop execution ---- */
        case OP_HALT: {
            vm->running = false;
            break;
        }

        /* ---- Unknown opcode ---- */
        default: {
            vm->error = VM_ERROR_INVALID_OPCODE;
            vm->running = false;
            break;
        }
    }
}

/*
 * Run the VM until it halts or hits an error.
 */
VMError vm_run(VM *vm) {
    vm->running = true;
    vm->error = VM_OK;

    /* Main execution loop */
    while (vm->running && vm->error == VM_OK) {
        execute_instruction(vm);
    }

    return vm->error;
}

/* ============================================
 * DEBUGGING AND ERROR HANDLING
 * ============================================ */

/*
 * Print the current state of the VM (for debugging).
 */
void vm_dump_state(VM *vm) {
    printf("=== VM State ===\n");
    printf("PC: %d\n", vm->pc);
    printf("SP: %d\n", vm->sp);
    printf("Running: %s\n", vm->running ? "yes" : "no");
    printf("Error: %s\n", vm_error_string(vm->error));

    printf("Stack: [");
    for (int i = 0; i < vm->sp; i++) {
        printf("%d", vm->stack[i]);
        if (i < vm->sp - 1) printf(", ");
    }
    printf("]\n");

    if (vm->sp > 0) {
        printf("Top of stack: %d\n", vm->stack[vm->sp - 1]);
    }
    printf("================\n");
}

/*
 * Convert an error code to a human-readable string.
 */
const char* vm_error_string(VMError error) {
    switch (error) {
        case VM_OK:                      return "OK";
        case VM_ERROR_STACK_OVERFLOW:    return "Stack overflow";
        case VM_ERROR_STACK_UNDERFLOW:   return "Stack underflow";
        case VM_ERROR_INVALID_OPCODE:    return "Invalid opcode";
        case VM_ERROR_DIVISION_BY_ZERO:  return "Division by zero";
        case VM_ERROR_MEMORY_BOUNDS:     return "Memory access out of bounds";
        case VM_ERROR_CODE_BOUNDS:       return "Code access out of bounds";
        case VM_ERROR_RETURN_STACK_OVERFLOW:  return "Return stack overflow";
        case VM_ERROR_RETURN_STACK_UNDERFLOW: return "Return stack underflow";
        case VM_ERROR_FILE_IO:           return "File I/O error";
        default:                         return "Unknown error";
    }
}
