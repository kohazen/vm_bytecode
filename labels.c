/*
 * labels.c - Label/Symbol Table Implementation
 *
 * Two-pass assembly: first collect labels, then resolve references.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "labels.h"
#include "instructions.h"

/* ============================================
 * HELPER FUNCTIONS
 * ============================================ */

/*
 * Compare strings case-insensitively.
 */
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

/*
 * Calculate the size of an instruction in bytes.
 */
static int instruction_size(ParsedInstruction *inst) {
    if (inst->has_operand) {
        return 5;  /* 1 byte opcode + 4 bytes operand */
    }
    return 1;      /* Just opcode */
}

/* ============================================
 * PUBLIC FUNCTIONS
 * ============================================ */

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

/*
 * Add a new label to the symbol table.
 */
static bool add_label(SymbolTable *table, const char *name, int32_t address, int line) {
    /* Check for duplicate */
    LabelEntry *existing = symtab_lookup(table, name);
    if (existing && existing->defined) {
        snprintf(table->error_msg, sizeof(table->error_msg),
                 "Line %d: Label '%s' already defined on line %d",
                 line, name, existing->line);
        table->has_error = true;
        return false;
    }

    /* Check capacity */
    if (table->label_count >= MAX_LABELS) {
        snprintf(table->error_msg, sizeof(table->error_msg),
                 "Too many labels (max %d)", MAX_LABELS);
        table->has_error = true;
        return false;
    }

    /* Add the label */
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
    /*
     * Pass 1: Walk through tokens to find label definitions.
     * For each label, compute its address based on the instructions before it.
     *
     * Strategy:
     * - Keep track of which instruction we're at
     * - Keep a running total of bytecode address
     * - When we see a label, record current address
     *
     * Note: A TOKEN_INSTRUCTION that follows another TOKEN_INSTRUCTION
     * is actually a label reference (operand), not a real instruction.
     */

    int current_address = 0;
    int instruction_index = 0;
    bool prev_was_instruction = false;

    for (int i = 0; i < token_count; i++) {
        Token *token = &tokens[i];

        if (token->type == TOKEN_LABEL_DEF) {
            /* This is a label definition - record its address */
            if (!add_label(table, token->text, current_address, token->line)) {
                return false;
            }
            prev_was_instruction = false;
        }
        else if (token->type == TOKEN_INSTRUCTION) {
            /*
             * This is a TOKEN_INSTRUCTION, but it could be:
             * 1. A real instruction (like PUSH, ADD, JMP)
             * 2. A label reference used as an operand (like "loop" in "JMP loop")
             *
             * If the previous token was also an instruction, this is an operand.
             */
            if (!prev_was_instruction) {
                /* This is a real instruction */
                if (instruction_index < instruction_count) {
                    current_address += instruction_size(&instructions[instruction_index]);
                    instruction_index++;
                }
                prev_was_instruction = true;
            } else {
                /* This is an operand (label reference) - don't count it */
                prev_was_instruction = false;
            }
        }
        else if (token->type == TOKEN_NUMBER) {
            /* This is a numeric operand - don't count it */
            prev_was_instruction = false;
        }
        else if (token->type == TOKEN_NEWLINE || token->type == TOKEN_EOF) {
            /* End of line - reset */
            prev_was_instruction = false;
        }
        /* Ignore other token types */
    }

    return true;
}

bool symtab_resolve_labels(SymbolTable *table, ParsedInstruction *instructions,
                           int instruction_count) {
    /*
     * Pass 2: Replace label references with actual addresses.
     */

    for (int i = 0; i < instruction_count; i++) {
        ParsedInstruction *inst = &instructions[i];

        if (inst->is_label_ref) {
            /* Look up the label */
            LabelEntry *entry = symtab_lookup(table, inst->label_name);
            if (!entry) {
                snprintf(table->error_msg, sizeof(table->error_msg),
                         "Line %d: Undefined label '%s'",
                         inst->line, inst->label_name);
                table->has_error = true;
                return false;
            }

            /* Replace with address */
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
