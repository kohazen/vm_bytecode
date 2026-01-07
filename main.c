/*
 * main.c - Virtual Machine Entry Point
 *
 * Final version with complete CLI interface.
 *
 * Usage: ./vm <bytecode_file>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "bytecode_loader.h"

/*
 * Print usage information
 */
static void print_usage(const char *program_name) {
    printf("Usage: %s <bytecode_file>\n", program_name);
    printf("\n");
    printf("Runs a bytecode program on the virtual machine.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("\n");
    printf("Bytecode file format:\n");
    printf("  - Magic number: 0xCAFEBABE (4 bytes)\n");
    printf("  - Version: 1 (4 bytes)\n");
    printf("  - Code size: N (4 bytes, little-endian)\n");
    printf("  - Code: N bytes of bytecode instructions\n");
}

/*
 * Run a bytecode file and return the result
 */
static int run_bytecode_file(const char *filename) {
    /* Create the VM */
    VM *vm = vm_create();
    if (!vm) {
        fprintf(stderr, "Error: Failed to create VM\n");
        return 1;
    }

    /* Load the bytecode file */
    printf("Loading: %s\n", filename);
    VMError load_result = vm_load_bytecode_file(vm, filename);
    if (load_result != VM_OK) {
        fprintf(stderr, "Error: Failed to load bytecode: %s\n", vm_error_string(load_result));
        vm_destroy(vm);
        return 1;
    }

    printf("Loaded %d bytes of bytecode\n", vm->code_size);
    printf("\n");

    /* Run the program */
    printf("Running...\n");
    VMError run_result = vm_run(vm);

    printf("\n");
    printf("=== Execution Complete ===\n");

    if (run_result != VM_OK) {
        printf("Error: %s\n", vm_error_string(run_result));
    } else {
        printf("Status: OK\n");
    }

    /* Show result */
    if (vm->sp > 0) {
        printf("Result (top of stack): %d\n", vm->stack[vm->sp - 1]);
    } else {
        printf("Result: (stack is empty)\n");
    }

    /* Show final state */
    printf("\n");
    vm_dump_state(vm);

    /* Clean up */
    vm_destroy(vm);

    return (run_result == VM_OK) ? 0 : 1;
}

int main(int argc, char *argv[]) {
    /* Check for arguments */
    if (argc < 2) {
        fprintf(stderr, "Error: No bytecode file specified.\n\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Check for help flag */
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    /* Run the specified bytecode file */
    return run_bytecode_file(argv[1]);
}
