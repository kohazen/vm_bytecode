/*
 * main.c - Label Resolution Test Program
 *
 * Day 3: Test two-pass label resolution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "labels.h"

/*
 * Test the complete pipeline: lex → parse → resolve labels
 */
static void test_program(const char *name, const char *source) {
    printf("\n=== Test: %s ===\n", name);
    printf("Source:\n%s\n", source);

    /* Step 1: Tokenize */
    Lexer lexer;
    lexer_init(&lexer, source);

    if (!lexer_tokenize(&lexer)) {
        printf("Lexer error: %s\n", lexer.error_msg);
        return;
    }

    /* Step 2: Parse */
    Parser parser;
    parser_init(&parser, lexer.tokens, lexer.token_count);

    if (!parser_parse(&parser)) {
        printf("Parser error: %s\n", parser.error_msg);
        return;
    }

    printf("Parsed %d instructions\n", parser.instruction_count);

    /* Step 3: Collect labels (Pass 1) */
    SymbolTable symtab;
    symtab_init(&symtab);

    if (!symtab_collect_labels(&symtab, lexer.tokens, lexer.token_count,
                               parser.instructions, parser.instruction_count)) {
        printf("Label collection error: %s\n", symtab.error_msg);
        return;
    }

    symtab_print(&symtab);

    /* Step 4: Resolve labels (Pass 2) */
    if (!symtab_resolve_labels(&symtab, parser.instructions, parser.instruction_count)) {
        printf("Label resolution error: %s\n", symtab.error_msg);
        return;
    }

    /* Print final instructions with resolved addresses */
    printf("\n=== Resolved Instructions ===\n");
    int address = 0;
    for (int i = 0; i < parser.instruction_count; i++) {
        ParsedInstruction *inst = &parser.instructions[i];
        printf("[%3d] addr=%3d: opcode=0x%02X", i, address, inst->opcode);
        if (inst->has_operand) {
            printf(" operand=%d (0x%04X)", inst->operand, inst->operand);
        }
        printf("\n");
        address += inst->has_operand ? 5 : 1;
    }
    printf("=============================\n");
    printf("Total bytecode size: %d bytes\n", address);
    printf("\nLabel resolution successful!\n");
}

int main(void) {
    printf("========================================\n");
    printf("  Assembler Labels - Day 3 Tests\n");
    printf("========================================\n");

    /* Test 1: Simple loop with backward jump */
    test_program("Simple loop (backward jump)",
        "loop:\n"
        "    PUSH 1\n"
        "    SUB\n"
        "    DUP\n"
        "    JNZ loop\n"
        "    HALT\n"
    );

    /* Test 2: Forward jump */
    test_program("Forward jump",
        "    PUSH 0\n"
        "    JZ skip\n"
        "    PUSH 100\n"
        "skip:\n"
        "    PUSH 200\n"
        "    HALT\n"
    );

    /* Test 3: Multiple labels */
    test_program("Multiple labels",
        "start:\n"
        "    PUSH 5\n"
        "    JMP middle\n"
        "end:\n"
        "    HALT\n"
        "middle:\n"
        "    PUSH 10\n"
        "    JMP end\n"
    );

    /* Test 4: Function calls */
    test_program("Function calls",
        "main:\n"
        "    PUSH 10\n"
        "    CALL double\n"
        "    HALT\n"
        "\n"
        "double:\n"
        "    DUP\n"
        "    ADD\n"
        "    RET\n"
    );

    /* Test 5: Complex program - countdown */
    test_program("Countdown",
        "; Count down from 5 to 0\n"
        "    PUSH 5        ; initial value\n"
        "    STORE 0       ; save counter\n"
        "\n"
        "loop:\n"
        "    LOAD 0        ; load counter\n"
        "    PUSH 1        ; decrement amount\n"
        "    SUB           ; counter - 1\n"
        "    DUP           ; save result\n"
        "    STORE 0       ; save new counter\n"
        "    JNZ loop      ; if not zero, continue\n"
        "\n"
        "done:\n"
        "    HALT\n"
    );

    /* Test 6: Error - undefined label */
    printf("\n=== Test: Error - undefined label ===\n");
    {
        const char *source = "PUSH 5\nJMP undefined\nHALT\n";
        printf("Source:\n%s\n", source);

        Lexer lexer;
        lexer_init(&lexer, source);
        lexer_tokenize(&lexer);

        Parser parser;
        parser_init(&parser, lexer.tokens, lexer.token_count);
        parser_parse(&parser);

        SymbolTable symtab;
        symtab_init(&symtab);
        symtab_collect_labels(&symtab, lexer.tokens, lexer.token_count,
                              parser.instructions, parser.instruction_count);

        if (!symtab_resolve_labels(&symtab, parser.instructions, parser.instruction_count)) {
            printf("Expected error: %s\n", symtab.error_msg);
        } else {
            printf("ERROR: Should have failed!\n");
        }
    }

    /* Test 7: Error - duplicate label */
    printf("\n=== Test: Error - duplicate label ===\n");
    {
        const char *source = "start:\nPUSH 1\nstart:\nHALT\n";
        printf("Source:\n%s\n", source);

        Lexer lexer;
        lexer_init(&lexer, source);
        lexer_tokenize(&lexer);

        Parser parser;
        parser_init(&parser, lexer.tokens, lexer.token_count);
        parser_parse(&parser);

        SymbolTable symtab;
        symtab_init(&symtab);

        if (!symtab_collect_labels(&symtab, lexer.tokens, lexer.token_count,
                                   parser.instructions, parser.instruction_count)) {
            printf("Expected error: %s\n", symtab.error_msg);
        } else {
            printf("ERROR: Should have failed!\n");
        }
    }

    printf("\n========================================\n");
    printf("  All label tests completed!\n");
    printf("========================================\n");

    return 0;
}
