/*
 * vm.h - Virtual Machine Structure and Function Declarations
 *
 * This file defines what our virtual machine looks like in memory
 * and declares the functions we'll use to work with it.
 */

#ifndef VM_H
#define VM_H

#include <stdint.h>    /* For int32_t, uint8_t - fixed-size integers */
#include <stdbool.h>   /* For bool, true, false */

/* ============================================
 * CONFIGURATION CONSTANTS
 * ============================================ */

#define STACK_SIZE      1024    /* Maximum items on the data stack */
#define MEMORY_SIZE     256     /* Number of memory cells available */
#define RETURN_STACK_SIZE 256   /* Maximum nested function calls */

/* ============================================
 * ERROR CODES
 * ============================================
 * These tell us what went wrong if the VM stops unexpectedly.
 */

typedef enum {
    VM_OK = 0,                  /* Everything is fine */
    VM_ERROR_STACK_OVERFLOW,    /* Tried to push too many items */
    VM_ERROR_STACK_UNDERFLOW,   /* Tried to pop from empty stack */
    VM_ERROR_INVALID_OPCODE,    /* Unknown instruction encountered */
    VM_ERROR_DIVISION_BY_ZERO,  /* Tried to divide by zero */
    VM_ERROR_MEMORY_BOUNDS,     /* Invalid memory address */
    VM_ERROR_CODE_BOUNDS,       /* Program counter went out of bounds */
    VM_ERROR_RETURN_STACK_OVERFLOW,   /* Too many nested calls */
    VM_ERROR_RETURN_STACK_UNDERFLOW,  /* RET without CALL */
    VM_ERROR_FILE_IO            /* Problem reading bytecode file */
} VMError;

/* ============================================
 * THE VIRTUAL MACHINE STRUCTURE
 * ============================================
 * This is the "brain" of our VM - it holds all the state needed
 * to run a program.
 */

typedef struct {
    /* The Data Stack - where we do all our calculations */
    int32_t *stack;         /* Array to hold stack values */
    int sp;                 /* Stack Pointer - points to next free slot */

    /* Global Memory - for storing variables */
    int32_t *memory;        /* Array for memory storage */

    /* The Program */
    uint8_t *code;          /* The bytecode we're running */
    int code_size;          /* How many bytes of code we have */
    int pc;                 /* Program Counter - which instruction to run next */

    /* Return Stack - for function calls */
    int32_t *return_stack;  /* Stores return addresses */
    int rsp;                /* Return Stack Pointer */

    /* VM State */
    bool running;           /* Is the VM still running? */
    VMError error;          /* What error occurred (if any) */

} VM;

/* ============================================
 * FUNCTION DECLARATIONS
 * ============================================ */

/* Create and destroy the VM */
VM* vm_create(void);
void vm_destroy(VM *vm);

/* Load and run programs */
VMError vm_load_program(VM *vm, uint8_t *bytecode, int size);
VMError vm_run(VM *vm);

/* For debugging - print the current state */
void vm_dump_state(VM *vm);

/* Get error message as string */
const char* vm_error_string(VMError error);

#endif /* VM_H */
