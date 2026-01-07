#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"

#define MAX_INSTRUCTIONS 1024
#define MAX_LABEL_LENGTH 64

typedef struct {
    const char *name;
    uint8_t opcode;
    bool has_operand;
} OpcodeEntry;

typedef struct {
    uint8_t opcode;
    bool has_operand;
    int32_t operand;
    bool is_label_ref;
    char label_name[MAX_LABEL_LENGTH];
    int line;
} ParsedInstruction;

typedef struct {
    Token *tokens;
    int token_count;
    int pos;

    ParsedInstruction instructions[MAX_INSTRUCTIONS];
    int instruction_count;

    char error_msg[256];
    bool has_error;
} Parser;

void parser_init(Parser *parser, Token *tokens, int token_count);
bool parser_parse(Parser *parser);
const OpcodeEntry* lookup_opcode(const char *name);
void parser_print_instructions(Parser *parser);

#endif
