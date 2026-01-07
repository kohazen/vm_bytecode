# Day 5: Error Handling + CLI

## What We Built Today

Today we created a proper **command-line interface (CLI)** for the assembler and improved error handling throughout. The assembler is now a complete, user-friendly tool.

Features added:
- Read assembly from files (not just strings)
- Automatic output filename generation
- The `-o` flag for custom output names
- Clear error messages with context
- Help text (`-h` / `--help`)

---

## Key Concepts Explained

### 1. Command-Line Argument Parsing

When you run `./asm input.asm -o output.bc`, the program receives:
- `argc = 4` (argument count)
- `argv[0] = "./asm"` (program name)
- `argv[1] = "input.asm"` (first argument)
- `argv[2] = "-o"` (flag)
- `argv[3] = "output.bc"` (flag value)

```c
int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            output_file = argv[++i];  // Next argument
        }
        else if (argv[i][0] != '-') {
            input_file = argv[i];     // Not a flag
        }
    }
}
```

### 2. Error Handling Strategy

Good error handling means:

1. **Detect early**: Check for problems as soon as possible
2. **Report clearly**: Tell the user exactly what went wrong
3. **Include context**: Line numbers, file names, what was expected
4. **Clean up properly**: Free memory, close files

```c
// Good error message
"Line 5: Undefined label 'loop_end'"

// Bad error message
"Error"
```

### 3. Result Structures

Instead of just returning success/failure, we return a structure with details:

```c
typedef struct {
    bool success;           // Did it work?
    int instruction_count;  // How many instructions?
    int bytecode_size;      // How big is the output?
    int label_count;        // How many labels?
    char error_msg[512];    // What went wrong?
} AssemblerResult;
```

This lets us show useful information on success and helpful errors on failure.

### 4. Default Output Filename

If the user doesn't specify `-o`, we generate a default:

```
input.asm  →  input.bc
program.s  →  program.bc
test       →  test.bc
```

```c
// Find the last '.' and replace extension
char *dot = strrchr(filename, '.');
if (dot) {
    strcpy(dot, ".bc");
}
```

### 5. File Reading

Reading a file into a string:

```c
char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    char *buffer = malloc(size + 1);

    // Read entire file
    fread(buffer, 1, size, file);
    buffer[size] = '\0';  // Null-terminate

    fclose(file);
    return buffer;
}
```

---

## How the Code Works

### The Assembler Wrapper

`assembler.c` provides a clean interface:

```c
AssemblerResult assemble_file(const char *input, const char *output) {
    // 1. Read the file
    char *source = read_file(input);

    // 2. Tokenize
    Lexer lexer;
    lexer_init(&lexer, source);
    if (!lexer_tokenize(&lexer)) {
        return error_result("Lexer error: ...");
    }

    // 3. Parse
    Parser parser;
    parser_init(&parser, lexer.tokens, lexer.token_count);
    if (!parser_parse(&parser)) {
        return error_result("Parser error: ...");
    }

    // 4. Collect labels
    SymbolTable symtab;
    symtab_init(&symtab);
    if (!symtab_collect_labels(...)) {
        return error_result("Label error: ...");
    }

    // 5. Resolve labels
    if (!symtab_resolve_labels(...)) {
        return error_result("Label error: ...");
    }

    // 6. Generate bytecode
    CodeGenerator codegen;
    codegen_init(&codegen);
    if (!codegen_generate(...)) {
        return error_result("Codegen error: ...");
    }

    // 7. Write output file
    if (!codegen_write_file(&codegen, output)) {
        return error_result("File error: ...");
    }

    return success_result(...);
}
```

### The CLI

`main.c` handles command-line arguments:

```c
int main(int argc, char *argv[]) {
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-o") == 0) {
            output_file = argv[++i];
        }
        else {
            input_file = argv[i];
        }
    }

    // Validate
    if (!input_file) {
        fprintf(stderr, "Error: No input file\n");
        return 1;
    }

    // Generate default output filename
    if (!output_file) {
        make_output_filename(input_file, default_output);
        output_file = default_output;
    }

    // Assemble
    AssemblerResult result = assemble_file(input_file, output_file);

    // Report result
    if (result.success) {
        printf("Success!\n");
        return 0;
    } else {
        fprintf(stderr, "Failed: %s\n", result.error_msg);
        return 1;
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

### Create a test file
```bash
cat > test.asm << 'EOF'
; Simple test program
PUSH 40
PUSH 2
ADD
HALT
EOF
```

### Assemble it
```bash
./asm test.asm
```

### Specify output name
```bash
./asm test.asm -o myprogram.bc
```

### Get help
```bash
./asm --help
```

---

## Expected Output

### Successful assembly
```
$ ./asm test.asm
Assembling: test.asm
Output:     test.bc

Assembly successful!
  Instructions: 4
  Labels:       0
  Bytecode:     12 bytes (+ 12 byte header)
```

### Missing input file
```
$ ./asm
Error: No input file specified

Usage: ./asm <input.asm> [-o <output.bc>]

Assembles an assembly source file into bytecode.

Options:
  -o <file>   Specify output file (default: input with .bc extension)
  -h, --help  Show this help message
```

### File not found
```
$ ./asm nonexistent.asm
Assembling: nonexistent.asm

Assembly failed!
Cannot read file 'nonexistent.asm'
```

### Syntax error
```
$ echo "PUSH" > bad.asm
$ ./asm bad.asm
Assembling: bad.asm

Assembly failed!
Parser error: Line 1: PUSH requires an operand
```

### Undefined label
```
$ echo "JMP nowhere" > bad.asm
$ ./asm bad.asm
Assembling: bad.asm

Assembly failed!
Label error: Line 1: Undefined label 'nowhere'
```

---

## Error Messages by Stage

| Stage | Example Error |
|-------|---------------|
| File I/O | "Cannot read file 'test.asm'" |
| Lexer | "Line 3: Unexpected character '#'" |
| Parser | "Line 5: PUSH requires an operand" |
| Labels | "Line 7: Undefined label 'loop'" |
| Labels | "Line 9: Label 'start' already defined on line 1" |
| Codegen | "Bytecode too large (max 65536 bytes)" |
| Output | "Cannot create file 'output.bc'" |

---

## Common Mistakes and How to Avoid Them

### 1. Not Checking argc Before Accessing argv
```c
// WRONG - crashes if argc < 2
output_file = argv[1];

// CORRECT - check first
if (argc < 2) {
    print_usage(argv[0]);
    return 1;
}
```

### 2. Not Handling -o Without Argument
```c
// WRONG - crashes
if (strcmp(argv[i], "-o") == 0) {
    output_file = argv[i + 1];  // Might be out of bounds!
}

// CORRECT - check bounds
if (strcmp(argv[i], "-o") == 0) {
    if (i + 1 >= argc) {
        fprintf(stderr, "Error: -o requires a filename\n");
        return 1;
    }
    output_file = argv[++i];
}
```

### 3. Forgetting to Free Memory
```c
// WRONG - memory leak
char *source = read_file(filename);
return error_result("Parse failed");  // source not freed!

// CORRECT - clean up first
char *source = read_file(filename);
if (!parse_ok) {
    free(source);
    return error_result("Parse failed");
}
```

### 4. Writing to stderr vs stdout
```c
// Use stderr for errors
fprintf(stderr, "Error: %s\n", message);

// Use stdout for normal output
printf("Success!\n");
```

This matters for shell scripting: `./asm file.asm > /dev/null` hides normal output but still shows errors.

---

## What's Next?

Tomorrow (Day 6) we'll create test programs - a suite of assembly files that test all the features:
- Basic arithmetic
- Stack operations
- Memory access
- Loops and conditionals
- Function calls
- Complex programs (factorial, fibonacci)

---

## Files in This Day

| File | Lines | Purpose |
|------|-------|---------|
| `instructions.h` | ~60 | Opcode definitions |
| `lexer.h/.c` | ~270 | Tokenization |
| `parser.h/.c` | ~250 | Parsing |
| `labels.h/.c` | ~190 | Label resolution |
| `codegen.h/.c` | ~165 | Code generation |
| `assembler.h` | ~50 | Assembler interface |
| `assembler.c` | ~110 | Assembler implementation |
| `main.c` | ~100 | CLI entry point |
| `Makefile` | ~50 | Build configuration |

---

## Key Takeaways

1. **Good CLIs are user-friendly** - help text, clear errors, sensible defaults
2. **Result structures beat error codes** - more information, cleaner code
3. **Error messages need context** - file, line, what was expected
4. **Exit codes matter** - 0 for success, non-zero for failure
5. **Separate concerns** - CLI logic vs core functionality
