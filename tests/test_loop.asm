PUSH 5        ; count down from 5 to 0
loop:
PUSH 1
SUB
DUP
JNZ loop
HALT
