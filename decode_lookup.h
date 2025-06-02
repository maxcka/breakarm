#ifndef DECODE_LOOKUP_H
#define DECODE_LOOKUP_H


#include <stdint.h>
#include "decode.h"

static const char *core_reg[16] = {
    "R0", "R1", "R2", "R3",
    "R4", "R5", "R6", "R7",
    "R8", "R9", "SL", "FP",
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
#define IS_DP_REG_OR_RSR(instr) ( ( ((instr) >> 20) & 0x19) != 0x10 ) // not 0b10xx0
//>> layer 3
//=== instr is data-processing (register) ===
#define IS_DP_REG(instr)        ( ( ((instr) >> 4) & 0x1) == 0x0) // 0bxxx0
//>> layer 4
static inline int is_AND(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x0); } // 0b0000x
static inline int is_EOR(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x2); } // 0b0001x
static inline int is_SUB(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x4); } // 0b0010x
static inline int is_RSB(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x6); } // 0b0011x
static inline int is_ADD(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x8); } // 0b0100x
static inline int is_ADC(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0xA); } // 0b0101x
static inline int is_SBC(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0xC); } // 0b0110x
static inline int is_RSC(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0xE); } // 0b0111x
// 0b10xx0
static inline int is_TST(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x11); } // 0b10001
static inline int is_TEQ(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x13); } // 0b10011
static inline int is_CMP(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x15); } // 0b10101
static inline int is_CMN(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x17); } // 0b10111
static inline int is_ORR(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x18); } // 0b1100x
//--- move (reg), shift (imm), and rotate (imm) instructions ---
#define IS_MV_OR_SHFT(instr)    ( ( ((instr) >> 20) & 0x1E) == 0x1A) // 0b1101x
static inline int is_MOV_reg(uint32_t instr)      { return ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ); } // 0b00 & 0b00000
static inline int is_LSL_imm(uint32_t instr)      { return ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ); } // 0b00 & not 0b00000
static inline int is_LSR_imm(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x1); } // 0b01
static inline int is_ASR_imm(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2); } // 0b10
static inline int is_RRX(uint32_t instr)          { return ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ); } // 0b11 & 0b00000
static inline int is_ROR_imm(uint32_t instr)      { return ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ); } // 0b11 & not 0b00000
//---------------------------------------------------------------
static inline int is_BIC(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x1C); } // 0b1110x
static inline int is_MVN(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1E) == 0x1E); } // 0b1111x
//================================
//=== instr is data-processing (register-shifted register) ===
#define IS_DP_RSR(instr)        ( ( ((instr) >> 4) & 0x9) == 0x1) // 0b0xx1
//>> layer 4
static inline int is_LSL_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x0); } // 0b00
static inline int is_LSR_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x1); } // 0b01
static inline int is_ASR_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2); } // 0b10
static inline int is_ROR_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x3); } // 0b11

//================================

// start indices in the proc_instr_table 
#define DP_REG_START 0
#define DP_RSR_START 21

// lookup table for processing instructions
static int (*proc_instr_table[][2])(uint32_t) = {
    // data-processing reg
    { is_AND    , AND_instr },
    { is_EOR    , EOR_instr },
    { is_SUB    , SUB_instr },
    { is_RSB    , RSB_instr },
    { is_ADD    , ADD_instr },
    { is_ADC    , ADC_instr },
    { is_SBC    , SBC_instr },
    { is_RSC    , RSC_instr },
    { is_TST    , TST_instr },
    { is_TEQ    , TEQ_instr },
    { is_CMP    , CMP_instr },
    { is_CMN    , CMN_instr },
    { is_ORR    , ORR_instr },
    { is_MOV_reg, MOV_reg_instr },
    { is_LSL_imm, LSL_instr },
    { is_LSR_imm, LSR_instr },
    { is_ASR_imm, ASR_instr },
    { is_RRX    , RRX_instr     },
    { is_ROR_imm, ROR_instr },
    { is_BIC    , BIC_instr },
    { is_MVN    , MVN_instr },
    // data-processing rsr
    { is_AND    , AND_instr },
    { is_EOR    , EOR_instr },
    { is_SUB    , SUB_instr },
    { is_RSB    , RSB_instr },
    { is_ADD    , ADD_instr },
    { is_ADC    , ADC_instr },
    { is_SBC    , SBC_instr },
    { is_RSC    , RSC_instr },
    { is_TST    , TST_instr },
    { is_TEQ    , TEQ_instr },
    { is_CMP    , CMP_instr },
    { is_CMN    , CMN_instr },
    { is_ORR    , ORR_instr },
    { is_LSL_reg, LSL_instr },
    { is_LSR_reg, LSR_instr },
    { is_ASR_reg, ASR_instr },
    { is_ROR_reg, ROR_instr },
    { is_BIC    , BIC_instr },
    { is_MVN    , MVN_instr },

};





#endif