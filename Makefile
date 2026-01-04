# Makefile for VM Day 2
# Builds the virtual machine test program

CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = vm_test
SRCS = main.c vm.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

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
