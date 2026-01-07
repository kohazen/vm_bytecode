# Makefile for VM Day 5
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# Object files
OBJECTS = main.o lexer.o parser.o labels.o codegen.o

# Executable
TARGET = asm_test

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile main.c
main.o: main.c lexer.h parser.h labels.h codegen.h
	$(CC) $(CFLAGS) -c main.c

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

# Run tests
test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
