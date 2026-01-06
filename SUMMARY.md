# Day 3: Label Resolution (Two-Pass Assembly)

## What We Built Today

Today we implemented **two-pass assembly** to resolve labels. This allows programmers to use symbolic names like `loop:` and `JMP loop` instead of raw numeric addresses.

- **Pass 1**: Scan through the code, collect all label definitions, and compute their addresses
- **Pass 2**: Replace all label references with their actual addresses

---

## Key Concepts Explained

### 1. Why Two Passes?

Consider this code:
```asm
    JMP end      ; Jump to 'end' - but where is it?
    PUSH 42
end:
    HALT
```

When we see `JMP end`, we don't know where `end` is yet - it's defined later (forward reference). We need two passes:

1. **Pass 1**: Find all labels and their addresses
2. **Pass 2**: Replace label names with addresses

### 2. Address Calculation

Each instruction takes a certain number of bytes:

| Instruction Type | Size |
|-----------------|------|
| No operand (ADD, POP, etc.) | 1 byte |
| With operand (PUSH, JMP, etc.) | 5 bytes |

To find a label's address, sum the sizes of all instructions before it:

```
Address 0:  PUSH 5      (5 bytes) → next at 5
Address 5:  PUSH 1      (5 bytes) → next at 10
Address 10: ADD         (1 byte)  → next at 11
Address 11: loop:                  → label at 11
Address 11: DUP         (1 byte)  → next at 12
...
```

### 3. The Symbol Table

A symbol table maps names to addresses:

```
Symbol Table:
  loop    = 11
  end     = 20
  double  = 25
```

In C:
```c
typedef struct {
    char name[64];     // Label name
    int32_t address;   // Bytecode address
    int line;          // Line number (for errors)
} LabelEntry;
```

### 4. Forward and Backward References

**Forward reference**: Using a label before it's defined
```asm
    JMP end      ; Forward: 'end' defined later
    PUSH 1
end:
    HALT
```

**Backward reference**: Using a label after it's defined
```asm
loop:
    PUSH 1
    JMP loop     ; Backward: 'loop' already defined
```

Two-pass assembly handles both correctly!

### 5. Error Detection

We catch two kinds of label errors:

**Undefined label**: Using a label that doesn't exist
```asm
    JMP nowhere   ; Error: 'nowhere' is never defined
    HALT
```

**Duplicate label**: Defining the same label twice
```asm
start:
    PUSH 1
start:            ; Error: 'start' already defined
    HALT
```

---

## How the Code Works

### Pass 1: Collecting Labels

```c
bool symtab_collect_labels(...) {
    int current_address = 0;
    int instruction_index = 0;

    for (each token) {
        if (token is LABEL_DEF) {
            // Record: this label = current_address
            add_label(table, token->text, current_address);
        }
        else if (token is INSTRUCTION) {
            // Move address forward by instruction size
            current_address += instruction_size(instructions[instruction_index]);
            instruction_index++;
        }
    }
}
```

### Pass 2: Resolving References

```c
bool symtab_resolve_labels(...) {
    for (each instruction) {
        if (instruction.is_label_ref) {
            // Look up the label
            LabelEntry *entry = symtab_lookup(instruction.label_name);
            if (!entry) {
                // Error: undefined label
            }
            // Replace with address
            instruction.operand = entry->address;
            instruction.is_label_ref = false;
        }
    }
}
```

---

## How to Test

### Compile
```bash
make clean
make
```

### Run Tests
```bash
./labels_test
```

Or:
```bash
make test
```

---

## Expected Output

```
========================================
  Assembler Labels - Day 3 Tests
========================================

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
================================

=== Resolved Instructions ===
[  0] addr=  0: opcode=0x01 operand=1 (0x0001)
[  1] addr=  5: opcode=0x11
[  2] addr=  6: opcode=0x03
[  3] addr=  7: opcode=0x22 operand=0 (0x0000)
[  4] addr= 12: opcode=0xFF
=============================
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
================================

=== Resolved Instructions ===
[  0] addr=  0: opcode=0x01 operand=10 (0x000A)
[  1] addr=  5: opcode=0x40 operand=11 (0x000B)
[  2] addr= 10: opcode=0xFF
[  3] addr= 11: opcode=0x03
[  4] addr= 12: opcode=0x10
[  5] addr= 13: opcode=0x41
=============================
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

========================================
  All label tests completed!
========================================
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

---

## Common Mistakes and How to Avoid Them

### 1. Off-by-One Errors in Address Calculation
```c
// WRONG - instruction size calculated incorrectly
address += inst->has_operand ? 4 : 1;  // Missing opcode byte!

// CORRECT
address += inst->has_operand ? 5 : 1;  // 1 opcode + 4 operand
```

### 2. Case Sensitivity in Label Lookup
```c
// WRONG - won't match "Loop" with "loop"
if (strcmp(entry->name, name) == 0)

// CORRECT - case-insensitive
if (strcasecmp(entry->name, name) == 0)
```

### 3. Not Checking for Duplicates
```c
// WRONG - silently overwrites
labels[count] = new_label;
count++;

// CORRECT - check first
if (symtab_lookup(table, name)) {
    // Error: duplicate label
}
```

### 4. Processing Operands as Instructions
```c
// WRONG - counts NUMBER tokens as instructions
if (token->type == TOKEN_INSTRUCTION || token->type == TOKEN_NUMBER) {
    address += instruction_size(...);
}

// CORRECT - only count actual instructions
if (token->type == TOKEN_INSTRUCTION) {
    address += instruction_size(instructions[instruction_index]);
    instruction_index++;
}
```

---

## What's Next?

Tomorrow (Day 4) we'll implement the **code generator** that:
1. Takes resolved instructions
2. Writes the bytecode file header
3. Encodes each instruction as bytes
4. Creates the final `.bc` file

---

## Files in This Day

| File | Lines | Purpose |
|------|-------|---------|
| `instructions.h` | ~60 | Opcode definitions |
| `lexer.h/.c` | ~270 | Tokenization |
| `parser.h/.c` | ~250 | Parsing |
| `labels.h` | ~60 | Symbol table definitions |
| `labels.c` | ~130 | Two-pass implementation |
| `main.c` | ~160 | Test program |
| `Makefile` | ~40 | Build configuration |

---

## Key Takeaways

1. **Two-pass assembly solves forward references** - can't know addresses until you've seen everything
2. **Symbol tables map names to values** - fundamental data structure in compilers
3. **Address calculation requires knowing instruction sizes** - every byte counts
4. **Good error messages include context** - "undefined label X on line Y"
5. **Case insensitivity is a design choice** - we chose to match assembly conventions
