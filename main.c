/*
 * main.c - Assembler Command-Line Interface
 *
 * Day 5: Complete CLI for the assembler.
 *
 * Usage: asm <input.asm> [-o <output.bc>]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

/*
 * Generate output filename by replacing extension with .bc
 */
static void make_output_filename(const char *input, char *output, size_t size) {
    strncpy(output, input, size - 1);
    output[size - 1] = '\0';

    /* Find the last dot */
    char *dot = strrchr(output, '.');
    char *slash = strrchr(output, '/');

    /* Only replace if dot is after last slash (or no slash) */
    if (dot && (!slash || dot > slash)) {
        strcpy(dot, ".bc");
    } else {
        /* No extension - just append .bc */
        size_t len = strlen(output);
        if (len + 3 < size) {
            strcat(output, ".bc");
        }
    }
}

int main(int argc, char *argv[]) {
    const char *input_file = NULL;
    const char *output_file = NULL;
    char default_output[256];

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -o requires a filename\n");
                return 1;
            }
            output_file = argv[++i];
        }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
        else {
            if (input_file) {
                fprintf(stderr, "Error: Multiple input files not supported\n");
                return 1;
            }
            input_file = argv[i];
        }
    }

    /* Check for required input file */
    if (!input_file) {
        fprintf(stderr, "Error: No input file specified\n\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Generate default output filename if not specified */
    if (!output_file) {
        make_output_filename(input_file, default_output, sizeof(default_output));
        output_file = default_output;
    }

    /* Assemble */
    printf("Assembling: %s\n", input_file);

    AssemblerResult result = assemble_file(input_file, output_file);

    if (result.success) {
        printf("Output:     %s\n", output_file);
        printf("\n");
        printf("Assembly successful!\n");
        printf("  Instructions: %d\n", result.instruction_count);
        printf("  Labels:       %d\n", result.label_count);
        printf("  Bytecode:     %d bytes (+ 12 byte header)\n", result.bytecode_size);
        return 0;
    } else {
        fprintf(stderr, "\nAssembly failed!\n");
        fprintf(stderr, "%s\n", result.error_msg);
        return 1;
    }
}
