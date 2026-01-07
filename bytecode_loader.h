/*
 * bytecode_loader.h - Bytecode File Loading
 *
 * Handles reading bytecode files and loading them into the VM.
 */

#ifndef BYTECODE_LOADER_H
#define BYTECODE_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include "vm.h"

/*
 * Bytecode file format:
 *
 * [Header - 12 bytes]
 *   Magic Number (4 bytes): 0xCAFEBABE
 *   Version (4 bytes):      0x00000001
 *   Code Size (4 bytes):    number of bytecode bytes
 *
 * [Code Section - variable]
 *   Raw bytecode instructions
 */

/* Magic number to identify our bytecode files */
#define BYTECODE_MAGIC 0xCAFEBABE

/* Current bytecode version */
#define BYTECODE_VERSION 0x00000001

/*
 * Load bytecode from a file into the VM.
 *
 * Parameters:
 *   vm       - The virtual machine to load into
 *   filename - Path to the bytecode file
 *
 * Returns:
 *   VM_OK on success, or an error code on failure
 *
 * Note: The VM takes ownership of the loaded bytecode memory.
 *       It will be freed when vm_destroy() is called.
 */
VMError vm_load_bytecode_file(VM *vm, const char *filename);

/*
 * Free the bytecode memory that was loaded from a file.
 * Called automatically by vm_destroy(), but can also be called manually.
 */
void vm_free_bytecode(VM *vm);

#endif /* BYTECODE_LOADER_H */
