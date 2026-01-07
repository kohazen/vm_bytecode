#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "labels.h"
#include "instructions.h"

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

static int instruction_size(ParsedInstruction *inst) {
    if (inst->has_operand) {
        return 5;
    }
    return 1;
}

void symtab_init(SymbolTable *table) {
    table->label_count = 0;
    table->has_error = false;
    table->error_msg[0] = '\0';
}

LabelEntry* symtab_lookup(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->label_count; i++) {
        if (strcasecmp_local(table->labels[i].name, name) == 0) {
            return &table->labels[i];
        }
    }
    return NULL;
}

static bool add_label(SymbolTable *table, const char *name, int32_t address, int line) {
    LabelEntry *existing = symtab_lookup(table, name);
    if (existing && existing->defined) {
        snprintf(table->error_msg, sizeof(table->error_msg),
                 "Line %d: Label '%s' already defined on line %d",
                 line, name, existing->line);
        table->has_error = true;
        return false;
    }

    if (table->label_count >= MAX_LABELS) {
        snprintf(table->error_msg, sizeof(table->error_msg),
                 "Too many labels (max %d)", MAX_LABELS);
        table->has_error = true;
        return false;
    }

    LabelEntry *entry = &table->labels[table->label_count];
    strncpy(entry->name, name, MAX_LABEL_LENGTH - 1);
    entry->name[MAX_LABEL_LENGTH - 1] = '\0';
    entry->address = address;
    entry->line = line;
    entry->defined = true;

    table->label_count++;
    return true;
}

bool symtab_collect_labels(SymbolTable *table, Token *tokens, int token_count,
                           ParsedInstruction *instructions, int instruction_count) {
    int current_address = 0;
    int instruction_index = 0;
    bool prev_was_instruction = false;

    for (int i = 0; i < token_count; i++) {
        Token *token = &tokens[i];

        if (token->type == TOKEN_LABEL_DEF) {
            if (!add_label(table, token->text, current_address, token->line)) {
                return false;
            }
            prev_was_instruction = false;
        }
        else if (token->type == TOKEN_INSTRUCTION) {
            if (!prev_was_instruction) {
                if (instruction_index < instruction_count) {
                    current_address += instruction_size(&instructions[instruction_index]);
                    instruction_index++;
                }
                prev_was_instruction = true;
            } else {
                prev_was_instruction = false;
            }
        }
        else if (token->type == TOKEN_NUMBER) {
            prev_was_instruction = false;
        }
        else if (token->type == TOKEN_NEWLINE || token->type == TOKEN_EOF) {
            prev_was_instruction = false;
        }
    }

    return true;
}

bool symtab_resolve_labels(SymbolTable *table, ParsedInstruction *instructions,
                           int instruction_count) {
    for (int i = 0; i < instruction_count; i++) {
        ParsedInstruction *inst = &instructions[i];

        if (inst->is_label_ref) {
            LabelEntry *entry = symtab_lookup(table, inst->label_name);
            if (!entry) {
                snprintf(table->error_msg, sizeof(table->error_msg),
                         "Line %d: Undefined label '%s'",
                         inst->line, inst->label_name);
                table->has_error = true;
                return false;
            }

            inst->operand = entry->address;
            inst->is_label_ref = false;
        }
    }

    return true;
}

void symtab_print(SymbolTable *table) {
    printf("=== Symbol Table (%d labels) ===\n", table->label_count);
    for (int i = 0; i < table->label_count; i++) {
        LabelEntry *entry = &table->labels[i];
        printf("  %-20s = %d (0x%04X)  [line %d]\n",
               entry->name, entry->address, entry->address, entry->line);
    }
    printf("================================\n");
}
