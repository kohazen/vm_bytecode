# Day 2: Arithmetic Operations

## What We Built Today

Today we added arithmetic capabilities to our VM. We implemented five new instructions:
1. **ADD** - Add two numbers
2. **SUB** - Subtract two numbers
3. **MUL** - Multiply two numbers
4. **DIV** - Divide two numbers (with division-by-zero check)
5. **CMP** - Compare two numbers (less than)

Now our VM can do actual calculations, not just move numbers around!

---

## Key Concepts Explained

### Stack-Based Arithmetic

In a stack-based VM, we don't use variables like `x = 5 + 3`. Instead:
1. Push the operands onto the stack
2. Execute the operation (it pops operands, pushes result)

```
To calculate 5 + 3:

    PUSH 5      PUSH 3       ADD

    +----+      +----+      +----+
    | 5  |  ->  | 3  |  ->  | 8  |    Result!
    +----+      +----+      +----+
                | 5  |
                +----+
```

### The Order of Operations (Critical!)

When we pop two values for an operation like SUB or DIV, the order matters:

```
Stack:  [..., a, b]    (b is on top)
        ^^^^^^^^^^

SUB pops:  First pop = b
           Second pop = a

Result:    a - b   (NOT b - a!)
```

**Example**: `PUSH 10, PUSH 3, SUB` gives us `10 - 3 = 7`, not `-7`.

This is because:
1. First, we push 10 (stack: [10])
2. Then, we push 3 (stack: [10, 3])
3. SUB pops 3 first (the "subtrahend")
4. SUB pops 10 second (the "minuend")
5. Result is 10 - 3 = 7

### Integer Division

Our VM uses **integer division** - it truncates the decimal part:
- `10 / 3 = 3` (not 3.33...)
- `7 / 2 = 3` (not 3.5)
- `-7 / 2 = -3` (in C, integer division rounds toward zero)

### The CMP Instruction

CMP compares two values and pushes a result:
- Push 1 if the first operand is **less than** the second
- Push 0 otherwise

```
CMP behavior:
    Pop b (top)
    Pop a (second)
    Push (a < b) ? 1 : 0

Example: PUSH 3, PUSH 5, CMP
    Stack after: [1]    because 3 < 5 is true
```

This will be useful for loops and conditionals (coming in Day 4).

---

## How the Code Works

### ADD Instruction Implementation

```c
case OP_ADD: {
    int32_t b = stack_pop(vm);  // First pop = second operand
    if (vm->error != VM_OK) { vm->running = false; return; }

    int32_t a = stack_pop(vm);  // Second pop = first operand
    if (vm->error != VM_OK) { vm->running = false; return; }

    if (!stack_push(vm, a + b)) {  // Push the result
        vm->running = false;
    }
    break;
}
```

**Step by step**:
1. Pop the first value (this becomes `b`, the second operand)
2. Check if popping caused an error (stack underflow?)
3. Pop the second value (this becomes `a`, the first operand)
4. Check for errors again
5. Calculate `a + b` and push it back
6. Check if pushing caused an error (stack overflow?)

### DIV Instruction - Division by Zero Check

```c
case OP_DIV: {
    int32_t b = stack_pop(vm);  // b is the divisor
    if (vm->error != VM_OK) { vm->running = false; return; }

    // IMPORTANT: Check for division by zero!
    if (b == 0) {
        vm->error = VM_ERROR_DIVISION_BY_ZERO;
        vm->running = false;
        return;
    }

    int32_t a = stack_pop(vm);  // a is the dividend
    if (vm->error != VM_OK) { vm->running = false; return; }

    if (!stack_push(vm, a / b)) {
        vm->running = false;
    }
    break;
}
```

**Why check for zero?** In C, dividing by zero causes undefined behavior - the program might crash, give garbage, or anything. Our VM gracefully reports an error instead.

### CMP Instruction - Comparison

```c
case OP_CMP: {
    int32_t b = stack_pop(vm);
    if (vm->error != VM_OK) { vm->running = false; return; }

    int32_t a = stack_pop(vm);
    if (vm->error != VM_OK) { vm->running = false; return; }

    // The ternary operator: (condition) ? value_if_true : value_if_false
    int32_t result = (a < b) ? 1 : 0;

    if (!stack_push(vm, result)) {
        vm->running = false;
    }
    break;
}
```

---

## Calculating Expressions

### Example: (10 + 5) * 3 - 3 = 42

In our assembly:
```assembly
PUSH 10
PUSH 5
ADD         ; Stack: [15]
PUSH 3
MUL         ; Stack: [45]
PUSH 3
SUB         ; Stack: [42]
HALT
```

**Execution trace**:
```
Instruction    Stack After
-----------    -----------
PUSH 10        [10]
PUSH 5         [10, 5]
ADD            [15]
PUSH 3         [15, 3]
MUL            [45]
PUSH 3         [45, 3]
SUB            [42]
HALT           [42]  (program stops)
```

### How to Convert Infix to Stack-Based

**Infix notation**: `(10 + 5) * 3 - 3`

To convert to stack-based:
1. Identify the order of operations
2. Push operands before each operation
3. Do innermost operations first

```
(10 + 5) * 3 - 3
    ^^^^^^^         First: 10 + 5
    ^^^^^^^^^^^     Then: result * 3
    ^^^^^^^^^^^^^^^  Finally: result - 3
```

Stack-based:
```
PUSH 10, PUSH 5, ADD    ; Do (10 + 5)
PUSH 3, MUL             ; Do result * 3
PUSH 3, SUB             ; Do result - 3
```

---

## How to Test It

### Building

```bash
cd student1/day2
make
```

### Running

```bash
./vm_test
```

### Expected Output

You should see 10 tests:
1. ADD: Basic addition
2. SUB: Basic subtraction
3. MUL: Basic multiplication
4. DIV: Basic division
5. Division by zero: Error detection
6. CMP (true): 3 < 5 gives 1
7. CMP (false): 10 < 5 gives 0
8. Complex expression: (10 + 5) * 3 - 3 = 42
9. Negative result: 10 - 15 = -5
10. Integer division: 10 / 3 = 3 (truncated)

All tests should pass!

---

## Common Mistakes

### 1. Wrong Operand Order

**Wrong thinking**: "SUB pops two values, so b - a"
**Right thinking**: "First pop is second operand, second pop is first operand"

If you get -7 instead of 7 for `PUSH 10, PUSH 3, SUB`, you have the operand order backwards.

### 2. Forgetting Error Checks Between Pops

Each pop can fail if the stack is empty. Check after EACH pop:

```c
// BAD - might use garbage if first pop fails
int32_t b = stack_pop(vm);
int32_t a = stack_pop(vm);  // Runs even if first pop failed!

// GOOD - check after each operation
int32_t b = stack_pop(vm);
if (vm->error != VM_OK) return;  // Stop if error
int32_t a = stack_pop(vm);
if (vm->error != VM_OK) return;  // Stop if error
```

### 3. Not Checking for Division by Zero

Always check the divisor BEFORE doing the division:

```c
// BAD - undefined behavior if b is 0
int32_t result = a / b;

// GOOD - check first
if (b == 0) {
    vm->error = VM_ERROR_DIVISION_BY_ZERO;
    return;
}
int32_t result = a / b;
```

### 4. Integer Overflow

Our VM uses 32-bit signed integers. The maximum value is about 2 billion. If you add two large numbers, they might wrap around to a negative number. Our simple VM doesn't check for this - be aware it can happen!

---

## Files in This Day

| File | Purpose |
|------|---------|
| `instructions.h` | Same opcode definitions |
| `vm.h` | Same VM structure |
| `vm.c` | VM with arithmetic operations added |
| `main.c` | Tests for arithmetic operations |
| `Makefile` | Build instructions |
| `SUMMARY.md` | This explanation file |

---

## What's Next (Day 3)

Tomorrow we'll add memory operations:
- **STORE**: Save a value to a memory location
- **LOAD**: Retrieve a value from memory

This will let us store intermediate results, which is essential for more complex programs like loops.
