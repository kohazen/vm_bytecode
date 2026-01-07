PUSH 10
CALL quadruple
HALT

double:
DUP
ADD
RET

quadruple:    ; calls double twice
CALL double
CALL double
RET
