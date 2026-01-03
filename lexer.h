/*
 * lexer.h - Lexer (Tokenizer) for the Assembler
 *
 * The lexer breaks source code into tokens - the smallest meaningful
 * units like instructions, numbers, labels, etc.
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>

/* Maximum length of a token (instruction name, label, etc.) */
#define MAX_TOKEN_LENGTH 64

/* Maximum number of tokens we can handle */
#define MAX_TOKENS 1024

/* ============================================
 * TOKEN TYPES
 * ============================================
 * These identify what kind of token we found.
 */

typedef enum {
    TOKEN_INSTRUCTION,  /* An instruction like PUSH, ADD, HALT */
    TOKEN_NUMBER,       /* A numeric value like 42 or -7 */
    TOKEN_LABEL_DEF,    /* A label definition like "loop:" */
    TOKEN_LABEL_REF,    /* A label reference like "loop" (used as operand) */
    TOKEN_NEWLINE,      /* End of a line */
    TOKEN_EOF,          /* End of file */
    TOKEN_ERROR         /* Invalid token */
} TokenType;

/* ============================================
 * TOKEN STRUCTURE
 * ============================================
 * Holds information about a single token.
 */

typedef struct {
    TokenType type;                   /* What kind of token this is */
    char text[MAX_TOKEN_LENGTH];      /* The actual text of the token */
    int32_t value;                    /* Numeric value (for TOKEN_NUMBER) */
    int line;                         /* Line number in source file */
} Token;

/* ============================================
 * LEXER STRUCTURE
 * ============================================
 * Holds the state of the lexer as it processes input.
 */

typedef struct {
    const char *source;    /* The source code being tokenized */
    int pos;               /* Current position in source */
    int line;              /* Current line number (for error messages) */

    Token tokens[MAX_TOKENS];  /* Array of tokens found */
    int token_count;           /* Number of tokens found */

    char error_msg[256];   /* Error message if something went wrong */
    bool has_error;        /* Whether an error occurred */
} Lexer;

/* ============================================
 * FUNCTION DECLARATIONS
 * ============================================ */

/*
 * Initialize the lexer with source code.
 */
void lexer_init(Lexer *lexer, const char *source);

/*
 * Tokenize the entire source code.
 * Returns true on success, false on error.
 * After calling, check lexer->tokens and lexer->token_count.
 */
bool lexer_tokenize(Lexer *lexer);

/*
 * Print all tokens (for debugging).
 */
void lexer_print_tokens(Lexer *lexer);

/*
 * Get a string representation of a token type.
 */
const char* token_type_string(TokenType type);

#endif /* LEXER_H */
