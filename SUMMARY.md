# Day 5: Error Handling + CLI

## What We Built Today

Today we created a proper **command-line interface (CLI)** for the assembler and improved error handling throughout. The assembler is now a complete, user-friendly tool.

Features added:
- Read assembly from files (not just strings)
- Automatic output filename generation
- The `-o` flag for custom output names
- Clear error messages with context
- Help text (`-h` / `--help`)
# Day 5: Function Calls

## What We Built Today

Today we added function call capabilities:
1. **CALL** - Call a function (save where to return, jump to function)
2. **RET** - Return from function (jump back to caller)

With functions, we can now write reusable code and even recursion!

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

```c
typedef struct {
    bool success;           // Did it work?
    int instruction_count;  // How many instructions?
    int bytecode_size;      // How big is the output?
    int label_count;        // How many labels?
    char error_msg[512];    // What went wrong?
} AssemblerResult;
```
Before CALL 100:
    PC = 10 (at the CALL instruction)
    Return Stack: []

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

### Successful assembly
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
```bash
cd student1/day5
make
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

### 4. Writing to stderr vs stdout
```c
// Use stderr for errors
fprintf(stderr, "Error: %s\n", message);

// Use stdout for normal output
printf("Success!\n");
```

This matters for shell scripting: `./asm file.asm > /dev/null` hides normal output but still shows errors.

---
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

Tomorrow (Day 6) we'll create test programs - a suite of assembly files that test all the features:
- Basic arithmetic
- Stack operations
- Memory access
- Loops and conditionals
- Function calls
- Complex programs (factorial, fibonacci)
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

1. **Good CLIs are user-friendly** - help text, clear errors, sensible defaults
2. **Result structures beat error codes** - more information, cleaner code
3. **Error messages need context** - file, line, what was expected
4. **Exit codes matter** - 0 for success, non-zero for failure
5. **Separate concerns** - CLI logic vs core functionality
This will let us run programs from files instead of hardcoded arrays!
