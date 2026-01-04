# Makefile for Day 2 - Parser
#
# Compiles the parser test program.

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# Object files
OBJECTS = main.o lexer.o parser.o

# Executable
TARGET = parser_test

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile main.c
main.o: main.c lexer.h parser.h
	$(CC) $(CFLAGS) -c main.c

# Compile lexer.c
lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Compile parser.c
parser.o: parser.c parser.h lexer.h instructions.h
	$(CC) $(CFLAGS) -c parser.c

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run tests
test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
