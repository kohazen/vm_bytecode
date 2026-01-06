/*
 * labels.h - Label/Symbol Table for the Assembler
 *
 * Manages labels and their addresses using two-pass assembly.
 */

#ifndef LABELS_H
#define LABELS_H

#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"
#include "parser.h"

/* Maximum number of labels */
#define MAX_LABELS 256

/* ============================================
 * LABEL ENTRY
 * ============================================
 */

typedef struct {
    char name[MAX_LABEL_LENGTH];  /* Label name */
    int32_t address;              /* Address in bytecode */
    int line;                     /* Line where defined (for errors) */
    bool defined;                 /* Has this label been defined? */
} LabelEntry;

/* ============================================
 * SYMBOL TABLE
 * ============================================
 */

typedef struct {
    LabelEntry labels[MAX_LABELS];
    int label_count;

    char error_msg[256];
    bool has_error;
} SymbolTable;

/* ============================================
 * FUNCTION DECLARATIONS
 * ============================================ */

/*
 * Initialize the symbol table.
 */
void symtab_init(SymbolTable *table);

/*
 * Pass 1: Collect all labels and compute their addresses.
 * Takes tokens (for label definitions) and parsed instructions (for size).
 */
bool symtab_collect_labels(SymbolTable *table, Token *tokens, int token_count,
                           ParsedInstruction *instructions, int instruction_count);

/*
 * Pass 2: Resolve all label references to addresses.
 */
bool symtab_resolve_labels(SymbolTable *table, ParsedInstruction *instructions,
                           int instruction_count);

/*
 * Look up a label by name.
 * Returns NULL if not found.
 */
LabelEntry* symtab_lookup(SymbolTable *table, const char *name);

/*
 * Print all labels (for debugging).
 */
void symtab_print(SymbolTable *table);

#endif /* LABELS_H */
