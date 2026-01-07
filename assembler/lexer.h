#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_TOKEN_LENGTH 64
#define MAX_TOKENS 1024

typedef enum {
    TOKEN_INSTRUCTION,
    TOKEN_NUMBER,
    TOKEN_LABEL_DEF,
    TOKEN_LABEL_REF,
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LENGTH];
    int32_t value;
    int line;
} Token;

typedef struct {
    const char *source;
    int pos;
    int line;

    Token tokens[MAX_TOKENS];
    int token_count;

    char error_msg[256];
    bool has_error;
} Lexer;

void lexer_init(Lexer *lexer, const char *source);
bool lexer_tokenize(Lexer *lexer);
void lexer_print_tokens(Lexer *lexer);
const char* token_type_string(TokenType type);

#endif
