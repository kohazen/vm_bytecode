# Day 1: Lexer (Tokenizer)

## What We Built Today

Today we created the **lexer** (also called a tokenizer) - the first stage of our assembler. The lexer reads assembly source code as raw text and breaks it into **tokens** - the smallest meaningful pieces like instructions, numbers, and labels.

This is exactly how real compilers and assemblers work: first tokenize, then parse, then generate code.

---

## Key Concepts Explained

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

---

## How the Code Works

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

---

## How to Test

### Compile
```bash
make clean
make
```

### Run Tests
```bash
./lexer_test
```

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

---

## What's Next?

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
