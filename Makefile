# Makefile for Day 3 - Label Resolution
#
# Compiles the label resolution test program.

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

# Object files
OBJECTS = main.o lexer.o parser.o labels.o

# Executable
TARGET = labels_test

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile main.c
main.o: main.c lexer.h parser.h labels.h
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

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run tests
test: $(TARGET)
	./$(TARGET)

.PHONY: all clean test
