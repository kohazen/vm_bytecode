# Makefile for Day 5 - Complete Assembler
#
# Compiles the assembler with CLI.

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# Object files
OBJECTS = main.o lexer.o parser.o labels.o codegen.o assembler.o

# Executable
TARGET = asm

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile main.c
main.o: main.c assembler.h
	$(CC) $(CFLAGS) -c main.c

# Compile assembler.c
assembler.o: assembler.c assembler.h lexer.h parser.h labels.h codegen.h
	$(CC) $(CFLAGS) -c assembler.c

# Compile lexer.c
lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Compile parser.c
parser.o: parser.c parser.h lexer.h instructions.h
	$(CC) $(CFLAGS) -c parser.c

# Compile labels.c
labels.o: labels.c labels.h parser.h instructions.h
	$(CC) $(CFLAGS) -c labels.c

# Compile codegen.c
codegen.o: codegen.c codegen.h parser.h
	$(CC) $(CFLAGS) -c codegen.c

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET) *.bc

# Help
help:
	@echo "Assembler Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the assembler"
	@echo "  make clean    - Remove compiled files"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Usage after building:"
	@echo "  ./asm program.asm           - Assemble to program.bc"
	@echo "  ./asm program.asm -o out.bc - Assemble to out.bc"

.PHONY: all clean help
