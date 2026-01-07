# Bytecode Virtual Machine

A stack-based virtual machine that executes bytecode programs.

## Building

```bash
make
```

## Usage

```bash
./vm <bytecode_file>
```

Example:
```bash
./vm program.bc
```

For help:
```bash
./vm --help
```

## Bytecode File Format

| Field | Size | Description |
|-------|------|-------------|
| Magic | 4 bytes | `0xCAFEBABE` (little-endian) |
| Version | 4 bytes | `0x00000001` |
| Code Size | 4 bytes | Number of bytecode bytes |
| Code | variable | Raw bytecode instructions |

## Supported Instructions

| Instruction | Opcode | Operand | Description |
|-------------|--------|---------|-------------|
| PUSH val | 0x01 | 4 bytes | Push value onto stack |
| POP | 0x02 | - | Remove top of stack |
| DUP | 0x03 | - | Duplicate top of stack |
| ADD | 0x10 | - | Add top two values |
| SUB | 0x11 | - | Subtract top from second |
| MUL | 0x12 | - | Multiply top two values |
| DIV | 0x13 | - | Divide second by top |
| CMP | 0x14 | - | Compare: push 1 if a<b else 0 |
| JMP addr | 0x20 | 4 bytes | Jump to address |
| JZ addr | 0x21 | 4 bytes | Jump if zero |
| JNZ addr | 0x22 | 4 bytes | Jump if not zero |
| STORE idx | 0x30 | 4 bytes | Store to memory |
| LOAD idx | 0x31 | 4 bytes | Load from memory |
| CALL addr | 0x40 | 4 bytes | Call function |
| RET | 0x41 | - | Return from function |
| HALT | 0xFF | - | Stop execution |

## Output

The VM displays:
- Bytecode size loaded
- Execution status (OK or error)
- Result (top of stack)
- Final VM state (stack, memory, registers)

## Error Codes

- `Stack overflow` - Stack is full
- `Stack underflow` - Stack is empty
- `Division by zero` - Cannot divide by zero
- `Memory access out of bounds` - Invalid memory index
- `Code access out of bounds` - PC went past code
- `Invalid opcode` - Unknown instruction
- `Return stack overflow/underflow` - Function call issues
- `File I/O error` - Cannot read bytecode file

## Files

- `vm.h` / `vm.c` - Virtual machine implementation
- `bytecode_loader.h` / `bytecode_loader.c` - File loading
- `instructions.h` - Opcode definitions (shared with assembler)
- `main.c` - CLI entry point
