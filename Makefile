# Makefile for the Bytecode Virtual Machine
#
# Final version - compiles all VM components
#
# Targets:
#   make        - Build the VM
#   make clean  - Remove generated files
#   make help   - Show available targets

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# All object files
OBJECTS = main.o vm.o bytecode_loader.o

# The final executable
TARGET = vm

# Default target - build the VM
all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile main.c
main.o: main.c vm.h instructions.h bytecode_loader.h
	$(CC) $(CFLAGS) -c main.c

# Compile vm.c
vm.o: vm.c vm.h instructions.h
	$(CC) $(CFLAGS) -c vm.c

# Compile bytecode_loader.c
bytecode_loader.o: bytecode_loader.c bytecode_loader.h vm.h
	$(CC) $(CFLAGS) -c bytecode_loader.c

# Clean up generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Show help
help:
	@echo "Virtual Machine Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the VM executable"
	@echo "  make clean    - Remove compiled files"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Usage after building:"
	@echo "  ./vm <bytecode_file>    - Run a bytecode program"
	@echo "  ./vm --help             - Show VM usage information"

.PHONY: all clean help
