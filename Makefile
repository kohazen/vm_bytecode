# Makefile for Day 1 - Lexer
#
# Compiles the lexer test program.

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# Object files
OBJECTS = main.o lexer.o

# Executable
TARGET = lexer_test

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile main.c
main.o: main.c lexer.h
	$(CC) $(CFLAGS) -c main.c

# Compile lexer.c
lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run tests
test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
