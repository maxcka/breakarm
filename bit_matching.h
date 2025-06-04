#ifndef DECODE_LOOKUP_H
#define DECODE_LOOKUP_H


#include <stdint.h>

//================================


//... Layer 0 ...
// unconditional instructions
#define IS_COND(instr)       ( ( ((instr) >> 28) & 0xF) != 0xF ) // not 0b1111

//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Data-processing (register) & miscellaneous instructions
#define IS_DP_OP_0(instr)       ( ( ((instr) >> 25) & 0x7) == 0x0 ) // 0b000
//>> layer 2
// instruction is data-processing (register) or data-processing(register-shifted register)
#define IS_DP_REG_OR_RSR(instr) ( ( ((instr) >> 20) & 0x19) != 0x10 ) // not 0b10xx0
//>> layer 3
#define IS_DP_REG(instr)        ( ( ((instr) >> 4) & 0x1) == 0x0) // 0bxxx0
//=== instr is data-processing (register) ===
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
static inline int is_BIC(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x1E) == 0x1C); } // 0b1110x
static inline int is_MVN(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x1E) == 0x1E); } // 0b1111x
//===========================================
//>> layer 3
#define IS_DP_RSR(instr)        ( ( ((instr) >> 4) & 0x9) == 0x1) // 0b0xx1
//=== instr is data-processing (register-shifted register) ===
//>> layer 4
static inline int is_LSL_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x0); } // 0b00
static inline int is_LSR_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x1); } // 0b01
static inline int is_ASR_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2); } // 0b10
static inline int is_ROR_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x3); } // 0b11

//================================

//>> layer 2
// instruction is miscellaneous or halfword multiply and multiply accumulate instruction
#define IS_MISC_OR_HALF_MULT(instr) ( ( ((instr) >> 20) & 0x19) == 0x10 ) // 0b10xx0
//>> layer 3
#define IS_MISC(instr)              ( ( ((instr) >> 4) & 0x8) == 0x0 ) // 0b0xxx
//=== instr is miscellaneous ===
//>> layer 4
static inline int is_MRS_BANKED(uint32_t instr)     { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x1) && ( ( ((instr) >> 21) & 0x1) == 0x0); } // 0b000 & 0b1 & 0bx0
static inline int is_MSR_BANKED(uint32_t instr)     { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x1) && ( ( ((instr) >> 21) & 0x1) == 0x1); } // 0b000 & 0b1 & 0bx1
static inline int is_MRS(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x0) && ( ( ((instr) >> 21) & 0x1) == 0x0); } // 0b000 & 0b0 & 0bx0
static inline int is_MSR_REG_APP(uint32_t instr)    { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x0) && ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 16) & 0x3) == 0x0); } // 0b000 & 0b1 & 0b01 & 0bxx00
static inline int is_MSR_REG_SYS(uint32_t instr)    { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x0) &&     // 0b000 & 0b1
                                                                ( ( ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 16) & 0x3) != 0x0) ) || 
                                                                    ( ( ((instr) >> 21) & 0x3) == 0x3) ); } // (0b01 & not 0bxx00) or (0b11)
static inline int is_BX(uint32_t instr)             { return ( ( ((instr) >> 4) & 0x7) == 0x1) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b001 & 0b01
static inline int is_CLZ(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x1) && ( ( ((instr) >> 21) & 0x3) == 0x3); } // 0b001 & 0b11
static inline int is_BXJ(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x2) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b010 & 0b01
static inline int is_BLX_REG(uint32_t instr)        { return ( ( ((instr) >> 4) & 0x7) == 0x3) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b011 & 0b01
static inline int is_QADD(uint32_t instr)           { return ( ( ((instr) >> 4) & 0x7) == 0x5) && ( ( ((instr) >> 21) & 0x3) == 0x0); } // 0b101 & 0b00
static inline int is_QSUB(uint32_t instr)           { return ( ( ((instr) >> 4) & 0x7) == 0x5) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b101 & 0b01
static inline int is_QDADD(uint32_t instr)          { return ( ( ((instr) >> 4) & 0x7) == 0x5) && ( ( ((instr) >> 21) & 0x3) == 0x2); } // 0b101 & 0b11
static inline int is_QDSUB(uint32_t instr)          { return ( ( ((instr) >> 4) & 0x7) == 0x5) && ( ( ((instr) >> 21) & 0x3) == 0x3); } // 0b101 & 0b11
static inline int is_ERET(uint32_t instr)           { return ( ( ((instr) >> 4) & 0x7) == 0x6) && ( ( ((instr) >> 21) & 0x3) == 0x3); } // 0b110 & 0b11
static inline int is_BKPT(uint32_t instr)           { return ( ( ((instr) >> 4) & 0x7) == 0x7) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b111 & 0b01
static inline int is_HVC(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x7) && ( ( ((instr) >> 21) & 0x3) == 0x2); } // 0b111 & 0b10
static inline int is_SMC(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x7) && ( ( ((instr) >> 21) & 0x3) == 0x3); } // 0b111 & 0b11
//==============================
//>> layer 3
#define IS_HALF_MULT(instr)         ( ( ((instr) >> 4) & 0x9) == 0x8 ) // 0b1xx0
//=== instr is halfword multiply or multiply accumulate ===
//>> layer 4
static inline int is_SMLA(uint32_t instr)           { return ( ( ((instr) >> 21) & 0x3) == 0x0); } // 0b00
static inline int is_SMLAW(uint32_t instr)          { return ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 5) & 0x1) == 0x0); } // 0b01 and 0b0
static inline int is_SMULW(uint32_t instr)          { return ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 5) & 0x1) == 0x1); } // 0b01 and 0b1
static inline int is_SMLAL(uint32_t instr)          { return ( ( ((instr) >> 21) & 0x3) == 0x2); } // 0b10
static inline int is_SMUL(uint32_t instr)           { return ( ( ((instr) >> 21) & 0x3) == 0x3); } // 0b11
//=========================================================

//>> layer 2
// instruction is multiply and multiply accumulate instruction
#define IS_MULT_MULT(instr)         ( ( ( ((instr) >> 20) & 0x10) == 0x0 ) && ( ( ((instr) >> 4) & 0xF) == 0x9 ) ) // 0b0xxxx and 0b1xx0

//=== instr is multiply and multiply accumulate ===
//>> layer 3
static inline int is_MUL(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xE) == 0x0); } // 0b000x
static inline int is_MLA(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xE) == 0x2); } // 0b001x
static inline int is_UMAAL(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xF) == 0x4); } // 0b0100
// UNDEFINED // 0b0101
static inline int is_MLS(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xF) == 0x6); } // 0b0110
// UNDEFINED // 0b0111
static inline int is_UMULL(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xE) == 0x8); } // 0b100x
static inline int is_UMLAL(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xE) == 0xA); } // 0b101x
static inline int is_SMULL(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xE) == 0xC); } // 0b110x
static inline int is_SMLAL(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xE) == 0xE); } // 0b111x
//=================================================

//>> layer 2
// instruction is synchronization primitives
#define IS_SYNC(instr)              ( ( ( ((instr) >> 20) & 0x10) == 0x10 ) && ( ( ((instr) >> 4) & 0xF) == 0x9 ) ) // 0b1xxxx and 0b1xx0

//=== instr is synchronization primitives ===
//>> layer 3
static inline int is_SWP(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xB) == 0x0); } // 0b0x00
static inline int is_STREX(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xF) == 0x8); } // 0b1000
static inline int is_LDREX(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xF) == 0x9); } // 0b1001
static inline int is_STREXD(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xF) == 0xA); } // 0b1010
static inline int is_LDREXD(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xF) == 0xB); } // 0b1011
static inline int is_STREXB(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xF) == 0xC); } // 0b1100
static inline int is_LDREXB(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xF) == 0xD); } // 0b1101
static inline int is_STREXH(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xF) == 0xE); } // 0b1110
static inline int is_LDREXH(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xF) == 0xF); } // 0b1111
//===========================================

//>> layer 2
// instruction is extra load/store instructions
#define IS_EX_LD_STR(instr)         ( ( ( ((instr) >> 20) & 0x12) != 0x2 ) && ( ( ( ((instr) >> 4) & 0xF) == 0xB ) || ( ( ((instr) >> 4) & 0xD) == 0xD ) ) ) // not 0b0xx1x and (0b1011 or 0b11x1)


//=== instr is extra load/store instructions ===
//>> layer 3

//==============================================

//>> layer 2
// instruction is extra load/store instructions, unprivileged
#define IS_EX_LD_STR_UNP(instr)     ( ( ( ((instr) >> 20) & 0x12) == 0x2 ) && ( ( ( ((instr) >> 4) & 0xF) == 0xB ) || ( ( ((instr) >> 4) & 0xD) == 0xD ) ) ) // 0b0xx1x and (0b1011 or 0b11x1)

//=== instr is extra load/store instructions, unprivileged ===
//>> layer 3

//============================================================

//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Data-processing (immediate) & miscellaneous instructions
#define IS_DP_OP_1(instr)       ( ( ((instr) >> 25) & 0x7) == 0x1 ) // 0b001

//>> layer 2
// instruction is data-processing (immediate)
#define IS_DP_IMM(instr)        ( ( ((instr) >> 20) & 0x19) != 0x10 ) // not 0b10xx0

//=== instr is data-processing (immediate) ===
//>> layer 3

//============================================

//>> layer 2
// instruction is 16-bit immediate load (low or high halfword)
#define IS_16_IMM_LD(instr)     ( ( ((instr) >> 20) & 0x1B) == 0x10 ) // 0b10x00

//>> layer 2
// instruction is MSR (immediate) and hints
#define IS_MSR_HINTS(instr)        ( ( ((instr) >> 20) & 0x1B) == 0x12 ) // not 10x10

//=== instr is MSR (immediate) and hints ===
//>> layer 3

//==========================================

//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Load/store word and unsigned byte and media instructions
#define IS_LD_STR_MED(instr)        ( ( ((instr) >> 25) & 0x6) == 0x2 ) // 0b01x

//>> layer 2
// instruction is load/store word and unsigned byte
#define IS_LD_STR(instr)            ( ( ((instr) >> 25) & 0x7) == 0x2 ) || ( ( ( ((instr) >> 25) & 0x7) == 0x3 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ) ) // 0b010 or (0b011 and 0b0)
//======================================
//>> layer 3

//======================================

//>> layer 2
// instruction is media instructions
#define IS_MED(instr)               ( ( ( ((instr) >> 25) & 0x7) == 0x3 ) && ( ( ((instr) >> 4) & 0x1) == 0x1 ) ) // 0b011 and 0b1
//======================================
//>> layer 3

//======================================


//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Branch, branch with link, and block data transfer instructions
#define IS_BR_BLK(instr)            ( ( ((instr) >> 25) & 0x6) == 0x4 ) // 0b10x

//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Coprocessor instructions and Supervisor Call and Floating-point and Advanced SIMD data transfers
#define IS_CO_SPR(instr)            ( ( ((instr) >> 25) & 0x6) == 0x6 ) // 0b11x

//... Layer 0 ...
// unconditional instructions
#define IS_UNCOND(instr)       ( ( ((instr) >> 28) & 0xF) == 0xF ) // 0b1111

// start indices in the proc_instr_table 
#define DP_REG_START 0
#define DP_REG_END 21
#define DP_RSR_START 21
#define DP_RSR_END 40
#define MISC_START 40

// lookup table for processing instructions
extern int (*proc_instr_table[][2])(uint32_t);





#endif