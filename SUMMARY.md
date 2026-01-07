# Day 7: Final Integration

## What We Built Today

Today we polished the VM codebase and prepared it for final integration with Student 2's assembler. This involved cleaning up code, improving error messages, creating proper CLI handling, and ensuring everything works together seamlessly.

The VM is now complete and ready to run any bytecode file produced by the assembler!

---

## Key Concepts Explained

### 1. Code Organization

A well-organized project has:

**Header files (.h)**: Declarations only
- What functions exist
- What structures look like
- What constants are defined
- NO implementation code

**Source files (.c)**: Implementation
- How functions actually work
- The real code that does things

**Why separate them?**
1. Headers can be included by multiple files
2. Changes to implementation don't require recompiling everything
3. Clearer separation of interface vs implementation

### 2. The Build Process

When you run `make`, here's what happens:

```
Step 1: Compile each .c file into .o (object) files
   gcc -c main.c      → main.o
   gcc -c vm.c        → vm.o
   gcc -c bytecode_loader.c → bytecode_loader.o

Step 2: Link all .o files into one executable
   gcc main.o vm.o bytecode_loader.o → vm
```

Object files (.o) contain machine code but aren't executable on their own - they need to be linked together.

### 3. Error Handling Strategy

Our VM has a consistent error handling pattern:

```c
VMError result = do_something();
if (result != VM_OK) {
    // Handle error
    cleanup();
    return result;
}
```

Key principles:
- Every function that can fail returns an error code
- Check errors immediately after each call
- Clean up resources before returning on error
- Never ignore return values

### 4. Command-Line Interface (CLI)

The VM accepts command-line arguments:

```bash
./vm program.bc      # Run a bytecode file
./vm --help          # Show help message
./vm                 # Show error (no file specified)
```

How it works:
```c
int main(int argc, char *argv[]) {
    // argc = number of arguments (including program name)
    // argv[0] = program name ("./vm")
    // argv[1] = first argument ("program.bc")

    if (argc < 2) {
        // No arguments provided
    }
}
```

### 5. Memory Management Best Practices

Our VM follows these rules:

1. **Allocate early, free late**: Create resources at start, destroy at end
2. **Pair alloc/free**: Every `malloc` has a matching `free`
3. **Check allocations**: Always verify malloc succeeded
4. **Null after free**: Set pointers to NULL after freeing
5. **Free in reverse order**: Free in opposite order of allocation

Example from `vm_create()` and `vm_destroy()`:
```c
// Create in order: vm → stack → memory → return_stack
vm = malloc(...);
vm->stack = malloc(...);
vm->memory = malloc(...);
vm->return_stack = malloc(...);

// Destroy in reverse: return_stack → memory → stack → vm
free(vm->return_stack);
free(vm->memory);
free(vm->stack);
free(vm);
```

---

## Complete VM Architecture

### File Structure
```
day7/
├── instructions.h     # Opcode definitions (shared with assembler)
├── vm.h              # VM structure and function declarations
├── vm.c              # VM implementation (all instructions)
├── bytecode_loader.h # File loading declarations
├── bytecode_loader.c # File loading implementation
├── main.c            # CLI entry point
├── Makefile          # Build configuration
└── SUMMARY.md        # This file
```

### Data Flow
```
Bytecode File (.bc)
       ↓
vm_load_bytecode_file()  ← Validates format, loads code
       ↓
   VM Structure          ← Code stored in vm->code
       ↓
    vm_run()             ← Execute instructions
       ↓
execute_instruction()    ← Fetch, decode, execute loop
       ↓
  Stack/Memory           ← Results stored here
       ↓
    Result               ← Top of stack is the answer
```

### Instruction Categories

| Category | Instructions | Purpose |
|----------|-------------|---------|
| Stack | PUSH, POP, DUP | Manipulate the data stack |
| Arithmetic | ADD, SUB, MUL, DIV, CMP | Perform calculations |
| Memory | STORE, LOAD | Access global variables |
| Control | JMP, JZ, JNZ | Change execution flow |
| Functions | CALL, RET | Implement subroutines |
| Special | HALT | Stop execution |

---

## How to Test

### Build the VM
```bash
make clean
make
```

### Test with the assembler (once Student 2 is done)
```bash
# Student 2 creates bytecode:
../student2/day7/asm program.asm -o program.bc

# Student 1's VM runs it:
./vm program.bc
```

### Check for memory leaks (optional but recommended)
```bash
valgrind --leak-check=full ./vm program.bc
```

Expected output for clean code:
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: X allocs, X frees, Y bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
```

---

## What the VM Can Do Now

1. **Load bytecode files** with validation
2. **Execute all 16 instructions**:
   - PUSH, POP, DUP
   - ADD, SUB, MUL, DIV, CMP
   - JMP, JZ, JNZ
   - STORE, LOAD
   - CALL, RET
   - HALT
3. **Handle errors gracefully** with clear messages
4. **Show execution results** (top of stack, memory, etc.)
5. **Clean up properly** (no memory leaks)

---

## Integration with Student 2's Assembler

The assembler produces bytecode files that the VM can execute:

```
Source Code (.asm)          Bytecode (.bc)              Execution
─────────────────           ──────────────              ─────────
  PUSH 10              →    BE BA FE CA     →           Stack: [10]
  PUSH 20                   01 00 00 00                 Stack: [10, 20]
  ADD                       0C 00 00 00                 Stack: [30]
  HALT                      01 0A 00 00 00             Result: 30
                            01 14 00 00 00
                            10
                            FF
```

The magic number `0xCAFEBABE` (stored as `BE BA FE CA` in little-endian) identifies valid bytecode files.

---

## Common Issues and Solutions

### "Cannot open file"
- Check the file path is correct
- Ensure the file exists
- Check file permissions

### "Invalid bytecode file (bad magic number)"
- File is not a valid bytecode file
- File was corrupted
- File was created by a different/older assembler

### "Invalid opcode"
- Bytecode contains unknown instruction
- File is corrupted or truncated
- Assembler and VM use different instruction sets

### "Stack underflow"
- Program tried to pop from empty stack
- Check the assembly code for missing PUSH

### "Division by zero"
- DIV instruction with 0 on top of stack
- Add a check before dividing in your assembly code

---

## Files Summary

| File | Lines | Purpose |
|------|-------|---------|
| `instructions.h` | ~60 | Shared opcode definitions |
| `vm.h` | ~90 | VM structure and declarations |
| `vm.c` | ~310 | Complete VM implementation |
| `bytecode_loader.h` | ~55 | File format constants |
| `bytecode_loader.c` | ~110 | File loading code |
| `main.c` | ~90 | CLI entry point |
| **Total** | **~715** | Complete VM codebase |

---

## What's Next

The VM is complete! Student 2 needs to finish the assembler, then we can:

1. Write assembly programs (.asm files)
2. Assemble them to bytecode (.bc files)
3. Run them on the VM
4. Verify the results are correct

The final integration in the `final/` folder will combine both projects with test programs and benchmarks.

---

## Key Takeaways from Building the VM

1. **Start simple**: Day 1 was just PUSH, POP, DUP, HALT
2. **Add incrementally**: Each day added one category of instructions
3. **Test constantly**: Every new feature was tested before moving on
4. **Handle errors**: Never assume things will work - check everything
5. **Document as you go**: Comments and summaries help understanding
6. **Design for integration**: The shared `instructions.h` ensures compatibility
