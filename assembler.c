/*
 * assembler.c - Main Assembler Implementation
 *
 * Ties together all the components: lexer, parser, labels, codegen.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "lexer.h"
#include "parser.h"
#include "labels.h"
#include "codegen.h"

/*
 * Read entire file into a string.
 * Caller must free the returned string.
 */
static char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size <= 0 || size > MAX_SOURCE_SIZE) {
        fclose(file);
        return NULL;
    }

    /* Allocate buffer */
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    /* Read file */
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';

    fclose(file);
    return buffer;
}

AssemblerResult assemble_string(const char *source, const char *output_file) {
    AssemblerResult result;
    result.success = false;
    result.instruction_count = 0;
    result.bytecode_size = 0;
    result.label_count = 0;
    result.error_msg[0] = '\0';

    /* Step 1: Tokenize */
    Lexer lexer;
    lexer_init(&lexer, source);

    if (!lexer_tokenize(&lexer)) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Lexer error: %s", lexer.error_msg);
        return result;
    }

    /* Step 2: Parse */
    Parser parser;
    parser_init(&parser, lexer.tokens, lexer.token_count);

    if (!parser_parse(&parser)) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Parser error: %s", parser.error_msg);
        return result;
    }

    result.instruction_count = parser.instruction_count;

    /* Step 3: Collect labels (Pass 1) */
    SymbolTable symtab;
    symtab_init(&symtab);

    if (!symtab_collect_labels(&symtab, lexer.tokens, lexer.token_count,
                               parser.instructions, parser.instruction_count)) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Label error: %s", symtab.error_msg);
        return result;
    }

    result.label_count = symtab.label_count;

    /* Step 4: Resolve labels (Pass 2) */
    if (!symtab_resolve_labels(&symtab, parser.instructions, parser.instruction_count)) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Label error: %s", symtab.error_msg);
        return result;
    }

    /* Step 5: Generate bytecode */
    CodeGenerator codegen;
    codegen_init(&codegen);

    if (!codegen_generate(&codegen, parser.instructions, parser.instruction_count)) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Codegen error: %s", codegen.error_msg);
        return result;
    }

    result.bytecode_size = codegen.bytecode_size;

    /* Step 6: Write to file */
    if (!codegen_write_file(&codegen, output_file)) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "File error: %s", codegen.error_msg);
        return result;
    }

    result.success = true;
    return result;
}

AssemblerResult assemble_file(const char *input_file, const char *output_file) {
    AssemblerResult result;
    result.success = false;
    result.instruction_count = 0;
    result.bytecode_size = 0;
    result.label_count = 0;

    /* Read the source file */
    char *source = read_file(input_file);
    if (!source) {
        snprintf(result.error_msg, sizeof(result.error_msg),
                 "Cannot read file '%s'", input_file);
        return result;
    }

    /* Assemble it */
    result = assemble_string(source, output_file);

    /* Clean up */
    free(source);

    return result;
}

void print_usage(const char *program_name) {
    printf("Usage: %s <input.asm> [-o <output.bc>]\n", program_name);
    printf("\n");
    printf("Assembles an assembly source file into bytecode.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -o <file>   Specify output file (default: input with .bc extension)\n");
    printf("  -h, --help  Show this help message\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s program.asm              # Creates program.bc\n", program_name);
    printf("  %s program.asm -o out.bc    # Creates out.bc\n", program_name);
}
