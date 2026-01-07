PUSH 10
CALL double
HALT

double:       ; doubles value on stack
DUP
ADD
RET
