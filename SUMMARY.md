<<<<<<< HEAD
# Day 2: Arithmetic Operations

## What We Built Today

Today we added arithmetic capabilities to our VM. We implemented five new instructions:
1. **ADD** - Add two numbers
2. **SUB** - Subtract two numbers
3. **MUL** - Multiply two numbers
4. **DIV** - Divide two numbers (with division-by-zero check)
5. **CMP** - Compare two numbers (less than)

Now our VM can do actual calculations, not just move numbers around!
=======
# Day 1: Lexer (Tokenizer)

## What We Built Today

Today we created the **lexer** (also called a tokenizer) - the first stage of our assembler. The lexer reads assembly source code as raw text and breaks it into **tokens** - the smallest meaningful pieces like instructions, numbers, and labels.

This is exactly how real compilers and assemblers work: first tokenize, then parse, then generate code.
>>>>>>> master

---

## Key Concepts Explained

<<<<<<< HEAD
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
=======
### 1. What is a Lexer?

A lexer (lexical analyzer) converts raw text into tokens. Think of it like breaking a sentence into words:

**Raw text**: `"PUSH 42  ; add value"`

**Tokens**:
1. INSTRUCTION: "PUSH"
2. NUMBER: "42" (value: 42)
3. NEWLINE

The comment `; add value` is ignored - it's not meaningful for the assembler.

### 2. Token Types

Our lexer recognizes these token types:

| Type | Example | Description |
|------|---------|-------------|
| TOKEN_INSTRUCTION | PUSH, ADD, HALT | An assembly instruction |
| TOKEN_NUMBER | 42, -7, 0 | A numeric value |
| TOKEN_LABEL_DEF | loop: | A label definition (note the colon) |
| TOKEN_LABEL_REF | loop | A reference to a label |
| TOKEN_NEWLINE | (end of line) | Separates instructions |
| TOKEN_EOF | (end of file) | Marks end of input |
| TOKEN_ERROR | | Something went wrong |

### 3. How Tokenization Works

The lexer uses a simple state machine:

```
1. Skip whitespace (spaces, tabs)
2. Look at current character
3. Based on what it is:
   - ';' → Skip rest of line (comment)
   - '\n' → Emit NEWLINE token
   - Digit or '-' → Read number
   - Letter → Read identifier (instruction or label)
   - Other → Error
4. Repeat until end of file
```

### 4. Why Separate Lexing from Parsing?

We could try to do everything in one pass, but separating has advantages:

1. **Simpler code**: Each stage does one thing well
2. **Better errors**: "Unknown character '#'" vs "Parse error"
3. **Reusable**: Same lexer could work with different parsers
4. **Easier testing**: Can test lexer independently

This is the classic "separation of concerns" principle.

### 5. Handling Different Input

The lexer handles various input patterns:

**Comments**: Start with `;`, continue to end of line
```asm
PUSH 42  ; this is ignored
```

**Labels**: End with `:`
```asm
loop:    ; defines label "loop"
JMP loop ; references label "loop"
```

**Numbers**: Digits, optionally starting with `-`
```asm
PUSH 42
PUSH -7
```

**Whitespace**: Spaces and tabs between tokens are ignored
```asm
   PUSH   42   ; all the same as "PUSH 42"
```
>>>>>>> master

---

## How the Code Works

<<<<<<< HEAD
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
=======
### lexer.h

Defines the structures we need:

```c
typedef enum {
    TOKEN_INSTRUCTION,
    TOKEN_NUMBER,
    TOKEN_LABEL_DEF,
    TOKEN_LABEL_REF,
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;              // What kind of token
    char text[MAX_TOKEN_LENGTH]; // The actual text
    int32_t value;               // Numeric value (for numbers)
    int line;                    // Line number (for errors)
} Token;
```

### lexer.c

The main function is `lexer_tokenize()`:

```c
bool lexer_tokenize(Lexer *lexer) {
    while (!is_at_end(lexer)) {
        skip_whitespace(lexer);

        char c = peek(lexer);

        if (c == ';') {
            skip_comment(lexer);
        }
        else if (c == '\n') {
            add_token(lexer, TOKEN_NEWLINE, "\\n", 0);
            lexer->line++;
        }
        else if (is_digit(c) || c == '-') {
            read_number(lexer);
        }
        else if (is_alpha(c)) {
            read_identifier(lexer);
        }
        else {
            // Error: unexpected character
        }
    }

    add_token(lexer, TOKEN_EOF, "EOF", 0);
    return true;
}
```

Key helper functions:

- `peek()` - Look at current character without consuming it
- `advance()` - Get current character and move to next
- `skip_whitespace()` - Skip spaces and tabs
- `skip_comment()` - Skip from `;` to end of line
- `read_number()` - Read a numeric token
- `read_identifier()` - Read an instruction or label
>>>>>>> master

---

## How to Test

### Compile
```bash
<<<<<<< HEAD
cd student1/day2
make
```

### Running

=======
make clean
make
```

### Run Tests
>>>>>>> master
```bash
./lexer_test
```

<<<<<<< HEAD
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
=======
Or simply:
```bash
make test
```

---

## Expected Output

```
========================================
  Assembler Lexer - Day 1 Tests
========================================

=== Test: Simple program ===
Source:
PUSH 42
PUSH 8
ADD
HALT

=== Tokens (9 total) ===
[  0] Line  1: INSTRUCTION     'PUSH'
[  1] Line  1: NUMBER          '42' (value: 42)
[  2] Line  1: NEWLINE         '\n'
[  3] Line  2: INSTRUCTION     'PUSH'
[  4] Line  2: NUMBER          '8' (value: 8)
[  5] Line  2: NEWLINE         '\n'
[  6] Line  3: INSTRUCTION     'ADD'
[  7] Line  3: NEWLINE         '\n'
[  8] Line  4: INSTRUCTION     'HALT'
========================
Tokenization successful!

...

=== Test: Labels ===
Source:
start:
    PUSH 5
    PUSH 1
    SUB
    DUP
    JNZ start
    HALT

=== Tokens (15 total) ===
[  0] Line  1: LABEL_DEF       'start'
[  1] Line  1: NEWLINE         '\n'
[  2] Line  2: INSTRUCTION     'PUSH'
[  3] Line  2: NUMBER          '5' (value: 5)
[  4] Line  2: NEWLINE         '\n'
...
========================
Tokenization successful!
```

---

## Common Mistakes and How to Avoid Them

### 1. Forgetting to Handle Negative Numbers
```c
// WRONG - doesn't handle "-42"
if (is_digit(c)) {
    read_number(lexer);
}

// CORRECT - check for minus sign too
if (is_digit(c) || (c == '-' && is_digit(next_char))) {
    read_number(lexer);
}
```

### 2. Not Tracking Line Numbers
Line numbers are crucial for error messages. Always increment `lexer->line` when you see a newline:
```c
if (c == '\n') {
    lexer->line++;
}
```

### 3. Buffer Overflows
Always check length before copying strings:
```c
if (length >= MAX_TOKEN_LENGTH) {
    // Error: identifier too long
}
strncpy(token->text, source, length);
token->text[length] = '\0';  // Always null-terminate!
```

### 4. Not Null-Terminating Strings
C strings must end with `\0`. Always add it explicitly:
```c
strncpy(text, source, length);
text[length] = '\0';  // IMPORTANT!
```
>>>>>>> master

---

## What's Next?

<<<<<<< HEAD
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
=======
Tomorrow (Day 2) we'll build the **parser** which takes our tokens and:
1. Validates the syntax (is "PUSH 42" valid? is "PUSH PUSH" invalid?)
2. Looks up instruction opcodes
3. Distinguishes between label definitions and label references

---

## Files Created Today

| File | Lines | Purpose |
|------|-------|---------|
| `instructions.h` | ~60 | Shared opcode definitions |
| `lexer.h` | ~80 | Token types and structures |
| `lexer.c` | ~190 | Tokenization implementation |
| `main.c` | ~100 | Test program |
| `Makefile` | ~30 | Build configuration |

---

## Key Takeaways

1. **Lexers break text into tokens** - the first step in any language processor
2. **Each token has a type and value** - both are important
3. **Line numbers help users** - always track them for error messages
4. **Comments are whitespace** - just skip them during tokenization
5. **Simple is better** - our lexer is about 200 lines and handles everything we need
>>>>>>> master
