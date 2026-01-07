/*
 * codegen.h - Code Generator for the Assembler
 *
 * Takes parsed and resolved instructions and generates bytecode.
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdbool.h>
#include <stdint.h>
#include "parser.h"

/* Bytecode file format constants (must match VM's bytecode_loader.h) */
#define BYTECODE_MAGIC   0xCAFEBABE
#define BYTECODE_VERSION 0x00000001

/* Maximum bytecode size */
#define MAX_BYTECODE_SIZE 65536

/* ============================================
 * CODE GENERATOR STRUCTURE
 * ============================================
 */

typedef struct {
    uint8_t bytecode[MAX_BYTECODE_SIZE];
    int bytecode_size;

    char error_msg[256];
    bool has_error;
} CodeGenerator;

/* ============================================
 * FUNCTION DECLARATIONS
 * ============================================ */

/*
 * Initialize the code generator.
 */
void codegen_init(CodeGenerator *gen);

/*
 * Generate bytecode from parsed instructions.
 * Instructions must already have labels resolved.
 */
bool codegen_generate(CodeGenerator *gen, ParsedInstruction *instructions,
                      int instruction_count);

/*
 * Write the bytecode to a file.
 * This writes the header (magic, version, size) followed by the code.
 */
bool codegen_write_file(CodeGenerator *gen, const char *filename);

/*
 * Print the bytecode in hex (for debugging).
 */
void codegen_print_bytecode(CodeGenerator *gen);

#endif /* CODEGEN_H */
