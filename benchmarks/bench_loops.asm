; nested loops: 100 outer x 100 inner = 10,000 iterations

PUSH 0
STORE 2

PUSH 100
STORE 0

outer_loop:
PUSH 100
STORE 1

inner_loop:
LOAD 2
PUSH 1
ADD
STORE 2

LOAD 1
PUSH 1
SUB
DUP
STORE 1
JNZ inner_loop

LOAD 0
PUSH 1
SUB
DUP
STORE 0
JNZ outer_loop

LOAD 2
HALT
