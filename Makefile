<<<<<<< HEAD
# Makefile for VM Day 2
# Builds the virtual machine test program
=======
# Makefile for Day 1 - Lexer
#
# Compiles the lexer test program.
>>>>>>> master

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

<<<<<<< HEAD
TARGET = vm_test
SRCS = main.c vm.c
OBJS = $(SRCS:.c=.o)
=======
# Object files
OBJECTS = main.o lexer.o

# Executable
TARGET = lexer_test
>>>>>>> master

all: $(TARGET)

<<<<<<< HEAD
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

main.o: main.c vm.h instructions.h
vm.o: vm.c vm.h instructions.h

.PHONY: all run clean
=======
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
>>>>>>> master
