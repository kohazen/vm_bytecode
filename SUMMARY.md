# Day 5: Function Calls

## What We Built Today

Today we added function call capabilities:
1. **CALL** - Call a function (save where to return, jump to function)
2. **RET** - Return from function (jump back to caller)

With functions, we can now write reusable code and even recursion!

---

## Key Concepts Explained

### What is a Function Call?

A function call does two things:
1. **Jump** to the function's code
2. **Remember** where to come back to

When the function finishes (RET), it:
1. **Retrieves** the saved return address
2. **Jumps** back to continue after the CALL

### The Return Stack

We use a **separate stack** for return addresses:

```
Data Stack:      For values and calculations
Return Stack:    For return addresses only
```

Why separate? If we mixed them, functions could corrupt return addresses!

```
Return Stack visualization:

After CALL:      [return_addr]    RSP = 1
Nested CALL:     [addr1, addr2]   RSP = 2
After RET:       [addr1]          RSP = 1
After RET:       []               RSP = 0
```

### CALL Instruction

**Opcode**: 0x40
**Format**: 1 byte opcode + 4 bytes address (5 bytes)

**What it does**:
1. Read the function address from bytecode
2. Push current PC onto return stack (this is where to return)
3. Set PC = function address (jump to function)

```
Before CALL 100:
    PC = 10 (at the CALL instruction)
    Return Stack: []

After CALL 100:
    PC = 100 (at the function)
    Return Stack: [15]  (return to address after CALL, which is PC after reading operand)
```

### RET Instruction

**Opcode**: 0x41
**Format**: 1 byte (no operand)

**What it does**:
1. Pop the return address from return stack
2. Set PC = return address (jump back to caller)

```
Before RET:
    PC = 105 (somewhere in function)
    Return Stack: [15]

After RET:
    PC = 15 (back in main code)
    Return Stack: []
```

---

## How the Code Works

### CALL Implementation

```c
case OP_CALL: {
    /* Read the function address */
    int32_t address = read_int32(vm);
    if (vm->error != VM_OK) { vm->running = false; return; }

    /* Validate the target address */
    if (address < 0 || address >= vm->code_size) {
        vm->error = VM_ERROR_CODE_BOUNDS;
        vm->running = false;
        return;
    }

    /*
     * Save the return address on the return stack.
     * vm->pc is now pointing to the instruction AFTER the CALL,
     * which is exactly where we want to return to.
     */
    if (!return_stack_push(vm, vm->pc)) {
        vm->running = false;
        return;
    }

    /* Jump to the function */
    vm->pc = address;
    break;
}
```

### RET Implementation

```c
case OP_RET: {
    /* Pop the return address from the return stack */
    int32_t return_address = return_stack_pop(vm);
    if (vm->error != VM_OK) {
        vm->running = false;
        return;
    }

    /* Jump back to the caller */
    vm->pc = return_address;
    break;
}
```

### Return Stack Operations

```c
static bool return_stack_push(VM *vm, int32_t value) {
    if (vm->rsp >= RETURN_STACK_SIZE) {
        vm->error = VM_ERROR_RETURN_STACK_OVERFLOW;
        return false;
    }
    vm->return_stack[vm->rsp] = value;
    vm->rsp++;
    return true;
}

static int32_t return_stack_pop(VM *vm) {
    if (vm->rsp <= 0) {
        vm->error = VM_ERROR_RETURN_STACK_UNDERFLOW;
        return 0;
    }
    vm->rsp--;
    return vm->return_stack[vm->rsp];
}
```

---

## Function Examples

### Example 1: Square Function

```assembly
; Main program
PUSH 5
CALL square
HALT

; square(x) = x * x
square:
    DUP         ; Duplicate x: [x, x]
    MUL         ; Multiply: [x*x]
    RET         ; Return with result on stack
```

**Execution trace**:
```
1. PUSH 5       Stack: [5]          RetStack: []
2. CALL square  Stack: [5]          RetStack: [10]  (10 = address after CALL)
3. DUP          Stack: [5, 5]       RetStack: [10]
4. MUL          Stack: [25]         RetStack: [10]
5. RET          Stack: [25]         RetStack: []    (PC = 10)
6. HALT         Stack: [25]         Done!
```

### Example 2: Nested Calls

```assembly
; main: compute x + square(x) where x=3
PUSH 3
CALL f
HALT

; f(x) = x + square(x)
f:
    DUP         ; [3, 3]
    CALL square ; [3, 9]
    ADD         ; [12]
    RET

square:
    DUP
    MUL
    RET
```
  Assembler Labels - Day 3 Tests

=== Test: Simple loop (backward jump) ===
Source:
loop:
    PUSH 1
    SUB
    DUP
    JNZ loop
    HALT

Parsed 5 instructions
=== Symbol Table (1 labels) ===
  loop                 = 0 (0x0000)  [line 1]

=== Resolved Instructions ===
[  0] addr=  0: opcode=0x01 operand=1 (0x0001)
[  1] addr=  5: opcode=0x11
[  2] addr=  6: opcode=0x03
[  3] addr=  7: opcode=0x22 operand=0 (0x0000)
[  4] addr= 12: opcode=0xFF
Total bytecode size: 13 bytes

Label resolution successful!

...

=== Test: Function calls ===
Source:
main:
    PUSH 10
    CALL double
    HALT

double:
    DUP
    ADD
    RET

Parsed 6 instructions
=== Symbol Table (2 labels) ===
  main                 = 0 (0x0000)  [line 1]
  double               = 11 (0x000B)  [line 6]

=== Resolved Instructions ===
[  0] addr=  0: opcode=0x01 operand=10 (0x000A)
[  1] addr=  5: opcode=0x40 operand=11 (0x000B)
[  2] addr= 10: opcode=0xFF
[  3] addr= 11: opcode=0x03
[  4] addr= 12: opcode=0x10
[  5] addr= 13: opcode=0x41
Total bytecode size: 14 bytes

Label resolution successful!

...

=== Test: Error - undefined label ===
Source:
PUSH 5
JMP undefined
HALT

Expected error: Line 2: Undefined label 'undefined'

=== Test: Error - duplicate label ===
Source:
start:
PUSH 1
start:
HALT

Expected error: Line 3: Label 'start' already defined on line 1

  All label tests completed!
```

---

## Address Calculation Example

Let's trace through this program:

```asm
start:              ; Address = 0 (nothing before it)
    PUSH 5          ; 5 bytes (opcode + 4-byte operand)
    PUSH 1          ; 5 bytes
loop:               ; Address = 10 (5 + 5 = 10)
    SUB             ; 1 byte
    DUP             ; 1 byte
    JNZ loop        ; 5 bytes
end:                ; Address = 17 (10 + 1 + 1 + 5 = 17)
    HALT            ; 1 byte
```

Symbol table:
```
start = 0
loop  = 10
end   = 17
```

After resolution:
- `JNZ loop` becomes `JNZ 10`

**Return stack during nested call**:
```
Before any call:     []
After CALL f:        [5]      (main's return)
After CALL square:   [5, 17]  (main's return, f's return)
After RET (square):  [5]
After RET (f):       []
```

### Example 3: Double Function

```assembly
; double(x) = x + x
PUSH 21
CALL double
HALT

double:
    DUP         ; [21, 21]
    ADD         ; [42]
    RET
```

---

## Passing Parameters

Our VM uses **stack-based parameter passing**:
- Push parameters BEFORE calling
- Function reads them from the stack
- Return value stays on the stack

```
; add(a, b) - adds two numbers
PUSH 10     ; First argument
PUSH 20     ; Second argument
CALL add    ; Call with both on stack
; Result (30) is now on stack

add:
    ADD     ; Pops both, pushes sum
    RET
```

This is simpler than having local variables, but works for our needs!

---

## Error Handling

### RET Without CALL

If you RET without a matching CALL, the return stack is empty:

```assembly
PUSH 42
RET         ; ERROR: Return stack underflow!
```

### Too Many Nested Calls

With RETURN_STACK_SIZE = 256, you can nest up to 256 function calls. More than that causes return stack overflow.

---

## How to Test It

### Building

```bash
cd student1/day5
make
```

### Running

```bash
./vm_test
```

### Expected Output

7 tests covering:
1. Simple function call
2. Function with parameter (square)
3. Multiple calls
4. Nested calls
5. Deep nesting (3 levels)
6. RET without CALL (error)
7. Double function

All tests should pass!

---

## Common Mistakes

### 1. Wrong Return Address

CALL saves the address of the instruction AFTER the CALL, not the CALL itself.

```
Address 5: CALL function  (5 bytes)
Address 10: HALT

Return address = 10 (not 5!)
```

### 2. Forgetting RET

If a function doesn't RET, execution continues into whatever code follows:

```assembly
function:
    PUSH 42
    ; Forgot RET!
    ; Falls through to next code...
```

### 3. Stack Imbalance

If a function pops more than it should, it corrupts the caller's data:

```assembly
bad_function:
    POP         ; Pops caller's data!
    POP         ; More corruption!
    RET
```

### 4. Mixing Up Stacks

Return addresses go on return stack, not data stack!

---

## Files in This Day

| File | Purpose |
|------|---------|
| `instructions.h` | Same opcode definitions |
| `vm.h` | Same VM structure |
| `vm.c` | VM with function calls added |
| `main.c` | Tests for CALL and RET |
| `Makefile` | Build instructions |
| `SUMMARY.md` | This explanation file |

---

## What's Next (Day 6)

Tomorrow we'll add the bytecode loader:
- Read bytecode from a file
- Validate the file format (magic number, version)
- Load the code into the VM

This will let us run programs from files instead of hardcoded arrays!
