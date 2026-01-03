# Day 1: Basic VM Structure + Stack Operations

## What We Built Today

Today we created the foundation of our bytecode virtual machine (VM). We implemented:
1. The core VM data structure that holds all the state
2. Basic stack operations (push, pop, peek)
3. Four instructions: PUSH, POP, DUP, and HALT
4. Error handling for stack overflow and underflow

By the end of today, we have a working VM that can run simple programs that push numbers onto a stack, duplicate them, and pop them off.

---

## Key Concepts Explained

### What is a Virtual Machine?

A **virtual machine (VM)** is a program that pretends to be a computer. Instead of running on real hardware, it executes instructions in software. Think of it like a game emulator - the emulator reads game instructions and executes them, even though your computer isn't the original game console.

Our VM is a **bytecode VM**, which means it runs programs that have been converted into a sequence of bytes (numbers). Each number represents an instruction.

### What is a Stack?

A **stack** is a data structure that works like a stack of plates:
- **Push**: Put a new plate on top
- **Pop**: Take the top plate off
- **Peek**: Look at the top plate without removing it

This is called **LIFO** - Last In, First Out. The last thing you put on is the first thing you take off.

```
Stack visualization:

    PUSH 10     PUSH 20     POP

    +----+      +----+      +----+
    | 10 |  ->  | 20 |  ->  | 10 |
    +----+      +----+      +----+
                | 10 |
                +----+
```

### Stack Pointer (SP)

The **stack pointer** is a number that tells us where the top of the stack is. When we:
- **Push**: Put value at SP, then increase SP by 1
- **Pop**: Decrease SP by 1, then read value at SP

```
Initial:    SP = 0  (stack is empty)
PUSH 10:    stack[0] = 10, SP = 1
PUSH 20:    stack[1] = 20, SP = 2
POP:        SP = 1, return stack[1] = 20
```

### Why Do We Need Overflow/Underflow Checks?

**Stack Overflow**: Trying to push when the stack is full. This could corrupt other memory.

**Stack Underflow**: Trying to pop when the stack is empty. This would read garbage data.

Our VM checks for these and stops with an error instead of crashing or giving wrong results.

---

## How the Code Works

### The VM Structure (vm.h)

```c
typedef struct {
    int32_t *stack;      // Array to hold stack values
    int sp;              // Stack Pointer - index of next free slot
    int32_t *memory;     // Global memory (for later)
    uint8_t *code;       // The bytecode program
    int code_size;       // How many bytes in the program
    int pc;              // Program Counter - which byte to read next
    bool running;        // Is the VM still running?
    VMError error;       // What went wrong (if anything)
} VM;
```

### Creating the VM (vm.c: vm_create)

1. Allocate memory for the VM structure
2. Allocate arrays for stack, memory, and return stack
3. Set everything to zero/empty
4. Return the new VM

### Running a Program (vm.c: vm_run)

The VM runs a **fetch-decode-execute loop**:

```
while (running) {
    1. FETCH:  Read the byte at position PC (this is the opcode)
    2. DECODE: Figure out which instruction it is
    3. EXECUTE: Do what that instruction says
    4. Repeat
}
```

### PUSH Instruction

**Opcode**: 0x01
**Format**: 1 byte opcode + 4 bytes value (5 bytes total)

```
PUSH 42 in bytecode:  01 2A 00 00 00
                      ^  ^^^^^^^^^
                      |  |
                      |  +-- 42 in little-endian (least significant byte first)
                      +-- opcode for PUSH
```

**What it does**:
1. Read the 4-byte value after the opcode
2. Put that value on top of the stack
3. Move to the next instruction

### POP Instruction

**Opcode**: 0x02
**Format**: 1 byte (just the opcode)

**What it does**:
1. Remove the top value from the stack
2. (We don't do anything with the value - it's just discarded)

### DUP Instruction

**Opcode**: 0x03
**Format**: 1 byte (just the opcode)

**What it does**:
1. Look at the top value (peek)
2. Push that same value again
3. Now we have two copies of the value on the stack

### HALT Instruction

**Opcode**: 0xFF
**Format**: 1 byte (just the opcode)

**What it does**:
1. Set running = false
2. The main loop stops
3. The program ends

---

## How to Test It

### Building

```bash
cd student1/day1
make
```

This compiles the code and creates `vm_test`.

### Running

```bash
./vm_test
```

Or use the Makefile:

```bash
make run
```

### Expected Output

```
========================================
  Virtual Machine - Day 1 Tests
  Testing: PUSH, POP, DUP, HALT
========================================

=== Test 1: PUSH and HALT ===
Program: PUSH 42, HALT
Expected: Stack = [42]
=== VM State ===
PC: 6
SP: 1
Running: no
Error: OK
Stack: [42]
Top of stack: 42
================
TEST PASSED!

=== Test 2: Multiple PUSH ===
Program: PUSH 10, PUSH 20, PUSH 30, HALT
Expected: Stack = [10, 20, 30]
=== VM State ===
PC: 16
SP: 3
Running: no
Error: OK
Stack: [10, 20, 30]
Top of stack: 30
================
TEST PASSED!

... (more tests) ...

========================================
  All tests completed!
========================================
```

### Cleaning Up

```bash
make clean
```

---

## What to Expect

When you run the tests, you should see:
- **6 tests** run
- Each test shows what the program does
- The VM state after running (PC, SP, Stack contents)
- "TEST PASSED!" for each test

If any test fails, check:
1. Did the code compile without errors?
2. Are all files in the same directory?
3. Look at the stack contents - do they match expected?

---

## Common Mistakes

### 1. Forgetting Little-Endian Byte Order

When writing numbers in bytecode, the **least significant byte comes first**.

```
42 in decimal = 0x0000002A in hex
In bytecode:   2A 00 00 00  (NOT 00 00 00 2A)
```

### 2. Off-by-One Errors in Stack Pointer

The stack pointer points to the **next free slot**, not the top element.
- To read the top: use `stack[sp - 1]`
- To push: use `stack[sp]`, then increment sp

### 3. Forgetting to Move PC

After reading an instruction, PC must advance:
- No-operand instructions: PC += 1
- Instructions with 4-byte operand: PC += 5 (already done in our code by advancing after opcode and after reading operand)

### 4. Not Checking for Errors

Every stack operation can fail. Always check if an error occurred before continuing.

---

## Files in This Day

| File | Purpose |
|------|---------|
| `instructions.h` | Defines opcode numbers (shared with assembler) |
| `vm.h` | VM structure and function declarations |
| `vm.c` | VM implementation (stack ops, instructions) |
| `main.c` | Test harness with example programs |
| `Makefile` | Build instructions |
| `SUMMARY.md` | This explanation file |

---

## What's Next (Day 2)

Tomorrow we'll add arithmetic operations:
- ADD: Add two numbers
- SUB: Subtract
- MUL: Multiply
- DIV: Divide
- CMP: Compare two numbers

These will let us do calculations, not just move numbers around!
