# Bytecode Virtual Machine

## Overview

This project implements a complete **stack-based bytecode virtual machine** with a fully-featured **two-pass assembler**. The system supports arithmetic operations, control flow, memory management, and function calls through a well-defined instruction set of 16 instructions.

### Components

1. **Virtual Machine (VM)** - Stack-based execution engine that runs bytecode programs
2. **Assembler** - Converts human-readable assembly code to bytecode format
3. **Test Suite** - 11 comprehensive test programs (100% pass rate)
4. **Benchmark Suite** - 4 performance test programs
5. **Documentation** - Complete technical report and usage guides

## Prerequisites

- **C Compiler**: GCC or Clang
- **Make**: GNU Make build system
- **Bash**: For running test scripts
- **Perl**: For benchmark timing (standard on most systems)

## Build Instructions

### Step 1: Clean Previous Builds (Optional)

```bash
make clean
```

This removes all compiled object files, executables, and bytecode files.

### Step 2: Build the VM and Assembler

```bash
make
```

This command:
- Compiles the virtual machine executable: `vm/vm`
- Compiles the assembler executable: `assembler/asm`
- Uses flags: `-Wall -Wextra -g -std=c99`

**Expected Output:**
```
Build complete!
  VM:        vm/vm
  Assembler: assembler/asm

Run 'make tests' to assemble test programs
Run 'make run-tests' to run the test suite
```

### Step 3: Assemble Test Programs

```bash
make tests
```

This assembles all 11 test programs from `.asm` files to `.bc` bytecode files in the `tests/` directory.

### Step 4: (Optional) Assemble Benchmarks

```bash
make benchmarks
```

This assembles all 4 benchmark programs in the `benchmarks/` directory.

## Running the VM

### Basic Usage

To execute a bytecode file:

```bash
./vm/vm <bytecode_file>
```

**Example:**
```bash
./vm/vm tests/factorial.bc
```

**Expected Output:**
```
Loading: tests/factorial.bc
Loaded 88 bytes of bytecode

Running...

=== Execution Complete ===
Status: OK
Result (top of stack): 120
...
```

### Help Command

```bash
./vm/vm --help
```

Shows usage information and available options.

## Using the Assembler

### Basic Usage

To assemble an assembly file:

```bash
./assembler/asm <source.asm> [-o <output.bc>]
```

**Example:**
```bash
./assembler/asm tests/factorial.asm -o tests/factorial.bc
```

If `-o` is not specified, the output file will have the same name as the input with `.bc` extension.

### Help Command

```bash
./assembler/asm --help
```

Shows assembler usage information.

### Writing Assembly Programs

Assembly programs use the following syntax:

```asm
; Comments start with semicolon

    PUSH 10        ; Push value onto stack
    PUSH 20        ; Push another value
    ADD            ; Add top two values
    HALT           ; Stop execution

label_name:        ; Labels for jumps/calls
    LOAD 0         ; Load from memory[0]
    JNZ label_name ; Jump if not zero
```

## Executing Test Programs

### Method 1: Run Individual Tests

```bash
# Assemble the test
./assembler/asm tests/test_arithmetic.asm -o tests/test_arithmetic.bc

# Run the test
./vm/vm tests/test_arithmetic.bc
```

### Method 2: Run All Tests Automatically

```bash
make run-tests
```

This command:
1. Builds the VM and assembler (if needed)
2. Assembles all test programs
3. Runs each test and verifies results
4. Shows a summary of pass/fail status

**Expected Output:**
```
=========================================
  Running Test Suite
=========================================

Running test_arithmetic...
  test_arithmetic           PASS (expected 42, got 42)
Running test_stack...
  test_stack                PASS (expected 10, got 10)
...
Running fibonacci...
  fibonacci                 PASS (expected 55, got 55)

=========================================
  Test Summary: 11/11 PASSED
=========================================
```

### Method 3: Use Demo Script

```bash
bash demo.sh
```

This interactive script demonstrates all features of the VM and assembler.

## Running Benchmarks

Execute the benchmark suite:

```bash
make run-benchmarks
```

Or run manually:

```bash
bash run_benchmarks.sh
```

**Example Output:**
```
=========================================
  Running Benchmarks
=========================================

Running bench_arithmetic...
  bench_arithmetic          PASS  Time: 0.017s
Running bench_loops...
  bench_loops               PASS  Time: 0.012s
Running bench_functions...
  bench_functions           PASS  Time: 0.009s
Running bench_memory...
  bench_memory              PASS  Time: 0.008s

=========================================
  Benchmarks Complete
=========================================
```

## Test Programs Description

| Test Program | Description | Expected Result |
|--------------|-------------|-----------------|
| **test_arithmetic** | Basic arithmetic operations (ADD, SUB, MUL, DIV) | 42 |
| **test_stack** | Stack operations (PUSH, POP, DUP) | 10 |
| **test_comparison** | Comparison instruction (CMP) | 1 |
| **test_jump** | Unconditional jumps (JMP) | 200 |
| **test_conditional** | Conditional branching (JZ) | 200 |
| **test_loop** | Loop with counter (JNZ) | 0 |
| **test_memory** | Memory operations (STORE, LOAD) | 300 |
| **test_function** | Function calls (CALL, RET) | 20 |
| **test_nested_calls** | Nested function calls | 40 |
| **factorial** | Factorial(5) calculation | 120 |
| **fibonacci** | Fibonacci(10) calculation | 55 |

## Instruction Set Reference

### Stack Operations
| Instruction | Opcode | Description | Stack Effect |
|-------------|--------|-------------|--------------|
| `PUSH val` | 0x01 | Push 32-bit integer onto stack | `[] → [val]` |
| `POP` | 0x02 | Remove top of stack | `[val] → []` |
| `DUP` | 0x03 | Duplicate top of stack | `[a] → [a, a]` |

### Arithmetic Operations
| Instruction | Opcode | Description | Stack Effect |
|-------------|--------|-------------|--------------|
| `ADD` | 0x10 | Pop b, pop a, push a+b | `[a, b] → [a+b]` |
| `SUB` | 0x11 | Pop b, pop a, push a-b | `[a, b] → [a-b]` |
| `MUL` | 0x12 | Pop b, pop a, push a×b | `[a, b] → [a×b]` |
| `DIV` | 0x13 | Pop b, pop a, push a÷b | `[a, b] → [a÷b]` |
| `CMP` | 0x14 | Push 1 if a < b, else 0 | `[a, b] → [0/1]` |

### Control Flow
| Instruction | Opcode | Description |
|-------------|--------|-------------|
| `JMP addr` | 0x20 | Unconditional jump to address |
| `JZ addr` | 0x21 | Jump to addr if top of stack is 0 |
| `JNZ addr` | 0x22 | Jump to addr if top of stack is NOT 0 |

### Memory Operations
| Instruction | Opcode | Description | Stack Effect |
|-------------|--------|-------------|--------------|
| `STORE idx` | 0x30 | Store top of stack in Memory[idx] | `[val] → []` |
| `LOAD idx` | 0x31 | Push value from Memory[idx] | `[] → [val]` |

### Function Calls
| Instruction | Opcode | Description |
|-------------|--------|-------------|
| `CALL addr` | 0x40 | Push return address to return stack and jump |
| `RET` | 0x41 | Pop return address from return stack and jump |

### System
| Instruction | Opcode | Description |
|-------------|--------|-------------|
| `HALT` | 0xFF | Stop VM execution |

## Bytecode File Format

Bytecode files (`.bc`) use the following binary format:

```
+------------------------+
| Magic Number           |  4 bytes: 0xCAFEBABE
+------------------------+
| Version                |  4 bytes: 0x00000001
+------------------------+
| Bytecode Size          |  4 bytes: N (little-endian)
+------------------------+
| Bytecode Instructions  |  N bytes
+------------------------+
```

The VM validates the magic number and version before executing any bytecode.

## Project Structure

```
.
├── vm/                          # Virtual Machine
│   ├── vm.c                     # Core VM implementation
│   ├── vm.h                     # VM header
│   ├── bytecode_loader.c        # Bytecode file loader
│   ├── bytecode_loader.h        # Loader header
│   ├── instructions.h           # Opcode definitions
│   └── main.c                   # VM entry point
│
├── assembler/                   # Assembler
│   ├── lexer.c                  # Tokenization
│   ├── lexer.h                  # Lexer header
│   ├── parser.c                 # Instruction parsing
│   ├── parser.h                 # Parser header
│   ├── labels.c                 # Symbol table (two-pass)
│   ├── labels.h                 # Labels header
│   ├── codegen.c                # Bytecode generation
│   ├── codegen.h                # Codegen header
│   ├── assembler.c              # Main assembler logic
│   ├── assembler.h              # Assembler header
│   ├── instructions.h           # Opcode definitions
│   └── main.c                   # Assembler entry point
│
├── tests/                       # Test programs
│   ├── test_arithmetic.asm
│   ├── test_stack.asm
│   ├── test_comparison.asm
│   ├── test_jump.asm
│   ├── test_conditional.asm
│   ├── test_loop.asm
│   ├── test_memory.asm
│   ├── test_function.asm
│   ├── test_nested_calls.asm
│   ├── factorial.asm
│   └── fibonacci.asm
│
├── benchmarks/                  # Benchmark programs
│   ├── bench_arithmetic.asm
│   ├── bench_loops.asm
│   ├── bench_functions.asm
│   └── bench_memory.asm
│
├── instructions.h               # Shared opcode definitions
├── Makefile                     # Build system
├── README.md                    # This file
├── TECHNICAL_REPORT.md          # Technical documentation (LaTeX)
├── SUBMISSION.md                # Submission documentation
├── TEST_RESULTS.md              # Detailed test results
├── QUICKSTART.md                # Quick start guide
├── demo.sh                      # Interactive demo script
├── run_tests.sh                 # Test runner script
└── run_benchmarks.sh            # Benchmark runner script
```

## Makefile Targets

| Target | Description |
|--------|-------------|
| `make` | Build both VM and Assembler (default target) |
| `make tests` | Assemble all test programs |
| `make benchmarks` | Assemble all benchmark programs |
| `make run-tests` | Build, assemble, and run all tests |
| `make run-benchmarks` | Build, assemble, and run benchmarks |
| `make clean` | Remove all compiled files and bytecode |
| `make help` | Show help message with all targets |

## Example: Creating and Running a Program

### Step 1: Write the Assembly Code

Create a file `myprogram.asm`:

```asm
; Calculate 100 + 50 * 2

    PUSH 50        ; Push 50
    PUSH 2         ; Push 2
    MUL            ; 50 * 2 = 100
    PUSH 100       ; Push 100
    ADD            ; 100 + 100 = 200
    HALT           ; Stop with 200 on stack
```

### Step 2: Assemble the Program

```bash
./assembler/asm myprogram.asm -o myprogram.bc
```

**Output:**
```
Assembling: myprogram.asm
Output:     myprogram.bc

Assembly successful!
  Instructions: 6
  Labels:       0
  Bytecode:     18 bytes (+ 12 byte header)
```

### Step 3: Run the Program

```bash
./vm/vm myprogram.bc
```

**Output:**
```
Loading: myprogram.bc
Loaded 18 bytes of bytecode

Running...

=== Execution Complete ===
Status: OK
Result (top of stack): 200
...
```

## Advanced Example: Factorial Function

See `tests/factorial.asm` for a complete example:

```asm
; Calculate factorial of 5
    PUSH 5
    STORE 0       ; counter = 5
    PUSH 1
    STORE 1       ; result = 1

loop:
    LOAD 1        ; load result
    LOAD 0        ; load counter
    MUL           ; result * counter
    STORE 1       ; save result

    LOAD 0        ; load counter
    PUSH 1
    SUB           ; counter - 1
    DUP
    STORE 0       ; save counter
    JNZ loop      ; continue if not zero

    LOAD 1        ; load final result (120)
    HALT
```

This demonstrates:
- Memory operations (STORE/LOAD)
- Labels for loops
- Conditional branching (JNZ)
- Stack manipulation (DUP)

## Troubleshooting

### Build Fails

**Problem:** Compiler errors or warnings

**Solution:**
```bash
make clean
make
```

Ensure you have GCC or Clang installed.

### Test Fails

**Problem:** A test produces incorrect output

**Solution:**
1. Check if bytecode is up to date: `make tests`
2. Run the specific test: `./vm/vm tests/test_name.bc`
3. Check the expected vs. actual output

### Assembler Errors

**Problem:** Assembly fails with "undefined label" error

**Solution:** Ensure all labels referenced in JMP/JZ/JNZ/CALL are defined with `label:` syntax.

**Problem:** "Invalid instruction" error

**Solution:** Check instruction spelling (case-insensitive) and ensure operands are provided where needed.

## VM Architecture Highlights

### Memory Model
- **Data Stack**: 1024 elements for operands
- **Return Stack**: 256 elements for function calls (separate from data stack)
- **Memory Array**: 256 cells for global variables
- **Program Counter**: Points to current instruction

### Error Handling
- Stack overflow/underflow detection
- Memory bounds checking
- Division by zero protection
- Invalid instruction detection
- Return stack overflow/underflow protection

### Execution Model
- Fetch-decode-execute cycle
- Variable-length instructions (1 or 5 bytes)
- Little-endian byte order
- Deterministic execution
- No memory leaks (verified with valgrind)

## Assembler Features

### Two-Pass Assembly
- **Pass 1**: Collect all label definitions and calculate addresses
- **Pass 2**: Generate bytecode with resolved label references

### Supported Features
- Case-insensitive instruction mnemonics
- Label definitions for jumps and calls
- Comments (semicolon syntax)
- Line number tracking for error reporting
- Comprehensive error messages

## Performance Notes

Execution is optimized for correctness over speed. Typical execution times:
- **Arithmetic test**: ~40 instructions, <0.01s
- **Factorial(5)**: ~81 instructions, <0.01s
- **Fibonacci(10)**: ~340+ instructions, <0.01s

Benchmarks demonstrate consistent performance across different instruction types.


## Known Limitations

1. **Integer-only arithmetic** - No floating-point support
2. **Fixed memory sizes** - Stack: 1024, Return: 256, Memory: 256
3. **No I/O operations** - No system calls for input/output
4. **Single-file assembly** - No module system or linking

These are intentional design choices for educational clarity.

## Future Enhancements

Possible improvements (out of scope for this lab):
- Floating-point arithmetic instructions
- System calls for I/O operations
- Debugger with breakpoints and step execution
- Optimization passes in assembler
- JIT compilation for performance
- Multi-file assembly with linker

## License

MIT