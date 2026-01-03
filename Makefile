# Makefile for VM Day 1
# Builds the virtual machine test program

CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable
TARGET = vm_test

# Source files
SRCS = main.c vm.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the tests
run: $(TARGET)
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Dependencies
main.o: main.c vm.h instructions.h
vm.o: vm.c vm.h instructions.h

.PHONY: all run clean
