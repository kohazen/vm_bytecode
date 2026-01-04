/*
 * parser.h - Parser for the Assembler
 *
 * The parser takes tokens from the lexer and converts them into
 * a list of parsed instructions with their opcodes and operands.
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include "lexer.h"

/* Maximum number of instructions we can handle */
#define MAX_INSTRUCTIONS 1024

/* Maximum length of a label name */
#define MAX_LABEL_LENGTH 64

/* ============================================
 * OPCODE TABLE ENTRY
 * ============================================
 * Maps instruction names to their opcodes.
 */

typedef struct {
    const char *name;      /* Instruction name (e.g., "PUSH") */
    uint8_t opcode;        /* Opcode value (e.g., 0x01) */
    bool has_operand;      /* Does this instruction take an operand? */
} OpcodeEntry;

/* ============================================
 * PARSED INSTRUCTION
 * ============================================
 * Represents a single parsed instruction.
 */

typedef struct {
    uint8_t opcode;                    /* The opcode */
    bool has_operand;                  /* Whether it has an operand */
    int32_t operand;                   /* The operand value (if any) */
    bool is_label_ref;                 /* Is the operand a label reference? */
    char label_name[MAX_LABEL_LENGTH]; /* Label name (if is_label_ref) */
    int line;                          /* Source line number */
} ParsedInstruction;

/* ============================================
 * PARSER STRUCTURE
 * ============================================
 */

typedef struct {
    Token *tokens;         /* Array of tokens from lexer */
    int token_count;       /* Number of tokens */
    int pos;               /* Current position in token array */

    ParsedInstruction instructions[MAX_INSTRUCTIONS];
    int instruction_count;

    char error_msg[256];
    bool has_error;
} Parser;

/* ============================================
 * FUNCTION DECLARATIONS
 * ============================================ */

/*
 * Initialize the parser with tokens from the lexer.
 */
void parser_init(Parser *parser, Token *tokens, int token_count);

/*
 * Parse all tokens into instructions.
 * Returns true on success, false on error.
 */
bool parser_parse(Parser *parser);

/*
 * Look up an instruction name in the opcode table.
 * Returns NULL if not found.
 */
const OpcodeEntry* lookup_opcode(const char *name);

/*
 * Print all parsed instructions (for debugging).
 */
void parser_print_instructions(Parser *parser);

#endif /* PARSER_H */
