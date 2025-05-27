#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>


//===================================================

#define BUF_20 20

// A32 instructions are 4 bytes wide
#define A32_INSTR_SIZE 4

typedef enum {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    SL,
    R11,
    IP,
    SP,
    LR,
    PC
} Register;

// type enum
typedef enum {
    LSL, // logical shift left
    LSR, // logical shift right
    ASR, // arithmetic shift right
    ROR, // rotate right
    RRX  // rotate right one bit, with extend
} ShiftType;

typedef struct {
    ShiftType shift_t;    // shift type
    uint8_t shift_n;    // shift number of bits
} Shift;

// condition enum
typedef enum {
    EQ,
    NE,
    CS,
    CC,
    MI,
    PL,
    VS,
    VC,
    HI,
    LS,
    GE,
    LT,
    GT,
    LE,
    AL
} Cond;

//==========================================
//==========================================


// decoding

// design:
// - check bits until we know what instruction it is
// - process the instruction in a function
// development:
// - decode as if we are doing DFS

//===================

//=== cond field (bits 31 - 28)

// |31 30 29 28|27 26 25|24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5| 4 |3 2 1 0|
// |  cond     |  op1   |                                                      |op |       |

#define UNCOND  // 0b111

//===

//=== op1 field (bits 27 - 25)
// Note: because Branch and Coprocessor / Supervisor instructions only use bits 27 and 26,
// only check those first

//--> mask bits 27-25
// Data-processing (register)
//>> layer 1
#define IS_DP_OP_0(instr)       ( ( ((instr) >> 25) & 0x7) == 0x0 ) // 0b000
//>> layer 2
// instruction is data-processing (register) or data-processing(register-shifted register)
#define IS_DP_REG_OR_RSR(instr) ( ( ((instr) >> 20) & 0x19) != 0xF ) // not 0b10xx0
//>> layer 3
// instr is data-processing (register)
#define IS_DP_REG(instr)        ( ( ((instr) >> 4) & 0x1) == 0x0) // 0bxxx0
//>> layer 4
// instr is AND (register) instruction
#define IS_AND_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x0) // 0b0000x
//#define IS_EOR_REG(instr)
//#define IS_SUB_REG(instr)
//#define IS_RSB_REG(instr)
#define IS_ADD_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x8) // 0b0100x
//=======================


//===================================================
//===================================================



// need comments for fn declarations

// auxiliary functions
Shift decode_imm_shift(ShiftType type, uint8_t imm5);
void get_shift_str(Shift shift, char *shift_str, int buf_sz);

// instruction processing functions
void AND_reg_instr(uint32_t instr);

// main functions
void decode_dp_reg(uint32_t instr);

void decode_instr(uint32_t instr);




#endif