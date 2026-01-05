/*
 * vm.c - Virtual Machine Implementation
 *
 * Day 3: Added memory operations (STORE, LOAD)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "instructions.h"

/* ============================================
 * HELPER FUNCTIONS - Stack Operations
 * ============================================ */

static bool stack_push(VM *vm, int32_t value) {
    if (vm->sp >= STACK_SIZE) {
        vm->error = VM_ERROR_STACK_OVERFLOW;
        return false;
    }
    vm->stack[vm->sp] = value;
    vm->sp++;
    return true;
}

static int32_t stack_pop(VM *vm) {
    if (vm->sp <= 0) {
        vm->error = VM_ERROR_STACK_UNDERFLOW;
        return 0;
    }
    vm->sp--;
    return vm->stack[vm->sp];
}

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

    /* Reset memory to zeros when loading a new program */
    memset(vm->memory, 0, MEMORY_SIZE * sizeof(int32_t));

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

        /* ---- PUSH ---- */
        case OP_PUSH: {
            int32_t value = read_int32(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, value)) { vm->running = false; }
            break;
        }

        /* ---- POP ---- */
        case OP_POP: {
            stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; }
            break;
        }

        /* ---- DUP ---- */
        case OP_DUP: {
            int32_t value = stack_peek(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, value)) { vm->running = false; }
            break;
        }

        /* ---- ADD ---- */
        case OP_ADD: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, a + b)) { vm->running = false; }
            break;
        }

        /* ---- SUB ---- */
        case OP_SUB: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, a - b)) { vm->running = false; }
            break;
        }

        /* ---- MUL ---- */
        case OP_MUL: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, a * b)) { vm->running = false; }
            break;
        }

        /* ---- DIV ---- */
        case OP_DIV: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (b == 0) {
                vm->error = VM_ERROR_DIVISION_BY_ZERO;
                vm->running = false;
                return;
            }
            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, a / b)) { vm->running = false; }
            break;
        }

        /* ---- CMP ---- */
        case OP_CMP: {
            int32_t b = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            int32_t a = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }
            if (!stack_push(vm, (a < b) ? 1 : 0)) { vm->running = false; }
            break;
        }

        /* ============================================
         * MEMORY OPERATIONS (NEW IN DAY 3)
         * ============================================ */

        /* ---- STORE: Pop value and store at memory[index] ---- */
        case OP_STORE: {
            /* Read the memory index from bytecode */
            int32_t index = read_int32(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            /* Check bounds */
            if (index < 0 || index >= MEMORY_SIZE) {
                vm->error = VM_ERROR_MEMORY_BOUNDS;
                vm->running = false;
                return;
            }

            /* Pop value from stack and store in memory */
            int32_t value = stack_pop(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            vm->memory[index] = value;
            break;
        }

        /* ---- LOAD: Push memory[index] onto stack ---- */
        case OP_LOAD: {
            /* Read the memory index from bytecode */
            int32_t index = read_int32(vm);
            if (vm->error != VM_OK) { vm->running = false; return; }

            /* Check bounds */
            if (index < 0 || index >= MEMORY_SIZE) {
                vm->error = VM_ERROR_MEMORY_BOUNDS;
                vm->running = false;
                return;
            }

            /* Push value from memory onto stack */
            if (!stack_push(vm, vm->memory[index])) {
                vm->running = false;
            }
            break;
        }

        /* ---- HALT ---- */
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

    /* Show first few memory locations if they're non-zero */
    printf("Memory: [");
    int shown = 0;
    for (int i = 0; i < MEMORY_SIZE && shown < 5; i++) {
        if (vm->memory[i] != 0) {
            if (shown > 0) printf(", ");
            printf("M[%d]=%d", i, vm->memory[i]);
            shown++;
        }
    }
    if (shown == 0) printf("all zeros");
    printf("]\n");

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
