# Day 4: Code Generator

## What We Built Today

Today we completed the assembler by adding the **code generator** - the final stage that takes parsed instructions and generates actual bytecode that the VM can execute.

We can now:
1. Read assembly source code
2. Tokenize it
3. Parse it
4. Resolve labels
5. Generate bytecode
6. Write it to a `.bc` file

---

## Key Concepts Explained

### 1. What is Code Generation?

Code generation is the final phase of compilation/assembly. It takes the abstract representation (parsed instructions) and produces the concrete output (bytecode bytes).

**Input**: Parsed instructions with resolved addresses
```
Instruction { opcode=0x01, operand=42 }  // PUSH 42
Instruction { opcode=0x10 }               // ADD
```

**Output**: Raw bytes
```
01 2A 00 00 00 10
```

### 2. The Bytecode File Format

Our bytecode files have this structure:

```
+------------------------+
| Header (12 bytes)      |
|------------------------|
| Magic:   0xCAFEBABE    |  4 bytes, little-endian
| Version: 0x00000001    |  4 bytes, little-endian
| Size:    N             |  4 bytes, little-endian
+------------------------+
| Code (N bytes)         |
|------------------------|
| opcode1 [operand1]     |
| opcode2 [operand2]     |
| ...                    |
+------------------------+
```

### 3. Little-Endian Encoding

Multi-byte values are stored with the least significant byte first:

**Value**: `0x12345678`

**Little-endian**: `78 56 34 12`

This matches x86/x64 processors and is what our VM expects.

```c
// Write a 32-bit value in little-endian
bytes[0] = value & 0xFF;         // Least significant
bytes[1] = (value >> 8) & 0xFF;
bytes[2] = (value >> 16) & 0xFF;
bytes[3] = (value >> 24) & 0xFF; // Most significant
```

### 4. Instruction Encoding

Each instruction becomes 1 or 5 bytes:

**No operand** (1 byte):
```
ADD → 10
```

**With operand** (5 bytes):
```
PUSH 42 → 01 2A 00 00 00
           │  └──────────── 42 in little-endian (0x0000002A)
           └─────────────── opcode for PUSH
```

### 5. Binary File I/O

Writing binary files in C:

```c
// Open in binary write mode
FILE *file = fopen("output.bc", "wb");

// Write raw bytes
uint8_t data[] = {0x01, 0x02, 0x03};
fwrite(data, 1, 3, file);

// Always close
fclose(file);
```

The `"wb"` mode is crucial:
- `w` = write (create/overwrite)
- `b` = binary (don't translate line endings)

---

## How the Code Works

### Generating Bytecode

```c
bool codegen_generate(CodeGenerator *gen, ParsedInstruction *instructions,
                      int instruction_count) {
    for (each instruction) {
        // Emit the opcode
        emit_byte(gen, inst->opcode);

        // Emit operand if present
        if (inst->has_operand) {
            emit_int32(gen, inst->operand);
        }
    }
}
```

### Emitting Little-Endian Values

```c
static bool emit_int32(CodeGenerator *gen, int32_t value) {
    emit_byte(gen, value & 0xFF);
    emit_byte(gen, (value >> 8) & 0xFF);
    emit_byte(gen, (value >> 16) & 0xFF);
    emit_byte(gen, (value >> 24) & 0xFF);
}
```

### Writing the File

```c
bool codegen_write_file(CodeGenerator *gen, const char *filename) {
    FILE *file = fopen(filename, "wb");

    // Write header
    write_uint32(file, BYTECODE_MAGIC);    // 0xCAFEBABE
    write_uint32(file, BYTECODE_VERSION);  // 0x00000001
    write_uint32(file, gen->bytecode_size);

    // Write code
    fwrite(gen->bytecode, 1, gen->bytecode_size, file);

    fclose(file);
}
```

---

## How to Test

### Compile
```bash
make clean
make
```

### Run Tests (generates bytecode files)
```bash
./asm_test
```

### View Generated Files
```bash
hexdump -C test_add.bc
```

### Run on VM (if available)
```bash
../../student1/day7/vm test_add.bc
```

---

## Expected Output

```
========================================
  Assembler Code Generator - Day 4 Tests
========================================

=== Assembling: Simple addition ===
Source:
PUSH 40
PUSH 2
ADD
HALT

=== Bytecode (12 bytes) ===
Header:
  Magic:   0xCAFEBABE
  Version: 0x00000001
  Size:    12 bytes

Code (hex):
  0000: 01 28 00 00 00 01 02 00 00 00 10 FF
==========================
Wrote 12 bytes to 'test_add.bc' (+ 12 byte header)
Assembly successful!

...

========================================
  All bytecode files generated!
========================================

Generated files:
  test_add.bc     - Simple addition (40 + 2 = 42)
  test_expr.bc    - Expression ((5 + 3) * 2 = 16)
  test_loop.bc    - Loop (count from 3 to 0)
  test_memory.bc  - Memory (100 + 200 = 300)
  test_func.bc    - Function (10 * 2 = 20)
  test_cond.bc    - Conditional (0 → 200)

Run with VM: ../student1/day7/vm test_add.bc
```

---

## Bytecode Breakdown: test_add.bc

Source:
```asm
PUSH 40
PUSH 2
ADD
HALT
```

Bytecode (hex dump):
```
00000000: BE BA FE CA  01 00 00 00  0C 00 00 00  01 28 00 00  ...............
00000010: 00 01 02 00  00 00 10 FF                            ........
```

Breakdown:
```
BE BA FE CA     Magic number (0xCAFEBABE in little-endian)
01 00 00 00     Version (1)
0C 00 00 00     Code size (12 bytes)
01 28 00 00 00  PUSH 40 (opcode 0x01, operand 40 = 0x28)
01 02 00 00 00  PUSH 2  (opcode 0x01, operand 2)
10              ADD     (opcode 0x10)
FF              HALT    (opcode 0xFF)
```

---

## The Complete Assembly Pipeline

```
Source Code (.asm)
       │
       ▼
   [Lexer]  ──────────────►  Tokens
       │
       ▼
   [Parser] ──────────────►  Parsed Instructions
       │
       ▼
[Label Collector] ────────►  Symbol Table
       │
       ▼
[Label Resolver] ─────────►  Resolved Instructions
       │
       ▼
[Code Generator] ─────────►  Bytecode
       │
       ▼
[File Writer] ────────────►  .bc File
```

---

## Common Mistakes and How to Avoid Them

### 1. Forgetting Binary Mode
```c
// WRONG - text mode, might corrupt on Windows
FILE *file = fopen(filename, "w");

// CORRECT - binary mode
FILE *file = fopen(filename, "wb");
```

### 2. Wrong Byte Order
```c
// WRONG - big-endian
bytes[0] = (value >> 24) & 0xFF;

// CORRECT - little-endian
bytes[0] = value & 0xFF;
```

### 3. Not Checking Write Errors
```c
// WRONG - assumes write succeeds
fwrite(data, 1, size, file);

// CORRECT - check return value
if (fwrite(data, 1, size, file) != size) {
    // Handle error
}
```

### 4. Unresolved Labels
The code generator should never see unresolved label references. If it does, something went wrong in an earlier stage:
```c
if (inst->is_label_ref) {
    // This is a bug - labels should be resolved already
    error("Unresolved label");
}
```

---

## What's Next?

Tomorrow (Day 5) we'll:
1. Add proper error handling throughout
2. Create a command-line interface (CLI)
3. Make the assembler a proper standalone tool

---

## Files in This Day

| File | Lines | Purpose |
|------|-------|---------|
| `instructions.h` | ~60 | Opcode definitions |
| `lexer.h/.c` | ~270 | Tokenization |
| `parser.h/.c` | ~250 | Parsing |
| `labels.h/.c` | ~190 | Label resolution |
| `codegen.h` | ~45 | Code generator interface |
| `codegen.c` | ~120 | Code generation |
| `main.c` | ~140 | Test program |
| `Makefile` | ~45 | Build configuration |

---

## Key Takeaways

1. **Code generation is the final step** - turns abstract representation into bytes
2. **Little-endian matters** - must match what the VM expects
3. **File format includes header** - magic number, version, size
4. **Binary mode is essential** - prevents line ending translation
5. **Always verify output** - use hexdump to check generated files
