/*
 * main.c - Parser Test Program
 *
 * Day 2: Test the parser with sample assembly code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

/*
 * Test the lexer and parser with a sample program.
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

    printf("\nTokens: %d\n", lexer.token_count);

    /* Step 2: Parse */
    Parser parser;
    parser_init(&parser, lexer.tokens, lexer.token_count);

    if (!parser_parse(&parser)) {
        printf("Parser error: %s\n", parser.error_msg);
        return;
    }

    parser_print_instructions(&parser);
    printf("Parsing successful!\n");
}

int main(void) {
    printf("========================================\n");
    printf("  Assembler Parser - Day 2 Tests\n");
    printf("========================================\n");

    /* Test 1: Simple arithmetic */
    test_program("Simple arithmetic",
        "PUSH 10\n"
        "PUSH 20\n"
        "ADD\n"
        "HALT\n"
    );

    /* Test 2: All instructions without operands */
    test_program("No-operand instructions",
        "POP\n"
        "DUP\n"
        "ADD\n"
        "SUB\n"
        "MUL\n"
        "DIV\n"
        "CMP\n"
        "RET\n"
        "HALT\n"
    );

    /* Test 3: All instructions with operands */
    test_program("Operand instructions",
        "PUSH 42\n"
        "STORE 0\n"
        "LOAD 1\n"
        "JMP 0\n"
        "JZ 0\n"
        "JNZ 0\n"
        "CALL 0\n"
        "HALT\n"
    );

    /* Test 4: Negative numbers */
    test_program("Negative numbers",
        "PUSH -100\n"
        "PUSH 50\n"
        "ADD\n"
        "HALT\n"
    );

    /* Test 5: Labels (references only, resolution in Day 3) */
    test_program("Label references",
        "start:\n"
        "    PUSH 5\n"
        "    JNZ start\n"
        "end:\n"
        "    HALT\n"
    );

    /* Test 6: Case insensitivity */
    test_program("Case insensitivity",
        "push 10\n"
        "Push 20\n"
        "PUSH 30\n"
        "add\n"
        "Add\n"
        "halt\n"
    );

    /* Test 7: Comments and whitespace */
    test_program("Comments and whitespace",
        "; Program start\n"
        "PUSH 100  ; first value\n"
        "   PUSH 200   ; second value\n"
        "ADD           ; add them\n"
        "HALT          ; done\n"
    );

    /* Test 8: Error - missing operand */
    printf("\n=== Test: Error - missing operand ===\n");
    {
        const char *source = "PUSH\n";
        printf("Source:\n%s\n", source);

        Lexer lexer;
        lexer_init(&lexer, source);
        lexer_tokenize(&lexer);

        Parser parser;
        parser_init(&parser, lexer.tokens, lexer.token_count);

        if (!parser_parse(&parser)) {
            printf("Expected error: %s\n", parser.error_msg);
        } else {
            printf("ERROR: Should have failed!\n");
        }
    }

    /* Test 9: Error - unknown instruction */
    printf("\n=== Test: Error - unknown instruction ===\n");
    {
        const char *source = "UNKNOWN 42\n";
        printf("Source:\n%s\n", source);

        Lexer lexer;
        lexer_init(&lexer, source);
        lexer_tokenize(&lexer);

        Parser parser;
        parser_init(&parser, lexer.tokens, lexer.token_count);

        if (!parser_parse(&parser)) {
            printf("Expected error: %s\n", parser.error_msg);
        } else {
            printf("ERROR: Should have failed!\n");
        }
    }

    printf("\n========================================\n");
    printf("  All parser tests completed!\n");
    printf("========================================\n");

    return 0;
}
