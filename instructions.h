/*
 * instructions.h - Opcode Definitions for the Bytecode VM
 *
 * This file defines all the opcodes (operation codes) that our virtual machine
 * understands. It's shared between the VM and the Assembler to ensure they
 * agree on what each instruction number means.
 *
 * SHARED FILE: This header is used by both Student 1 (VM) and Student 2 (Assembler)
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* ============================================
 * DATA MOVEMENT AND STACK MANAGEMENT
 * ============================================ */

#define OP_PUSH  0x01   /* Push a 32-bit value onto the stack */
#define OP_POP   0x02   /* Remove the top value from the stack */
#define OP_DUP   0x03   /* Duplicate the top value on the stack */

/* ============================================
 * ARITHMETIC AND COMPARISON OPERATIONS
 * ============================================ */

#define OP_ADD   0x10   /* Add top two values: push(pop() + pop()) */
#define OP_SUB   0x11   /* Subtract: a = pop(), b = pop(), push(b - a) */
#define OP_MUL   0x12   /* Multiply top two values */
#define OP_DIV   0x13   /* Divide: a = pop(), b = pop(), push(b / a) */
#define OP_CMP   0x14   /* Compare: a = pop(), b = pop(), push(b < a ? 1 : 0) */

/* ============================================
 * CONTROL FLOW
 * ============================================ */

#define OP_JMP   0x20   /* Unconditional jump to address */
#define OP_JZ    0x21   /* Jump if top of stack is zero (pops value) */
#define OP_JNZ   0x22   /* Jump if top of stack is not zero (pops value) */

/* ============================================
 * MEMORY OPERATIONS
 * ============================================ */

#define OP_STORE 0x30   /* Store top of stack to memory[index] */
#define OP_LOAD  0x31   /* Load from memory[index] to stack */

/* ============================================
 * FUNCTION CALLS
 * ============================================ */

#define OP_CALL  0x40   /* Call function: save return address, jump to target */
#define OP_RET   0x41   /* Return from function: pop return address, jump back */

/* ============================================
 * SPECIAL
 * ============================================ */

#define OP_HALT  0xFF   /* Stop the virtual machine */

#endif /* INSTRUCTIONS_H */
