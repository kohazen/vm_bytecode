#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "instructions.h"

static const OpcodeEntry opcode_table[] = {
    {"PUSH",  OP_PUSH,  true},
    {"POP",   OP_POP,   false},
    {"DUP",   OP_DUP,   false},

    {"ADD",   OP_ADD,   false},
    {"SUB",   OP_SUB,   false},
    {"MUL",   OP_MUL,   false},
    {"DIV",   OP_DIV,   false},
    {"CMP",   OP_CMP,   false},

    {"JMP",   OP_JMP,   true},
    {"JZ",    OP_JZ,    true},
    {"JNZ",   OP_JNZ,   true},

    {"STORE", OP_STORE, true},
    {"LOAD",  OP_LOAD,  true},

    {"CALL",  OP_CALL,  true},
    {"RET",   OP_RET,   false},

    {"HALT",  OP_HALT,  false},

    {NULL, 0, false}
};

static int strcasecmp_local(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = toupper((unsigned char)*s1);
        char c2 = toupper((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return toupper((unsigned char)*s1) - toupper((unsigned char)*s2);
}

static bool is_at_end(Parser *parser) {
    return parser->pos >= parser->token_count ||
           parser->tokens[parser->pos].type == TOKEN_EOF;
}

static Token* current(Parser *parser) {
    return &parser->tokens[parser->pos];
}

static void advance(Parser *parser) {
    if (!is_at_end(parser)) {
        parser->pos++;
    }
}

static void skip_newlines(Parser *parser) {
    while (!is_at_end(parser) && current(parser)->type == TOKEN_NEWLINE) {
        advance(parser);
    }
}

static bool add_instruction(Parser *parser, ParsedInstruction *inst) {
    if (parser->instruction_count >= MAX_INSTRUCTIONS) {
        snprintf(parser->error_msg, sizeof(parser->error_msg),
                 "Too many instructions (max %d)", MAX_INSTRUCTIONS);
        parser->has_error = true;
        return false;
    }

    parser->instructions[parser->instruction_count] = *inst;
    parser->instruction_count++;
    return true;
}

const OpcodeEntry* lookup_opcode(const char *name) {
    for (int i = 0; opcode_table[i].name != NULL; i++) {
        if (strcasecmp_local(opcode_table[i].name, name) == 0) {
            return &opcode_table[i];
        }
    }
    return NULL;
}

void parser_init(Parser *parser, Token *tokens, int token_count) {
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->pos = 0;
    parser->instruction_count = 0;
    parser->has_error = false;
    parser->error_msg[0] = '\0';
}

bool parser_parse(Parser *parser) {
    while (!is_at_end(parser)) {
        skip_newlines(parser);

        if (is_at_end(parser)) break;

        Token *token = current(parser);

        if (token->type == TOKEN_LABEL_DEF) {
            advance(parser);
            continue;
        }

        if (token->type != TOKEN_INSTRUCTION) {
            snprintf(parser->error_msg, sizeof(parser->error_msg),
                     "Line %d: Expected instruction, got %s",
                     token->line, token_type_string(token->type));
            parser->has_error = true;
            return false;
        }

        const OpcodeEntry *entry = lookup_opcode(token->text);
        if (!entry) {
            snprintf(parser->error_msg, sizeof(parser->error_msg),
                     "Line %d: Unknown instruction '%s'",
                     token->line, token->text);
            parser->has_error = true;
            return false;
        }

        ParsedInstruction inst;
        inst.opcode = entry->opcode;
        inst.has_operand = entry->has_operand;
        inst.operand = 0;
        inst.is_label_ref = false;
        inst.label_name[0] = '\0';
        inst.line = token->line;

        advance(parser);

        if (entry->has_operand) {
            if (is_at_end(parser)) {
                snprintf(parser->error_msg, sizeof(parser->error_msg),
                         "Line %d: %s requires an operand",
                         inst.line, entry->name);
                parser->has_error = true;
                return false;
            }

            Token *operand = current(parser);

            if (operand->type == TOKEN_NUMBER) {
                inst.operand = operand->value;
                inst.is_label_ref = false;
            }
            else if (operand->type == TOKEN_INSTRUCTION) {
                inst.is_label_ref = true;
                strncpy(inst.label_name, operand->text, MAX_LABEL_LENGTH - 1);
                inst.label_name[MAX_LABEL_LENGTH - 1] = '\0';
            }
            else if (operand->type == TOKEN_NEWLINE || operand->type == TOKEN_EOF) {
                snprintf(parser->error_msg, sizeof(parser->error_msg),
                         "Line %d: %s requires an operand",
                         inst.line, entry->name);
                parser->has_error = true;
                return false;
            }
            else {
                snprintf(parser->error_msg, sizeof(parser->error_msg),
                         "Line %d: Invalid operand for %s",
                         inst.line, entry->name);
                parser->has_error = true;
                return false;
            }

            advance(parser);
        }

        if (!add_instruction(parser, &inst)) {
            return false;
        }
    }

    return true;
}

void parser_print_instructions(Parser *parser) {
    printf("=== Parsed Instructions (%d total) ===\n", parser->instruction_count);
    for (int i = 0; i < parser->instruction_count; i++) {
        ParsedInstruction *inst = &parser->instructions[i];
        printf("[%3d] Line %2d: opcode=0x%02X", i, inst->line, inst->opcode);

        if (inst->has_operand) {
            if (inst->is_label_ref) {
                printf(" operand=<%s>", inst->label_name);
            } else {
                printf(" operand=%d", inst->operand);
            }
        }
        printf("\n");
    }
    printf("======================================\n");
}
