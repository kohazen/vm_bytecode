; memory[0] = a (f(n-2)), memory[1] = b (f(n-1)), memory[2] = counter

PUSH 0
STORE 0

PUSH 1
STORE 1

PUSH 10
STORE 2

loop:
LOAD 2
PUSH 0
CMP
LOAD 2
JZ done

LOAD 0
LOAD 1
ADD

LOAD 1
STORE 0

STORE 1

LOAD 2
PUSH 1
SUB
STORE 2

JMP loop

done:
LOAD 0
HALT
