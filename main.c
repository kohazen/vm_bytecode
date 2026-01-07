/*
 * main.c - Code Generator Test Program
 *
 * Day 4: Test the complete assembly pipeline.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "labels.h"
#include "codegen.h"

/*
 * Assemble a program and write to file.
 */
static bool assemble_program(const char *name, const char *source,
                             const char *output_file) {
    printf("\n=== Assembling: %s ===\n", name);
    printf("Source:\n%s\n", source);

    /* Step 1: Tokenize */
    Lexer lexer;
    lexer_init(&lexer, source);

    if (!lexer_tokenize(&lexer)) {
        printf("Lexer error: %s\n", lexer.error_msg);
        return false;
    }

    /* Step 2: Parse */
    Parser parser;
    parser_init(&parser, lexer.tokens, lexer.token_count);

    if (!parser_parse(&parser)) {
        printf("Parser error: %s\n", parser.error_msg);
        return false;
    }

    /* Step 3: Collect labels */
    SymbolTable symtab;
    symtab_init(&symtab);

    if (!symtab_collect_labels(&symtab, lexer.tokens, lexer.token_count,
                               parser.instructions, parser.instruction_count)) {
        printf("Label collection error: %s\n", symtab.error_msg);
        return false;
    }

    /* Step 4: Resolve labels */
    if (!symtab_resolve_labels(&symtab, parser.instructions, parser.instruction_count)) {
        printf("Label resolution error: %s\n", symtab.error_msg);
        return false;
    }

    /* Step 5: Generate bytecode */
    CodeGenerator codegen;
    codegen_init(&codegen);

    if (!codegen_generate(&codegen, parser.instructions, parser.instruction_count)) {
        printf("Code generation error: %s\n", codegen.error_msg);
        return false;
    }

    codegen_print_bytecode(&codegen);

    /* Step 6: Write to file */
    if (!codegen_write_file(&codegen, output_file)) {
        printf("File write error: %s\n", codegen.error_msg);
        return false;
    }

    printf("Wrote %d bytes to '%s' (+ 12 byte header)\n",
           codegen.bytecode_size, output_file);
    printf("Assembly successful!\n");

    return true;
}

int main(void) {
    printf("========================================\n");
    printf("  Assembler Code Generator - Day 4 Tests\n");
    printf("========================================\n");

    /* Test 1: Simple addition */
    assemble_program("Simple addition",
        "PUSH 40\n"
        "PUSH 2\n"
        "ADD\n"
        "HALT\n",
        "test_add.bc"
    );

    /* Test 2: Arithmetic expression: (5 + 3) * 2 = 16 */
    assemble_program("Arithmetic expression",
        "; Calculate (5 + 3) * 2 = 16\n"
        "PUSH 5\n"
        "PUSH 3\n"
        "ADD\n"
        "PUSH 2\n"
        "MUL\n"
        "HALT\n",
        "test_expr.bc"
    );

    /* Test 3: Loop (count down from 3) */
    assemble_program("Loop",
        "    PUSH 3        ; counter\n"
        "loop:\n"
        "    PUSH 1\n"
        "    SUB           ; counter - 1\n"
        "    DUP\n"
        "    JNZ loop      ; repeat if not zero\n"
        "    HALT\n",
        "test_loop.bc"
    );

    /* Test 4: Memory operations */
    assemble_program("Memory operations",
        "PUSH 100\n"
        "STORE 0\n"
        "PUSH 200\n"
        "STORE 1\n"
        "LOAD 0\n"
        "LOAD 1\n"
        "ADD\n"
        "HALT\n",
        "test_memory.bc"
    );

    /* Test 5: Function call */
    assemble_program("Function call",
        "; Main: push 10, call double, halt\n"
        "    PUSH 10\n"
        "    CALL double\n"
        "    HALT\n"
        "\n"
        "; Double function: duplicates and adds\n"
        "double:\n"
        "    DUP\n"
        "    ADD\n"
        "    RET\n",
        "test_func.bc"
    );

    /* Test 6: Conditional jump */
    assemble_program("Conditional jump",
        "    PUSH 0        ; test value\n"
        "    JZ is_zero    ; jump if zero\n"
        "    PUSH 100      ; not zero path\n"
        "    JMP done\n"
        "is_zero:\n"
        "    PUSH 200      ; zero path\n"
        "done:\n"
        "    HALT\n",
        "test_cond.bc"
    );

    printf("\n========================================\n");
    printf("  All bytecode files generated!\n");
    printf("========================================\n");
    printf("\nGenerated files:\n");
    printf("  test_add.bc     - Simple addition (40 + 2 = 42)\n");
    printf("  test_expr.bc    - Expression ((5 + 3) * 2 = 16)\n");
    printf("  test_loop.bc    - Loop (count from 3 to 0)\n");
    printf("  test_memory.bc  - Memory (100 + 200 = 300)\n");
    printf("  test_func.bc    - Function (10 * 2 = 20)\n");
    printf("  test_cond.bc    - Conditional (0 → 200)\n");
    printf("\nRun with VM: ../student1/day7/vm test_add.bc\n");

    return 0;
}
