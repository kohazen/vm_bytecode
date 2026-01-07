#ifndef LABELS_H
#define LABELS_H

#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"
#include "parser.h"

#define MAX_LABELS 256

typedef struct {
    char name[MAX_LABEL_LENGTH];
    int32_t address;
    int line;
    bool defined;
} LabelEntry;

typedef struct {
    LabelEntry labels[MAX_LABELS];
    int label_count;

    char error_msg[256];
    bool has_error;
} SymbolTable;

void symtab_init(SymbolTable *table);
bool symtab_collect_labels(SymbolTable *table, Token *tokens, int token_count,
                           ParsedInstruction *instructions, int instruction_count);
bool symtab_resolve_labels(SymbolTable *table, ParsedInstruction *instructions,
                           int instruction_count);
LabelEntry* symtab_lookup(SymbolTable *table, const char *name);
void symtab_print(SymbolTable *table);

#endif
