# Makefile for Complete Bytecode VM Project
#
# This Makefile builds both the VM and Assembler, and provides
# targets for running tests and benchmarks.

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# Directories
VM_DIR = vm
ASM_DIR = assembler
TEST_DIR = tests
BENCH_DIR = benchmarks

# VM files
VM_SOURCES = $(VM_DIR)/vm.c $(VM_DIR)/bytecode_loader.c $(VM_DIR)/main.c
VM_OBJECTS = $(VM_DIR)/vm.o $(VM_DIR)/bytecode_loader.o $(VM_DIR)/main.o
VM_TARGET = vm/vm

# Assembler files
ASM_SOURCES = $(ASM_DIR)/lexer.c $(ASM_DIR)/parser.c $(ASM_DIR)/labels.c \
              $(ASM_DIR)/codegen.c $(ASM_DIR)/assembler.c $(ASM_DIR)/main.c
ASM_OBJECTS = $(ASM_DIR)/lexer.o $(ASM_DIR)/parser.o $(ASM_DIR)/labels.o \
              $(ASM_DIR)/codegen.o $(ASM_DIR)/assembler.o $(ASM_DIR)/main.o
ASM_TARGET = assembler/asm

# Test files
TESTS = test_arithmetic test_stack test_comparison test_jump test_conditional \
        test_loop test_memory test_function test_nested_calls factorial fibonacci

# Benchmark files
BENCHMARKS = bench_arithmetic bench_loops bench_functions bench_memory

# ============================================
# Main targets
# ============================================

all: $(VM_TARGET) $(ASM_TARGET)
	@echo ""
	@echo "Build complete!"
	@echo "  VM:        $(VM_TARGET)"
	@echo "  Assembler: $(ASM_TARGET)"
	@echo ""
	@echo "Run 'make tests' to assemble test programs"
	@echo "Run 'make run-tests' to run the test suite"

# ============================================
# VM targets
# ============================================

$(VM_TARGET): $(VM_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(VM_OBJECTS)

$(VM_DIR)/vm.o: $(VM_DIR)/vm.c $(VM_DIR)/vm.h $(VM_DIR)/instructions.h
	$(CC) $(CFLAGS) -c $< -o $@

$(VM_DIR)/bytecode_loader.o: $(VM_DIR)/bytecode_loader.c $(VM_DIR)/bytecode_loader.h $(VM_DIR)/vm.h
	$(CC) $(CFLAGS) -c $< -o $@

$(VM_DIR)/main.o: $(VM_DIR)/main.c $(VM_DIR)/vm.h $(VM_DIR)/bytecode_loader.h
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================
# Assembler targets
# ============================================

$(ASM_TARGET): $(ASM_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(ASM_OBJECTS)

$(ASM_DIR)/lexer.o: $(ASM_DIR)/lexer.c $(ASM_DIR)/lexer.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_DIR)/parser.o: $(ASM_DIR)/parser.c $(ASM_DIR)/parser.h $(ASM_DIR)/lexer.h $(ASM_DIR)/instructions.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_DIR)/labels.o: $(ASM_DIR)/labels.c $(ASM_DIR)/labels.h $(ASM_DIR)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_DIR)/codegen.o: $(ASM_DIR)/codegen.c $(ASM_DIR)/codegen.h $(ASM_DIR)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_DIR)/assembler.o: $(ASM_DIR)/assembler.c $(ASM_DIR)/assembler.h
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM_DIR)/main.o: $(ASM_DIR)/main.c $(ASM_DIR)/assembler.h
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================
# Test and benchmark targets
# ============================================

tests: $(ASM_TARGET)
	@echo "Assembling test programs..."
	@for test in $(TESTS); do \
		echo "  $$test.asm -> $$test.bc"; \
		./$(ASM_TARGET) $(TEST_DIR)/$$test.asm -o $(TEST_DIR)/$$test.bc || exit 1; \
	done
	@echo "All test programs assembled!"

benchmarks: $(ASM_TARGET)
	@echo "Assembling benchmark programs..."
	@for bench in $(BENCHMARKS); do \
		echo "  $$bench.asm -> $$bench.bc"; \
		./$(ASM_TARGET) $(BENCH_DIR)/$$bench.asm -o $(BENCH_DIR)/$$bench.bc || exit 1; \
	done
	@echo "All benchmark programs assembled!"

run-tests: all tests
	@chmod +x run_tests.sh
	@./run_tests.sh ./$(VM_TARGET)

run-benchmarks: all benchmarks
	@chmod +x run_benchmarks.sh
	@./run_benchmarks.sh ./$(VM_TARGET)

# ============================================
# Clean and help
# ============================================

clean:
	rm -f $(VM_OBJECTS) $(ASM_OBJECTS)
	rm -f $(VM_TARGET) $(ASM_TARGET)
	rm -f $(TEST_DIR)/*.bc $(BENCH_DIR)/*.bc

help:
	@echo "Bytecode VM Project Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build VM and Assembler"
	@echo "  make tests        - Assemble test programs"
	@echo "  make benchmarks   - Assemble benchmark programs"
	@echo "  make run-tests    - Run the test suite"
	@echo "  make run-benchmarks - Run benchmarks"
	@echo "  make clean        - Remove compiled files"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./assembler/asm program.asm -o program.bc"
	@echo "  ./vm/vm program.bc"

.PHONY: all tests benchmarks run-tests run-benchmarks clean help
