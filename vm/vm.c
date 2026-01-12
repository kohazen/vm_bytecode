#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"  /* Includes gc.h automatically */

VM* vm_create(void) {
    VM *vm = (VM*)malloc(sizeof(VM));
    if (!vm) return NULL;

    vm->stack = (int32_t*)malloc(STACK_SIZE * sizeof(int32_t));
    if (!vm->stack) { free(vm); return NULL; }

    vm->memory = (int32_t*)malloc(MEMORY_SIZE * sizeof(int32_t));
    if (!vm->memory) { free(vm->stack); free(vm); return NULL; }

    vm->return_stack = (int32_t*)malloc(RETURN_STACK_SIZE * sizeof(int32_t));
    if (!vm->return_stack) { free(vm->memory); free(vm->stack); free(vm); return NULL; }

    vm->value_stack = (Value*)malloc(VM_STACK_MAX * sizeof(Value));
    if (!vm->value_stack) {
        free(vm->return_stack); free(vm->memory); free(vm->stack); free(vm);
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

    /* Initialize GC */
    gc_init(vm);

    return vm;
}

void vm_destroy(VM *vm) {
    if (vm) {
        /* Cleanup GC first */
        gc_cleanup(vm);

        if (vm->stack) free(vm->stack);
        if (vm->memory) free(vm->memory);
        if (vm->return_stack) free(vm->return_stack);
        if (vm->value_stack) free(vm->value_stack);
        free(vm);
    }
}

VMError vm_load_program(VM *vm, uint8_t *bytecode, int size) {
    /* Stub - not needed for GC tests */
    vm->code = bytecode;
    vm->code_size = size;
    return VM_OK;
}

VMError vm_run(VM *vm) {
    /* Stub - not needed for GC tests */
    (void)vm;  /* Suppress unused parameter warning */
    return VM_OK;
}

void vm_dump_state(VM *vm) {
    printf("VM State:\n");
    printf("  Stack Pointer: %d\n", vm->sp);
    printf("  Program Counter: %d\n", vm->pc);
    printf("  GC Objects: %d\n", vm->num_objects);
    printf("  GC Threshold: %d\n", vm->max_objects);
    printf("  Auto GC: %s\n", vm->auto_gc ? "enabled" : "disabled");
}

const char* vm_error_string(VMError error) {
    switch (error) {
        case VM_OK: return "OK";
        case VM_ERROR_STACK_OVERFLOW: return "Stack Overflow";
        case VM_ERROR_STACK_UNDERFLOW: return "Stack Underflow";
        case VM_ERROR_INVALID_OPCODE: return "Invalid Opcode";
        case VM_ERROR_DIVISION_BY_ZERO: return "Division by Zero";
        case VM_ERROR_MEMORY_BOUNDS: return "Memory Bounds Error";
        case VM_ERROR_CODE_BOUNDS: return "Code Bounds Error";
        case VM_ERROR_RETURN_STACK_OVERFLOW: return "Return Stack Overflow";
        case VM_ERROR_RETURN_STACK_UNDERFLOW: return "Return Stack Underflow";
        case VM_ERROR_FILE_IO: return "File I/O Error";
        default: return "Unknown Error";
    }
}
