# Day 2: Parser + Opcode Table

## What We Built Today

Today we created the **parser** - the second stage of our assembler. The parser takes tokens from the lexer and converts them into **parsed instructions** with their opcodes and operands.

We also created the **opcode table** - a lookup table that maps instruction names like "PUSH" to their numeric opcodes like `0x01`.

---

## Key Concepts Explained

### 1. What is a Parser?

A parser takes tokens and builds a structured representation of the program. In our case:

**Input (Tokens)**:
```
INSTRUCTION("PUSH") → NUMBER(42) → NEWLINE → INSTRUCTION("ADD") → NEWLINE
```

**Output (Parsed Instructions)**:
```
Instruction { opcode=0x01, operand=42, has_operand=true }
Instruction { opcode=0x10, operand=0, has_operand=false }
```

### 2. The Opcode Table

The opcode table is a simple lookup table:

| Name | Opcode | Has Operand? |
|------|--------|--------------|
| PUSH | 0x01 | Yes |
| POP | 0x02 | No |
| ADD | 0x10 | No |
| JMP | 0x20 | Yes |
| HALT | 0xFF | No |

In C:
```c
static const OpcodeEntry opcode_table[] = {
    {"PUSH", OP_PUSH, true},
    {"POP",  OP_POP,  false},
    {"ADD",  OP_ADD,  false},
    {"JMP",  OP_JMP,  true},
    {"HALT", OP_HALT, false},
    {NULL, 0, false}  // End marker
};
```

### 3. Parsing Strategy

Our parser uses a simple approach:

```
1. Skip any newlines
2. Look at current token
3. If it's a label definition, skip it (handled in Day 3)
4. If it's an instruction:
   a. Look up opcode in the table
   b. If instruction has operand, read the next token
   c. Add to instruction list
5. Repeat until EOF
```

### 4. Case Insensitivity

Assembly is traditionally case-insensitive. These are all equivalent:
- `PUSH 42`
- `push 42`
- `Push 42`

We use a case-insensitive string comparison:
```c
static int strcasecmp_local(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (toupper(*s1) != toupper(*s2)) return 1;
        s1++;
        s2++;
    }
    return *s1 - *s2;
}
```

### 5. Label References

When we see something like `JMP loop`, the operand is a label name, not a number. We mark it as a label reference:

```c
if (operand->type == TOKEN_INSTRUCTION) {
    inst.is_label_ref = true;
    strcpy(inst.label_name, operand->text);
}
```

The actual address will be resolved in Day 3 (label resolution).

---

## How the Code Works

### The ParsedInstruction Structure

```c
typedef struct {
    uint8_t opcode;        // The opcode (e.g., 0x01 for PUSH)
    bool has_operand;      // Does this instruction take an operand?
    int32_t operand;       // The numeric operand (if any)
    bool is_label_ref;     // Is the operand a label reference?
    char label_name[64];   // Label name (if is_label_ref is true)
    int line;              // Source line number for errors
} ParsedInstruction;
```

### The Parsing Loop

```c
bool parser_parse(Parser *parser) {
    while (!is_at_end(parser)) {
        skip_newlines(parser);
        Token *token = current(parser);

        // Skip label definitions
        if (token->type == TOKEN_LABEL_DEF) {
            advance(parser);
            continue;
        }

        // Must be an instruction
        const OpcodeEntry *entry = lookup_opcode(token->text);
        if (!entry) {
            // Error: unknown instruction
        }

        ParsedInstruction inst;
        inst.opcode = entry->opcode;
        inst.has_operand = entry->has_operand;
        advance(parser);

        // Read operand if needed
        if (entry->has_operand) {
            Token *operand = current(parser);
            if (operand->type == TOKEN_NUMBER) {
                inst.operand = operand->value;
            } else {
                // Assume it's a label reference
                inst.is_label_ref = true;
                strcpy(inst.label_name, operand->text);
            }
            advance(parser);
        }

        add_instruction(parser, &inst);
    }
    return true;
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
./parser_test
```

Or simply:
```bash
make test
```

---

## Expected Output

```
========================================
  Assembler Parser - Day 2 Tests
========================================

=== Test: Simple arithmetic ===
Source:
PUSH 10
PUSH 20
ADD
HALT

Tokens: 9
=== Parsed Instructions (4 total) ===
[  0] Line  1: opcode=0x01 operand=10
[  1] Line  2: opcode=0x01 operand=20
[  2] Line  3: opcode=0x10
[  3] Line  4: opcode=0xFF
======================================
Parsing successful!

...

=== Test: Label references ===
Source:
start:
    PUSH 5
    JNZ start
end:
    HALT

Tokens: 9
=== Parsed Instructions (3 total) ===
[  0] Line  2: opcode=0x01 operand=5
[  1] Line  3: opcode=0x22 operand=<start>
[  2] Line  5: opcode=0xFF
======================================
Parsing successful!

=== Test: Error - missing operand ===
Source:
PUSH

Expected error: Line 1: PUSH requires an operand

=== Test: Error - unknown instruction ===
Source:
UNKNOWN 42

Expected error: Line 1: Unknown instruction 'UNKNOWN'

========================================
  All parser tests completed!
========================================
```

---

## Instruction Categories

### Stack Operations
| Instruction | Opcode | Operand | Effect |
|-------------|--------|---------|--------|
| PUSH val | 0x01 | 4 bytes | Push value onto stack |
| POP | 0x02 | None | Remove top of stack |
| DUP | 0x03 | None | Duplicate top of stack |

### Arithmetic
| Instruction | Opcode | Operand | Effect |
|-------------|--------|---------|--------|
| ADD | 0x10 | None | a, b → a+b |
| SUB | 0x11 | None | a, b → a-b |
| MUL | 0x12 | None | a, b → a*b |
| DIV | 0x13 | None | a, b → a/b |
| CMP | 0x14 | None | a, b → (a<b ? 1 : 0) |

### Control Flow
| Instruction | Opcode | Operand | Effect |
|-------------|--------|---------|--------|
| JMP addr | 0x20 | 4 bytes | Jump to address |
| JZ addr | 0x21 | 4 bytes | Jump if zero |
| JNZ addr | 0x22 | 4 bytes | Jump if not zero |

### Memory
| Instruction | Opcode | Operand | Effect |
|-------------|--------|---------|--------|
| STORE idx | 0x30 | 4 bytes | Store to memory[idx] |
| LOAD idx | 0x31 | 4 bytes | Load from memory[idx] |

### Functions
| Instruction | Opcode | Operand | Effect |
|-------------|--------|---------|--------|
| CALL addr | 0x40 | 4 bytes | Call function |
| RET | 0x41 | None | Return from function |

### Special
| Instruction | Opcode | Operand | Effect |
|-------------|--------|---------|--------|
| HALT | 0xFF | None | Stop execution |

---

## Common Mistakes and How to Avoid Them

### 1. Forgetting Case Insensitivity
```c
// WRONG - only matches uppercase
if (strcmp(name, "PUSH") == 0)

// CORRECT - case-insensitive
if (strcasecmp(name, "PUSH") == 0)
```

### 2. Not Handling Missing Operands
```c
// WRONG - crashes if no operand
Token *operand = current(parser);
inst.operand = operand->value;

// CORRECT - check first
if (is_at_end(parser) || current(parser)->type == TOKEN_NEWLINE) {
    // Error: missing operand
}
```

### 3. Not Distinguishing Labels from Instructions
When `JMP loop` is parsed, "loop" looks like a TOKEN_INSTRUCTION but it's actually a label reference. We mark it:
```c
if (operand->type == TOKEN_INSTRUCTION) {
    inst.is_label_ref = true;
}
```

---

## What's Next?

Tomorrow (Day 3) we'll implement **label resolution**:
1. Collect all label definitions and their addresses
2. Replace label references with actual addresses
3. This is called "two-pass assembly"

---

## Files in This Day

| File | Lines | Purpose |
|------|-------|---------|
| `instructions.h` | ~60 | Opcode definitions |
| `lexer.h` | ~80 | Token types |
| `lexer.c` | ~190 | Tokenization |
| `parser.h` | ~70 | Parsed instruction structure |
| `parser.c` | ~180 | Parsing implementation |
| `main.c` | ~130 | Test program |
| `Makefile` | ~35 | Build configuration |

---

## Key Takeaways

1. **Parsers build structure from tokens** - turning a stream into meaningful data
2. **Lookup tables are efficient** - O(n) but n is small (16 instructions)
3. **Case insensitivity matters** - assembly tradition
4. **Separate concerns** - parsing doesn't resolve labels, just marks them
5. **Good errors help users** - include line numbers and clear messages
