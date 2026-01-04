/*
 * vm.c - Virtual Machine Implementation
 *
 * Day 2: Added arithmetic operations (ADD, SUB, MUL, DIV, CMP)
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
    if (vm->sp >= STACK_SIZE) {
        vm->error = VM_ERROR_STACK_OVERFLOW;
        return false;
    }
    vm->stack[vm->sp] = value;
    vm->sp++;
    return true;
}

/*
 * Pop a value from the stack.
 * Returns the value, or 0 if stack underflow (sets error flag).
 */
static int32_t stack_pop(VM *vm) {
    if (vm->sp <= 0) {
        vm->error = VM_ERROR_STACK_UNDERFLOW;
        return 0;
    }
    vm->sp--;
    return vm->stack[vm->sp];
}

/*
 * Peek at the top value without removing it.
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
 * Uses little-endian byte order.
 */
static int32_t read_int32(VM *vm) {
    if (vm->pc + 4 > vm->code_size) {
        vm->error = VM_ERROR_CODE_BOUNDS;
        return 0;
    }

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

VM* vm_create(void) {
    VM *vm = (VM*)malloc(sizeof(VM));
    if (!vm) return NULL;

    vm->stack = (int32_t*)malloc(STACK_SIZE * sizeof(int32_t));
    if (!vm->stack) {
        free(vm);
        return NULL;
    }

    vm->memory = (int32_t*)malloc(MEMORY_SIZE * sizeof(int32_t));
    if (!vm->memory) {
        free(vm->stack);
        free(vm);
        return NULL;
    }

    vm->return_stack = (int32_t*)malloc(RETURN_STACK_SIZE * sizeof(int32_t));
    if (!vm->return_stack) {
        free(vm->memory);
        free(vm->stack);
        free(vm);
        return NULL;
    }

    memset(vm->stack, 0, STACK_SIZE * sizeof(int32_t));
    memset(vm->memory, 0, MEMORY_SIZE * sizeof(int32_t));
    memset(vm->return_stack, 0, RETURN_STACK_SIZE * sizeof(int32_t));

    vm->sp = 0;
    vm->rsp = 0;
    vm->pc = 0;
    vm->code = NULL;
    vm->code_size = 0;
    vm->running = false;
    vm->error = VM_OK;

    return vm;
}

void vm_destroy(VM *vm) {
    if (vm) {
        if (vm->stack) free(vm->stack);
        if (vm->memory) free(vm->memory);
        if (vm->return_stack) free(vm->return_stack);
        free(vm);
    }
}

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

static void execute_instruction(VM *vm) {
    if (vm->pc >= vm->code_size) {
        vm->error = VM_ERROR_CODE_BOUNDS;
        vm->running = false;
        return;
    }

    uint8_t opcode = vm->code[vm->pc];
    vm->pc++;

    switch (opcode) {

        /* ---- PUSH: Push a value onto the stack ---- */
        case OP_PUSH: {
            int32_t value = read_int32(vm);
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

        /* ============================================
         * ARITHMETIC OPERATIONS (NEW IN DAY 2)
         * ============================================ */

        /* ---- ADD: Pop two values, push their sum ---- */
        case OP_ADD: {
            int32_t b = stack_pop(vm);  /* First pop = second operand */
            if (vm->error != VM_OK) { vm->running = false; return; }

            int32_t a = stack_pop(vm);  /* Second pop = first operand */
            if (vm->error != VM_OK) { vm->running = false; return; }

            if (!stack_push(vm, a + b)) {
                vm->running = false;
            }
            break;
        }

        /* ---- SUB: Pop two values, push their difference ---- */
        case OP_SUB: {
            int32_t b = stack_pop(vm);  /* b is subtracted */
            if (vm->error != VM_OK) { vm->running = false; return; }

            int32_t a = stack_pop(vm);  /* a is what we subtract from */
            if (vm->error != VM_OK) { vm->running = false; return; }

            /* Result is a - b (NOT b - a!) */
            if (!stack_push(vm, a - b)) {
                vm->running = false;
            }
            break;
        }

        /* ---- MUL: Pop two values, push their product ---- */
        case OP_MUL: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            if (!stack_push(vm, a * b)) {
                vm->running = false;
            }
            break;
        }

        /* ---- DIV: Pop two values, push their quotient ---- */
        case OP_DIV: {
            int32_t b = stack_pop(vm);  /* b is the divisor */
            if (vm->error != VM_OK) { vm->running = false; return; }

            /* Check for division by zero */
            if (b == 0) {
                vm->error = VM_ERROR_DIVISION_BY_ZERO;
                vm->running = false;
                return;
            }

            int32_t a = stack_pop(vm);  /* a is the dividend */
            if (vm->error != VM_OK) { vm->running = false; return; }

            /* Integer division: a / b */
            if (!stack_push(vm, a / b)) {
                vm->running = false;
            }
            break;
        }

        /* ---- CMP: Compare two values ---- */
        case OP_CMP: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            /* Push 1 if a < b, otherwise push 0 */
            int32_t result = (a < b) ? 1 : 0;
            if (!stack_push(vm, result)) {
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

VMError vm_run(VM *vm) {
    vm->running = true;
    vm->error = VM_OK;

    while (vm->running && vm->error == VM_OK) {
        execute_instruction(vm);
    }

    return vm->error;
}

/* ============================================
 * DEBUGGING AND ERROR HANDLING
 * ============================================ */

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
