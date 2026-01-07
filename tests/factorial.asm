; memory[0] = counter, memory[1] = result

PUSH 5
STORE 0

PUSH 1
STORE 1

loop:
LOAD 1
LOAD 0
MUL
STORE 1

LOAD 0
PUSH 1
SUB
DUP
STORE 0

JNZ loop

LOAD 1
HALT
