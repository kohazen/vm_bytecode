/*
 * main.c - Lexer Test Program
 *
 * Day 1: Test the lexer with sample assembly code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

/*
 * Test the lexer with a sample program.
 */
static void test_lexer(const char *name, const char *source) {
    printf("\n=== Test: %s ===\n", name);
    printf("Source:\n%s\n", source);
    printf("\n");

    Lexer lexer;
    lexer_init(&lexer, source);

    if (lexer_tokenize(&lexer)) {
        lexer_print_tokens(&lexer);
        printf("Tokenization successful!\n");
    } else {
        printf("Tokenization failed: %s\n", lexer.error_msg);
    }
}

int main(void) {
    printf("========================================\n");
    printf("  Assembler Lexer - Day 1 Tests\n");
    printf("========================================\n");

    /* Test 1: Simple program */
    test_lexer("Simple program",
        "PUSH 42\n"
        "PUSH 8\n"
        "ADD\n"
        "HALT\n"
    );

    /* Test 2: Program with comments */
    test_lexer("Comments",
        "; This is a comment\n"
        "PUSH 10    ; push first value\n"
        "PUSH 20    ; push second value\n"
        "ADD        ; add them\n"
        "HALT\n"
    );

    /* Test 3: Program with labels */
    test_lexer("Labels",
        "start:\n"
        "    PUSH 5\n"
        "    PUSH 1\n"
        "    SUB\n"
        "    DUP\n"
        "    JNZ start\n"
        "    HALT\n"
    );

    /* Test 4: Negative numbers */
    test_lexer("Negative numbers",
        "PUSH -42\n"
        "PUSH 10\n"
        "ADD\n"
        "HALT\n"
    );

    /* Test 5: All instructions */
    test_lexer("All instructions",
        "PUSH 1\n"
        "POP\n"
        "DUP\n"
        "ADD\n"
        "SUB\n"
        "MUL\n"
        "DIV\n"
        "CMP\n"
        "JMP end\n"
        "JZ skip\n"
        "JNZ loop\n"
        "STORE 0\n"
        "LOAD 0\n"
        "CALL func\n"
        "RET\n"
        "end:\n"
        "skip:\n"
        "loop:\n"
        "func:\n"
        "HALT\n"
    );

    /* Test 6: Memory operations */
    test_lexer("Memory operations",
        "PUSH 100\n"
        "STORE 0\n"
        "PUSH 200\n"
        "STORE 1\n"
        "LOAD 0\n"
        "LOAD 1\n"
        "ADD\n"
        "HALT\n"
    );

    /* Test 7: Empty lines and extra whitespace */
    test_lexer("Whitespace handling",
        "\n"
        "   PUSH   42   \n"
        "\n"
        "   HALT   \n"
        "\n"
    );

    printf("\n========================================\n");
    printf("  All lexer tests completed!\n");
    printf("========================================\n");

    return 0;
}
