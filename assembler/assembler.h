#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdbool.h>

#define MAX_SOURCE_SIZE 65536

typedef struct {
    bool success;
    int instruction_count;
    int bytecode_size;
    int label_count;
    char error_msg[512];
} AssemblerResult;

AssemblerResult assemble_file(const char *input_file, const char *output_file);
AssemblerResult assemble_string(const char *source, const char *output_file);
void print_usage(const char *program_name);

#endif
