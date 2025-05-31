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

typedef enum {
    TYPE_0, // syntax: MNEMONIC{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    TYPE_1, // syntax: MNEMONIC<c> <Rn>, <Rm>{, <shift>}
} Instr_type;

typedef struct {
    Instr_type i_type; // instruction type
    uint8_t special;
    const char *mnemonic;
    Shift shift;
    char shift_str[BUF_20];
    Cond c;
    Register Rd;
    Register Rn;
    Register Rm;
    uint8_t S;
} Instr;



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

//>> layer 1
// Data-processing (register) & miscellaneous instructions
#define IS_DP_OP_0(instr)       ( ( ((instr) >> 25) & 0x7) == 0x0 ) // 0b000
//>> layer 2
// instruction is data-processing (register) or data-processing(register-shifted register)
#define IS_DP_REG_OR_RSR(instr) ( ( ((instr) >> 20) & 0x19) != 0xF ) // not 0b10xx0
//>> layer 3
//=== instr is data-processing (register) ===
#define IS_DP_REG(instr)        ( ( ((instr) >> 4) & 0x1) == 0x0) // 0bxxx0
//>> layer 4
// instr is AND (register) instruction
#define IS_AND_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x0) // 0b0000x
#define IS_EOR_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x2) // 0b0001x
#define IS_SUB_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x4) // 0b0010x
#define IS_RSB_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x6) // 0b0011x
#define IS_ADD_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x8) // 0b0100x
#define IS_ADC_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0xA) // 0b0101x
#define IS_SBC_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0xC) // 0b0110x
#define IS_RSC_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0xE) // 0b0111x
// 0b10xx0
#define IS_TST_REG(instr)       ( ( ((instr) >> 20) & 0x1F) == 0x11) // 0b10001
#define IS_TEQ_REG(instr)       ( ( ((instr) >> 20) & 0x1F) == 0x13) // 0b10011
#define IS_CMP_REG(instr)       ( ( ((instr) >> 20) & 0x1F) == 0x15) // 0b10101
#define IS_CMN_REG(instr)       ( ( ((instr) >> 20) & 0x1F) == 0x17) // 0b10111
#define IS_ORR_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x18) // 0b1100x
//--- move (reg), shift (imm), and rotate (imm) instructions ---
#define IS_MV_OR_SHFT(instr)    ( ( ((instr) >> 20) & 0x1E) == 0x1A) // 0b1101x
#define IS_MOV_REG(instr)       ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ) // 0b00 & 0b00000
#define IS_LSL_IMM(instr)       ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ) // 0b00 & not 0b00000
#define IS_LSR_IMM(instr)       ( ( ((instr) >> 5) & 0x3) == 0x1) // 0b01
#define IS_ASR_IMM(instr)       ( ( ((instr) >> 5) & 0x3) == 0x2) // 0b10
#define IS_RRX(instr)           ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ) // 0b11 & 0b00000
#define IS_ROR_IMM(instr)       ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ) // 0b11 & not 0b00000
//---------------------------------------------------------------
#define IS_BIC_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x1C) // 0b1110x
#define IS_MVN_REG(instr)       ( ( ((instr) >> 20) & 0x1E) == 0x1E) // 0b1111x
//============================================

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