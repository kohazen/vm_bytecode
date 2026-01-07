#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static bool emit_byte(CodeGenerator *gen, uint8_t byte) {
    if (gen->bytecode_size >= MAX_BYTECODE_SIZE) {
        snprintf(gen->error_msg, sizeof(gen->error_msg),
                 "Bytecode too large (max %d bytes)", MAX_BYTECODE_SIZE);
        gen->has_error = true;
        return false;
    }
    gen->bytecode[gen->bytecode_size] = byte;
    gen->bytecode_size++;
    return true;
}

static bool emit_int32(CodeGenerator *gen, int32_t value) {
    if (!emit_byte(gen, (uint8_t)(value & 0xFF))) return false;
    if (!emit_byte(gen, (uint8_t)((value >> 8) & 0xFF))) return false;
    if (!emit_byte(gen, (uint8_t)((value >> 16) & 0xFF))) return false;
    if (!emit_byte(gen, (uint8_t)((value >> 24) & 0xFF))) return false;
    return true;
}

static bool write_uint32(FILE *file, uint32_t value) {
    uint8_t bytes[4];
    bytes[0] = (uint8_t)(value & 0xFF);
    bytes[1] = (uint8_t)((value >> 8) & 0xFF);
    bytes[2] = (uint8_t)((value >> 16) & 0xFF);
    bytes[3] = (uint8_t)((value >> 24) & 0xFF);
    return fwrite(bytes, 1, 4, file) == 4;
}

void codegen_init(CodeGenerator *gen) {
    gen->bytecode_size = 0;
    gen->has_error = false;
    gen->error_msg[0] = '\0';
}

bool codegen_generate(CodeGenerator *gen, ParsedInstruction *instructions,
                      int instruction_count) {
    for (int i = 0; i < instruction_count; i++) {
        ParsedInstruction *inst = &instructions[i];

        if (!emit_byte(gen, inst->opcode)) return false;

        if (inst->has_operand) {
            if (inst->is_label_ref) {
                snprintf(gen->error_msg, sizeof(gen->error_msg),
                         "Unresolved label '%s' on line %d",
                         inst->label_name, inst->line);
                gen->has_error = true;
                return false;
            }
            if (!emit_int32(gen, inst->operand)) return false;
        }
    }

    return true;
}

bool codegen_write_file(CodeGenerator *gen, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        snprintf(gen->error_msg, sizeof(gen->error_msg),
                 "Cannot create file '%s'", filename);
        gen->has_error = true;
        return false;
    }

    if (!write_uint32(file, BYTECODE_MAGIC)) {
        snprintf(gen->error_msg, sizeof(gen->error_msg),
                 "Failed to write magic number");
        gen->has_error = true;
        fclose(file);
        return false;
    }

    if (!write_uint32(file, BYTECODE_VERSION)) {
        snprintf(gen->error_msg, sizeof(gen->error_msg),
                 "Failed to write version");
        gen->has_error = true;
        fclose(file);
        return false;
    }

    if (!write_uint32(file, (uint32_t)gen->bytecode_size)) {
        snprintf(gen->error_msg, sizeof(gen->error_msg),
                 "Failed to write code size");
        gen->has_error = true;
        fclose(file);
        return false;
    }

    if (gen->bytecode_size > 0) {
        size_t written = fwrite(gen->bytecode, 1, gen->bytecode_size, file);
        if (written != (size_t)gen->bytecode_size) {
            snprintf(gen->error_msg, sizeof(gen->error_msg),
                     "Failed to write bytecode (wrote %zu of %d bytes)",
                     written, gen->bytecode_size);
            gen->has_error = true;
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

void codegen_print_bytecode(CodeGenerator *gen) {
    printf("=== Bytecode (%d bytes) ===\n", gen->bytecode_size);

    printf("Header:\n");
    printf("  Magic:   0x%08X\n", BYTECODE_MAGIC);
    printf("  Version: 0x%08X\n", BYTECODE_VERSION);
    printf("  Size:    %d bytes\n", gen->bytecode_size);

    printf("\nCode (hex):\n");
    for (int i = 0; i < gen->bytecode_size; i++) {
        if (i % 16 == 0) {
            printf("  %04X: ", i);
        }
        printf("%02X ", gen->bytecode[i]);
        if ((i + 1) % 16 == 0 || i == gen->bytecode_size - 1) {
            printf("\n");
        }
    }

    printf("==========================\n");
}
