/*
 * lexer.c - Lexer Implementation
 *
 * Breaks assembly source code into tokens.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

/* ============================================
 * HELPER FUNCTIONS
 * ============================================ */

/*
 * Check if we've reached the end of source.
 */
static bool is_at_end(Lexer *lexer) {
    return lexer->source[lexer->pos] == '\0';
}

/*
 * Get the current character without advancing.
 */
static char peek(Lexer *lexer) {
    return lexer->source[lexer->pos];
}

/*
 * Get the current character and advance to the next.
 */
static char advance(Lexer *lexer) {
    return lexer->source[lexer->pos++];
}

/*
 * Skip whitespace (but not newlines - they're significant).
 */
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

/*
 * Skip a comment (from ; to end of line).
 */
static void skip_comment(Lexer *lexer) {
    while (!is_at_end(lexer) && peek(lexer) != '\n') {
        advance(lexer);
    }
}

/*
 * Check if a character can start an identifier (letter or underscore).
 */
static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

/*
 * Check if a character can be part of an identifier.
 */
static bool is_alnum(char c) {
    return is_alpha(c) || (c >= '0' && c <= '9');
}

/*
 * Check if a character is a digit.
 */
static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

/*
 * Add a token to the token list.
 */
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

/*
 * Read an identifier (instruction or label).
 */
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

    /* Check if it's a label definition (ends with :) */
    if (!is_at_end(lexer) && peek(lexer) == ':') {
        advance(lexer);  /* consume the colon */
        return add_token(lexer, TOKEN_LABEL_DEF, text, 0);
    }

    /* It's either an instruction or a label reference.
     * We'll figure out which during parsing. For now, treat
     * uppercase as instruction, lowercase/mixed as label ref.
     * Actually, let's mark as INSTRUCTION first - parser will decide. */
    return add_token(lexer, TOKEN_INSTRUCTION, text, 0);
}

/*
 * Read a number (positive or negative integer).
 */
static bool read_number(Lexer *lexer) {
    int start = lexer->pos;

    /* Check for negative sign */
    if (peek(lexer) == '-') {
        advance(lexer);
    }

    /* Must have at least one digit */
    if (is_at_end(lexer) || !is_digit(peek(lexer))) {
        snprintf(lexer->error_msg, sizeof(lexer->error_msg),
                 "Line %d: Expected digit after '-'", lexer->line);
        lexer->has_error = true;
        return false;
    }

    /* Read all digits */
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

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

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

        /* Comment - skip to end of line */
        if (c == ';') {
            skip_comment(lexer);
            continue;
        }

        /* Newline - significant, marks end of instruction */
        if (c == '\n') {
            advance(lexer);
            if (!add_token(lexer, TOKEN_NEWLINE, "\\n", 0)) return false;
            lexer->line++;
            continue;
        }

        /* Number (including negative) */
        if (is_digit(c) || (c == '-' && is_digit(lexer->source[lexer->pos + 1]))) {
            if (!read_number(lexer)) return false;
            continue;
        }

        /* Identifier (instruction or label) */
        if (is_alpha(c)) {
            if (!read_identifier(lexer)) return false;
            continue;
        }

        /* Unknown character */
        snprintf(lexer->error_msg, sizeof(lexer->error_msg),
                 "Line %d: Unexpected character '%c'", lexer->line, c);
        lexer->has_error = true;
        return false;
    }

    /* Add EOF token */
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
