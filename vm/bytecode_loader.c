#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bytecode_loader.h"
#include "vm.h"

static bool read_uint32(FILE *file, uint32_t *value) {
    uint8_t bytes[4];

    if (fread(bytes, 1, 4, file) != 4) {
        return false;
    }

    /* little-endian */
    *value = (uint32_t)bytes[0] |
             ((uint32_t)bytes[1] << 8) |
             ((uint32_t)bytes[2] << 16) |
             ((uint32_t)bytes[3] << 24);

    return true;
}

VMError vm_load_bytecode_file(VM *vm, const char *filename) {
    FILE *file = NULL;
    uint32_t magic, version, code_size;
    uint8_t *code = NULL;

    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return VM_ERROR_FILE_IO;
    }

    if (!read_uint32(file, &magic)) {
        fprintf(stderr, "Error: Cannot read magic number\n");
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    if (magic != BYTECODE_MAGIC) {
        fprintf(stderr, "Error: Invalid bytecode file (bad magic number: 0x%08X, expected 0x%08X)\n",
                magic, BYTECODE_MAGIC);
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    if (!read_uint32(file, &version)) {
        fprintf(stderr, "Error: Cannot read version\n");
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    if (version != BYTECODE_VERSION) {
        fprintf(stderr, "Error: Unsupported bytecode version (got %u, expected %u)\n",
                version, BYTECODE_VERSION);
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    if (!read_uint32(file, &code_size)) {
        fprintf(stderr, "Error: Cannot read code size\n");
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    if (code_size == 0) {
        fprintf(stderr, "Error: Bytecode file has no code\n");
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    code = (uint8_t*)malloc(code_size);
    if (!code) {
        fprintf(stderr, "Error: Cannot allocate memory for bytecode (%u bytes)\n", code_size);
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    size_t bytes_read = fread(code, 1, code_size, file);
    if (bytes_read != code_size) {
        fprintf(stderr, "Error: Expected %u bytes of code, but only read %zu\n",
                code_size, bytes_read);
        free(code);
        fclose(file);
        return VM_ERROR_FILE_IO;
    }

    fclose(file);

    vm_free_bytecode(vm);

    vm->code = code;
    vm->code_size = (int)code_size;
    vm->pc = 0;
    vm->sp = 0;
    vm->rsp = 0;
    vm->running = false;
    vm->error = VM_OK;

    memset(vm->memory, 0, MEMORY_SIZE * sizeof(int32_t));

    return VM_OK;
}

void vm_free_bytecode(VM *vm) {
    if (vm && vm->code) {
        free(vm->code);
        vm->code = NULL;
        vm->code_size = 0;
    }
}
