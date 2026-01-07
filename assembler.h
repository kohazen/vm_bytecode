/*
 * assembler.h - Main Assembler Interface
 *
 * Provides a clean interface to the complete assembly pipeline.
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdbool.h>

/* Maximum size of source file */
#define MAX_SOURCE_SIZE 65536

/* ============================================
 * ASSEMBLER RESULT
 * ============================================
 */

typedef struct {
    bool success;
    int instruction_count;
    int bytecode_size;
    int label_count;
    char error_msg[512];
} AssemblerResult;

/* ============================================
 * FUNCTION DECLARATIONS
 * ============================================ */

/*
 * Assemble a source file to bytecode.
 *
 * Parameters:
 *   input_file  - Path to the .asm source file
 *   output_file - Path for the .bc bytecode output
 *
 * Returns:
 *   AssemblerResult with success/failure status and details.
 */
AssemblerResult assemble_file(const char *input_file, const char *output_file);

/*
 * Assemble source code string to bytecode file.
 *
 * Parameters:
 *   source      - The assembly source code string
 *   output_file - Path for the .bc bytecode output
 *
 * Returns:
 *   AssemblerResult with success/failure status and details.
 */
AssemblerResult assemble_string(const char *source, const char *output_file);

/*
 * Print usage information.
 */
void print_usage(const char *program_name);

#endif /* ASSEMBLER_H */
