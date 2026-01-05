# Day 3: Memory Operations

## What We Built Today

Today we added memory capabilities to our VM:
1. **STORE** - Save a value from the stack to memory
2. **LOAD** - Retrieve a value from memory to the stack

With memory, we can now store intermediate results and implement variables - essential for loops and complex programs!

---

## Key Concepts Explained

### Why Do We Need Memory?

The stack is great for calculations, but it has a limitation: once you pop a value, it's gone. What if you need to use a value multiple times, or save it for later?

**Memory** is like a set of numbered boxes where we can store values:
- Each box has an **index** (0, 1, 2, ..., 255)
- Each box can hold one 32-bit integer
- Values stay in memory until we overwrite them

```
Memory visualization:

Index:  [0]   [1]   [2]   [3]   ...  [255]
Value:  [42]  [0]   [100] [0]   ...  [0]
```

### Stack vs Memory

| Feature | Stack | Memory |
|---------|-------|--------|
| Access | Only top element | Any index |
| Size | Dynamic (push/pop) | Fixed (256 cells) |
| Persistence | Pop removes value | Stays until overwritten |
| Use case | Calculations | Variables, storage |

### STORE Instruction

**Opcode**: 0x30
**Format**: 1 byte opcode + 4 bytes index (5 bytes total)

**What it does**:
1. Read the memory index from bytecode
2. Pop a value from the stack
3. Store that value at memory[index]

```
Before STORE 0:
    Stack: [42]     Memory[0]: 0

After STORE 0:
    Stack: []       Memory[0]: 42
```

**Important**: STORE consumes (pops) the value from the stack!

### LOAD Instruction

**Opcode**: 0x31
**Format**: 1 byte opcode + 4 bytes index (5 bytes total)

**What it does**:
1. Read the memory index from bytecode
2. Read the value from memory[index]
3. Push that value onto the stack

```
Before LOAD 0:
    Stack: []       Memory[0]: 42

After LOAD 0:
    Stack: [42]     Memory[0]: 42  (unchanged)
```

**Important**: LOAD doesn't remove the value from memory - you can load the same memory location multiple times!

---

## How the Code Works

### STORE Implementation

```c
case OP_STORE: {
    /* Read the memory index from bytecode */
    int32_t index = read_int32(vm);
    if (vm->error != VM_OK) { vm->running = false; return; }

    /* Check bounds - is the index valid? */
    if (index < 0 || index >= MEMORY_SIZE) {
        vm->error = VM_ERROR_MEMORY_BOUNDS;
        vm->running = false;
        return;
    }

    /* Pop value from stack */
    int32_t value = stack_pop(vm);
    if (vm->error != VM_OK) { vm->running = false; return; }

    /* Store in memory */
    vm->memory[index] = value;
    break;
}
```

### LOAD Implementation

```c
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
```

### Memory Bounds Checking

Why do we check bounds?

```c
if (index < 0 || index >= MEMORY_SIZE)
```

Without this check, if someone tries `LOAD 1000`:
- We'd read beyond our allocated array
- Could crash or return garbage
- Security vulnerability!

With bounds checking, we report a clean error.

---

## Practical Examples

### Example 1: Using Memory as a Variable

Calculate `x = 5; y = x + 3; result = y * 2`

```assembly
; x = 5
PUSH 5
STORE 0     ; M[0] = x = 5

; y = x + 3
LOAD 0      ; push x (5)
PUSH 3
ADD         ; 5 + 3 = 8
STORE 1     ; M[1] = y = 8

; result = y * 2
LOAD 1      ; push y (8)
PUSH 2
MUL         ; 8 * 2 = 16
STORE 2     ; M[2] = result = 16

; Push final result
LOAD 2
HALT        ; Stack: [16]
```

### Example 2: Accumulator Pattern

Sum numbers using a memory location as accumulator:

```assembly
; sum = 0
PUSH 0
STORE 0

; sum += 10
LOAD 0      ; load current sum
PUSH 10
ADD
STORE 0     ; save new sum

; sum += 20
LOAD 0
PUSH 20
ADD
STORE 0

; sum += 30
LOAD 0
PUSH 30
ADD
STORE 0

; Result
LOAD 0      ; Stack: [60]
HALT
```

This pattern is the foundation for loops (Day 4)!

### Example 3: Swapping Two Values

Swap M[0] and M[1] using M[2] as temporary storage:

```assembly
; Initial: M[0] = 5, M[1] = 10
PUSH 5
STORE 0
PUSH 10
STORE 1

; Swap using temp
LOAD 0      ; temp = M[0]
STORE 2     ; M[2] = temp (5)
LOAD 1      ; get M[1]
STORE 0     ; M[0] = M[1] (10)
LOAD 2      ; get temp
STORE 1     ; M[1] = temp (5)

; Now: M[0] = 10, M[1] = 5
HALT
```

---

## Memory Initialization

When the VM starts (or loads a new program):
- All 256 memory cells are set to 0
- This is done in `vm_load_program()`

```c
memset(vm->memory, 0, MEMORY_SIZE * sizeof(int32_t));
```

This means:
- You can safely LOAD from any address (you'll get 0)
- You don't need to initialize variables to 0 first

---

## How to Test It

### Building

```bash
cd student1/day3
make
```

### Running

```bash
./vm_test
```

### Expected Output

8 tests covering:
1. Basic STORE and LOAD
2. Multiple memory locations
3. Accumulator pattern (sum)
4. Last valid index (255)
5. STORE out of bounds error
6. LOAD out of bounds error
7. Memory initialized to zero
8. Swap using memory

All tests should pass!

---

## Common Mistakes

### 1. Forgetting STORE Pops the Stack

```assembly
PUSH 42
STORE 0     ; Stack is now EMPTY!
ADD         ; ERROR: Stack underflow!
```

After STORE, the value is gone from the stack. If you need it again, LOAD it back.

### 2. Wrong Index Encoding

Memory index is 4 bytes in little-endian:
- Index 0: `00 00 00 00`
- Index 1: `01 00 00 00`
- Index 255: `FF 00 00 00`
- Index 256: `00 01 00 00` (out of bounds!)

### 3. Not Checking for Errors

Both STORE and LOAD can fail:
- STORE needs a value on the stack (underflow possible)
- Both can have invalid indices (bounds error)

### 4. Assuming Memory Persists Across Runs

When you load a new program, memory is reset to zeros. Don't assume values from a previous run are still there.

---

## Files in This Day

| File | Purpose |
|------|---------|
| `instructions.h` | Same opcode definitions |
| `vm.h` | Same VM structure |
| `vm.c` | VM with memory operations added |
| `main.c` | Tests for memory operations |
| `Makefile` | Build instructions |
| `SUMMARY.md` | This explanation file |

---

## What's Next (Day 4)

Tomorrow we'll add control flow:
- **JMP** - Unconditional jump (goto)
- **JZ** - Jump if zero (conditional)
- **JNZ** - Jump if not zero (conditional)

With memory + jumps, we can finally implement loops!
