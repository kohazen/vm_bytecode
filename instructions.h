/*
 * instructions.h - Opcode Definitions for the Bytecode VM
 *
 * This file defines all the opcodes (operation codes) that our virtual machine
 * understands. It's shared between the VM and the Assembler to ensure they
 * agree on what each instruction number means.
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

/* ============================================
 * DATA MOVEMENT AND STACK MANAGEMENT
 * ============================================ */

#define OP_PUSH  0x01   /* Push a value onto the stack */
#define OP_POP   0x02   /* Remove the top value from the stack */
#define OP_DUP   0x03   /* Duplicate the top value on the stack */

/* ============================================
 * ARITHMETIC AND LOGICAL OPERATIONS
 * ============================================ */

#define OP_ADD   0x10   /* Add top two values */
#define OP_SUB   0x11   /* Subtract: second from top - top */
#define OP_MUL   0x12   /* Multiply top two values */
#define OP_DIV   0x13   /* Divide: second from top / top */
#define OP_CMP   0x14   /* Compare: push 1 if a < b, else 0 */

/* ============================================
 * CONTROL FLOW
 * ============================================ */

#define OP_JMP   0x20   /* Unconditional jump to address */
#define OP_JZ    0x21   /* Jump if top of stack is zero */
#define OP_JNZ   0x22   /* Jump if top of stack is not zero */

/* ============================================
 * MEMORY OPERATIONS
 * ============================================ */

#define OP_STORE 0x30   /* Store top of stack to memory */
#define OP_LOAD  0x31   /* Load from memory to stack */

/* ============================================
 * FUNCTION CALLS
 * ============================================ */

#define OP_CALL  0x40   /* Call a function (jump and save return address) */
#define OP_RET   0x41   /* Return from a function */

/* ============================================
 * SPECIAL
 * ============================================ */

#define OP_HALT  0xFF   /* Stop the virtual machine */

#endif /* INSTRUCTIONS_H */
