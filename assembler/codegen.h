#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdbool.h>
#include <stdint.h>
#include "parser.h"

#define BYTECODE_MAGIC   0xCAFEBABE
#define BYTECODE_VERSION 0x00000001
#define MAX_BYTECODE_SIZE 65536

typedef struct {
    uint8_t bytecode[MAX_BYTECODE_SIZE];
    int bytecode_size;

    char error_msg[256];
    bool has_error;
} CodeGenerator;

void codegen_init(CodeGenerator *gen);
bool codegen_generate(CodeGenerator *gen, ParsedInstruction *instructions,
                      int instruction_count);
bool codegen_write_file(CodeGenerator *gen, const char *filename);
void codegen_print_bytecode(CodeGenerator *gen);

#endif
