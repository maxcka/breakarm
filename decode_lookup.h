#ifndef DECODE_LOOKUP_H
#define DECODE_LOOKUP_H


#include <stdint.h>
#include "decode.h"

static const char *core_reg[16] = {
    "R0", "R1", "R2", "R3",
    "R4", "R5", "R6", "R7",
    "R8", "R9", "SL", "R11",
    "IP", "SP", "LR", "PC"  // R12 can be called IP or R12
};



static const char *shift_codes[5] = {
    "LSL",
    "LSR",
    "ASR",
    "ROR",
    "RRX"
};





static const char *cond_codes[16] = {
    "EQ",
    "NE",
    "CS",
    "CC",
    "MI",
    "PL",
    "VS",
    "VC",
    "HI",
    "LS",
    "GE",
    "LT",
    "GT",
    "LE",
    ""  // AL is not printed bc it is the default
};

//================================

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
static inline int is_AND_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x0); } // 0b0000x
static inline int is_EOR_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x2); } // 0b0001x
static inline int is_SUB_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x4); } // 0b0010x
static inline int is_RSB_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x6); } // 0b0011x
static inline int is_ADD_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x8); } // 0b0100x
static inline int is_ADC_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0xA); } // 0b0101x
static inline int is_SBC_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0xC); } // 0b0110x
static inline int is_RSC_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0xE); } // 0b0111x
// 0b10xx0
static inline int is_TST_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x11); } // 0b10001
static inline int is_TEQ_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x13); } // 0b10011
static inline int is_CMP_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x15); } // 0b10101
static inline int is_CMN_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x17); } // 0b10111
static inline int is_ORR_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x18); } // 0b1100x
//--- move (reg), shift (imm), and rotate (imm) instructions ---
#define IS_MV_OR_SHFT(instr)    ( ( ((instr) >> 20) & 0x1E) == 0x1A) // 0b1101x
static inline int is_MOV_reg(uint32_t instr)      { return ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ); } // 0b00 & 0b00000
static inline int is_LSL_imm(uint32_t instr)      { return ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ); } // 0b00 & not 0b00000
static inline int is_LSR_imm(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x1); } // 0b01
static inline int is_ASR_imm(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2); } // 0b10
static inline int is_RRX(uint32_t instr)          { return ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ); } // 0b11 & 0b00000
static inline int is_ROR_imm(uint32_t instr)      { return ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ); } // 0b11 & not 0b00000
//---------------------------------------------------------------
static inline int is_BIC_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x1C); } // 0b1110x
static inline int is_MVN_reg(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x1E); } // 0b1111x

//================================

// lookup table for processing instructions
static int (*proc_instr_table[][2])(uint32_t) = {
    { is_AND_reg, AND_reg_instr },
    { is_EOR_reg, EOR_reg_instr },
    { is_SUB_reg, SUB_reg_instr },
    { is_RSB_reg, RSB_reg_instr },
    { is_ADD_reg, ADD_reg_instr },
    { is_ADC_reg, ADC_reg_instr },
    { is_SBC_reg, SBC_reg_instr },
    { is_RSC_reg, RSC_reg_instr },
    { is_TST_reg, TST_reg_instr },
    { is_TEQ_reg, TEQ_reg_instr },
    { is_CMP_reg, CMP_reg_instr },
    { is_CMN_reg, CMN_reg_instr },
    { is_ORR_reg, ORR_reg_instr }
};





#endif