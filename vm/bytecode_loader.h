#ifndef BYTECODE_LOADER_H
#define BYTECODE_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "vm.h"

#define BYTECODE_MAGIC 0xCAFEBABE
#define BYTECODE_VERSION 0x00000001

VMError vm_load_bytecode_file(VM *vm, const char *filename);
void vm_free_bytecode(VM *vm);

#endif
