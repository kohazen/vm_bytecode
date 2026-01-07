#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define OP_PUSH  0x01
#define OP_POP   0x02
#define OP_DUP   0x03

#define OP_ADD   0x10
#define OP_SUB   0x11
#define OP_MUL   0x12
#define OP_DIV   0x13
#define OP_CMP   0x14

#define OP_JMP   0x20
#define OP_JZ    0x21
#define OP_JNZ   0x22

#define OP_STORE 0x30
#define OP_LOAD  0x31

#define OP_CALL  0x40
#define OP_RET   0x41

#define OP_HALT  0xFF

#endif
