#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static bool is_at_end(Lexer *lexer) {
    return lexer->source[lexer->pos] == '\0';
}

static char peek(Lexer *lexer) {
    return lexer->source[lexer->pos];
}

static char advance(Lexer *lexer) {
    return lexer->source[lexer->pos++];
}

static void skip_whitespace(Lexer *lexer) {
    while (!is_at_end(lexer)) {
        char c = peek(lexer);
        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lexer);
        } else {
            break;
        }
    }
}

static void skip_comment(Lexer *lexer) {
    while (!is_at_end(lexer) && peek(lexer) != '\n') {
        advance(lexer);
    }
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

static bool is_alnum(char c) {
    return is_alpha(c) || (c >= '0' && c <= '9');
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool add_token(Lexer *lexer, TokenType type, const char *text, int32_t value) {
    if (lexer->token_count >= MAX_TOKENS) {
        snprintf(lexer->error_msg, sizeof(lexer->error_msg),
                 "Too many tokens (max %d)", MAX_TOKENS);
        lexer->has_error = true;
        return false;
    }

    Token *token = &lexer->tokens[lexer->token_count];
    token->type = type;
    strncpy(token->text, text, MAX_TOKEN_LENGTH - 1);
    token->text[MAX_TOKEN_LENGTH - 1] = '\0';
    token->value = value;
    token->line = lexer->line;

    lexer->token_count++;
    return true;
}

static bool read_identifier(Lexer *lexer) {
    int start = lexer->pos;

    while (!is_at_end(lexer) && is_alnum(peek(lexer))) {
        advance(lexer);
    }

    int length = lexer->pos - start;
    if (length >= MAX_TOKEN_LENGTH) {
        snprintf(lexer->error_msg, sizeof(lexer->error_msg),
                 "Line %d: Identifier too long", lexer->line);
        lexer->has_error = true;
        return false;
    }

    char text[MAX_TOKEN_LENGTH];
    strncpy(text, lexer->source + start, length);
    text[length] = '\0';

    if (!is_at_end(lexer) && peek(lexer) == ':') {
        advance(lexer);
        return add_token(lexer, TOKEN_LABEL_DEF, text, 0);
    }

    return add_token(lexer, TOKEN_INSTRUCTION, text, 0);
}

static bool read_number(Lexer *lexer) {
    int start = lexer->pos;

    if (peek(lexer) == '-') {
        advance(lexer);
    }

    if (is_at_end(lexer) || !is_digit(peek(lexer))) {
        snprintf(lexer->error_msg, sizeof(lexer->error_msg),
                 "Line %d: Expected digit after '-'", lexer->line);
        lexer->has_error = true;
        return false;
    }

    while (!is_at_end(lexer) && is_digit(peek(lexer))) {
        advance(lexer);
    }

    int length = lexer->pos - start;
    char text[MAX_TOKEN_LENGTH];
    strncpy(text, lexer->source + start, length);
    text[length] = '\0';

    int32_t value = atoi(text);

    return add_token(lexer, TOKEN_NUMBER, text, value);
}

void lexer_init(Lexer *lexer, const char *source) {
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->token_count = 0;
    lexer->has_error = false;
    lexer->error_msg[0] = '\0';
}

bool lexer_tokenize(Lexer *lexer) {
    while (!is_at_end(lexer)) {
        skip_whitespace(lexer);

        if (is_at_end(lexer)) break;

        char c = peek(lexer);

        if (c == ';') {
            skip_comment(lexer);
            continue;
        }

        if (c == '\n') {
            advance(lexer);
            if (!add_token(lexer, TOKEN_NEWLINE, "\\n", 0)) return false;
            lexer->line++;
            continue;
        }

        if (is_digit(c) || (c == '-' && is_digit(lexer->source[lexer->pos + 1]))) {
            if (!read_number(lexer)) return false;
            continue;
        }

        if (is_alpha(c)) {
            if (!read_identifier(lexer)) return false;
            continue;
        }

        snprintf(lexer->error_msg, sizeof(lexer->error_msg),
                 "Line %d: Unexpected character '%c'", lexer->line, c);
        lexer->has_error = true;
        return false;
    }

    return add_token(lexer, TOKEN_EOF, "EOF", 0);
}

void lexer_print_tokens(Lexer *lexer) {
    printf("=== Tokens (%d total) ===\n", lexer->token_count);
    for (int i = 0; i < lexer->token_count; i++) {
        Token *t = &lexer->tokens[i];
        printf("[%3d] Line %2d: %-15s '%s'",
               i, t->line, token_type_string(t->type), t->text);
        if (t->type == TOKEN_NUMBER) {
            printf(" (value: %d)", t->value);
        }
        printf("\n");
    }
    printf("========================\n");
}

const char* token_type_string(TokenType type) {
    switch (type) {
        case TOKEN_INSTRUCTION: return "INSTRUCTION";
        case TOKEN_NUMBER:      return "NUMBER";
        case TOKEN_LABEL_DEF:   return "LABEL_DEF";
        case TOKEN_LABEL_REF:   return "LABEL_REF";
        case TOKEN_NEWLINE:     return "NEWLINE";
        case TOKEN_EOF:         return "EOF";
        case TOKEN_ERROR:       return "ERROR";
        default:                return "UNKNOWN";
    }
}
