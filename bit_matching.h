#ifndef BIT_MATCHING_H
#define BIT_MATCHING_H


#include <stdint.h>

//================================

//------------------------
// skipping Advanced SIMD and Floating Point instructions
//------------------------


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
static inline int is_AND(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x0); } // 0b0000x
static inline int is_EOR(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x2); } // 0b0001x
static inline int is_SUB(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x4); } // 0b0010x
static inline int is_RSB(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x6); } // 0b0011x
static inline int is_ADD(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x8); } // 0b0100x
static inline int is_ADC(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0xA); } // 0b0101x
static inline int is_SBC(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0xC); } // 0b0110x
static inline int is_RSC(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0xE); } // 0b0111x
// 0b10xx0
static inline int is_TST(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1F) == 0x11); } // 0b10001
static inline int is_TEQ(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1F) == 0x13); } // 0b10011
static inline int is_CMP(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1F) == 0x15); } // 0b10101
static inline int is_CMN(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1F) == 0x17); } // 0b10111
static inline int is_ORR(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x18); } // 0b1100x
//--- move (reg), shift (imm), and rotate (imm) instructions ---
#define IS_MV_OR_SHFT(instr)    ( ( ((instr) >> 20) & 0x1E) == 0x1A) // 0b1101x
static inline int is_MOV_reg(uint32_t instr)    { return ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ); } // 0b00 & 0b00000
static inline int is_LSL_imm(uint32_t instr)    { return ( ( ( ((instr) >> 5) & 0x3) == 0x0) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ); } // 0b00 & not 0b00000
static inline int is_LSR_imm(uint32_t instr)    { return ( ( ((instr) >> 5) & 0x3) == 0x1); } // 0b01
static inline int is_ASR_imm(uint32_t instr)    { return ( ( ((instr) >> 5) & 0x3) == 0x2); } // 0b10
static inline int is_RRX(uint32_t instr)        { return ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) == 0x0) ); } // 0b11 & 0b00000
static inline int is_ROR_imm(uint32_t instr)    { return ( ( ( ((instr) >> 5) & 0x3) == 0x3) && ( ( ((instr) >> 7) & 0x1F) != 0x0) ); } // 0b11 & not 0b00000
//---------------------------------------------------------------
static inline int is_BIC(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x1C); } // 0b1110x
static inline int is_MVN(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x1E); } // 0b1111x
//===========================================
//>> layer 3
#define IS_DP_RSR(instr)        ( ( ((instr) >> 4) & 0x9) == 0x1) // 0b0xx1
//=== instr is data-processing (register-shifted register) ===
//>> layer 4
// also uses some data-proc (register) inline functions
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
static inline int is_MSR_reg_app(uint32_t instr)    { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x0) && ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 16) & 0x3) == 0x0); } // 0b000 & 0b1 & 0b01 & 0bxx00
static inline int is_MSR_reg_sys(uint32_t instr)    { return ( ( ((instr) >> 4) & 0x7) == 0x0) && ( ( ((instr) >> 9) & 0x1) == 0x0) &&     // 0b000 & 0b1
                                                                ( ( ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 16) & 0x3) != 0x0) ) || 
                                                                    ( ( ((instr) >> 21) & 0x3) == 0x3) ); } // (0b01 & not 0bxx00) or (0b11)
static inline int is_BX(uint32_t instr)             { return ( ( ((instr) >> 4) & 0x7) == 0x1) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b001 & 0b01
static inline int is_CLZ(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x1) && ( ( ((instr) >> 21) & 0x3) == 0x3); } // 0b001 & 0b11
static inline int is_BXJ(uint32_t instr)            { return ( ( ((instr) >> 4) & 0x7) == 0x2) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b010 & 0b01
static inline int is_BLX_reg(uint32_t instr)        { return ( ( ((instr) >> 4) & 0x7) == 0x3) && ( ( ((instr) >> 21) & 0x3) == 0x1); } // 0b011 & 0b01
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
//=== instr is halfword multiply and multiply accumulate ===
//>> layer 4
static inline int is_SMLA(uint32_t instr)           { return ( ( ((instr) >> 21) & 0x3) == 0x0); } // 0b00
static inline int is_SMLAW(uint32_t instr)          { return ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 5) & 0x1) == 0x0); } // 0b01 and 0b0
static inline int is_SMULW(uint32_t instr)          { return ( ( ((instr) >> 21) & 0x3) == 0x1) && ( ( ((instr) >> 5) & 0x1) == 0x1); } // 0b01 and 0b1
static inline int is_SMLALXY(uint32_t instr)          { return ( ( ((instr) >> 21) & 0x3) == 0x2); } // 0b10
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
static inline int is_UNDEF(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xF) == 0x5); }// UNDEFINED // 0b0101
static inline int is_MLS(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xF) == 0x6); } // 0b0110
static inline int is_UNDEF_2(uint32_t instr)        { return ( ( ((instr) >> 20) & 0xF) == 0x7); }// UNDEFINED // 0b0111
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
static inline int is_STRH_reg(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x5) == 0x0 ); } //0b01 and 0bxx0x0
static inline int is_LDRH_reg(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x5) == 0x1 ); } //0b01 and 0bxx0x1
static inline int is_STRH_imm(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x5) == 0x4 ); } //0b01 and 0bxx1x0
static inline int is_LDRH_imm(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } //0b01 and 0bxx1x1 and not 0b1111
static inline int is_LDRH_lit(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } //0b01 and 0bxx1x1 and 0b1111

static inline int is_LDRD_reg(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x5) == 0x0 ); } //0b10 and 0bxx0x0
static inline int is_LDRSB_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x5) == 0x1 ); } //0b10 and 0bxx0x1
static inline int is_LDRD_imm(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x5) == 0x4 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } //0b10 and 0bxx1x0 and not 0b1111
static inline int is_LDRD_lit(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x5) == 0x4 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } //0b10 and 0bxx1x0 and 0b1111
static inline int is_LDRSB_imm(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } //0b10 and 0bxx1x1 and not 0b1111
static inline int is_LDRSB_lit(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } //0b10 and 0bxx1x1 and 0b1111

static inline int is_STRD_reg(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x3 ) && ( ( ((instr) >> 20) & 0x5) == 0x0 ); } //0b11 and 0bxx0x0
static inline int is_LDRSH_reg(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x3 ) && ( ( ((instr) >> 20) & 0x5) == 0x1 ); } //0b11 and 0bxx0x1
static inline int is_STRD_imm(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x3) == 0x3 ) && ( ( ((instr) >> 20) & 0x5) == 0x4 ); } //0b11 and 0bxx1x0
static inline int is_LDRSH_imm(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x3 ) && ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } //0b11 and 0bxx1x1 and not 0b1111
static inline int is_LDRSH_lit(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x3) == 0x3 ) && ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } //0b11 and 0bxx1x1 and 0b1111
//==============================================

//>> layer 2
// instruction is extra load/store instructions, unprivileged
#define IS_EX_LD_STR_UNP(instr)     ( ( ( ((instr) >> 20) & 0x12) == 0x2 ) && ( ( ( ((instr) >> 4) & 0xF) == 0xB ) || ( ( ((instr) >> 4) & 0xD) == 0xD ) ) ) // 0b0xx1x and (0b1011 or 0b11x1)

//=== instr is extra load/store instructions, unprivileged ===
//>> layer 3
static inline int is_STRHT(uint32_t instr)          { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x1) == 0x0 ); } //0b01 and 0b0
static inline int is_LDRHT(uint32_t instr)          { return ( ( ((instr) >> 5) & 0x3) == 0x1 ) && ( ( ((instr) >> 20) & 0x1) == 0x1 ); } //0b01 and 0b1
static inline int is_UNPRED(uint32_t instr)         { return ( ( ((instr) >> 5) & 0x2) == 0x2 ) && ( ( ((instr) >> 20) & 0x1) == 0x0 ) && ( ( ((instr) >> 12) & 0x1) == 0x0 ); } //0b01 and 0b1 and 0bxxx0
static inline int is_UNDEF_3(uint32_t instr)        { return ( ( ((instr) >> 5) & 0x2) == 0x2 ) && ( ( ((instr) >> 20) & 0x1) == 0x0 ) && ( ( ((instr) >> 12) & 0x1) == 0x1 ); } //0b01 and 0b1 and 0bxxx1
static inline int is_LDRSBT(uint32_t instr)         { return ( ( ((instr) >> 5) & 0x3) == 0x2 ) && ( ( ((instr) >> 20) & 0x1) == 0x1 ); } //0b10 and 0b1
static inline int is_LDRSHT(uint32_t instr)         { return ( ( ((instr) >> 5) & 0x3) == 0x3 ) && ( ( ((instr) >> 20) & 0x1) == 0x1 ); } //0b11 and 0b1
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
// also uses some data-proc (register) inline functions
static inline int is_SUB_imm(uint32_t instr)        { return is_SUB(instr) && ( ( ((instr) >> 16) & 0xF) != 0xF); } // 0b0010x and not 0b1111
static inline int is_ADR(uint32_t instr)            { return is_SUB(instr) && ( ( ((instr) >> 16) & 0xF) == 0xF); } // 0b0010x and 0b1111
static inline int is_ADD_imm(uint32_t instr)        { return is_SUB(instr) && ( ( ((instr) >> 16) & 0xF) != 0xF); } // 0b0100x and not 0b1111
static inline int is_ADR_2(uint32_t instr)          { return is_SUB(instr) && ( ( ((instr) >> 16) & 0xF) == 0xF); } // 0b0100x and 0b1111
static inline int is_MOV_imm(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x1E) == 0x1A); } // 0b1101x


//============================================

//>> layer 2
// instruction is 16-bit immediate load (low or high halfword)
#define IS_16_IMM_LD(instr)     ( ( ((instr) >> 20) & 0x1B) == 0x10 ) // 0b10x00
//=== instr is 16-bit immediate load (low or high halfword) ===
//>> layer 3
static inline int is_MOV_imm_2(uint32_t instr)      { return ( ( ((instr) >> 20) & 0x1F) == 0x10 ); } // 0b10000
static inline int is_MOVT(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x1F) == 0x14 ); } // 0b10100
//=============================================================

//>> layer 2
// instruction is MSR (immediate) and hints
#define IS_MSR_HINTS(instr)        ( ( ((instr) >> 20) & 0x1B) == 0x12 ) // not 10x10
//=== instr is MSR (immediate) and hints ===
//>> layer 3
static inline int is_NOP(uint32_t instr)            { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && ( ( ((instr) >> 16) & 0xF) == 0x0) && ( ( ((instr) >> 0) & 0xFF) == 0x0); } // 0b0 and 0b0000 and 0b00000000
static inline int is_YIELD(uint32_t instr)          { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && ( ( ((instr) >> 16) & 0xF) == 0x0) && ( ( ((instr) >> 0) & 0xFF) == 0x1); } // 0b0 and 0b0000 and 0b00000001
static inline int is_WFE(uint32_t instr)            { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && ( ( ((instr) >> 16) & 0xF) == 0x0) && ( ( ((instr) >> 0) & 0xFF) == 0x2); } // 0b0 and 0b0000 and 0b00000010
static inline int is_WFI(uint32_t instr)            { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && ( ( ((instr) >> 16) & 0xF) == 0x0) && ( ( ((instr) >> 0) & 0xFF) == 0x3); } // 0b0 and 0b0000 and 0b00000011
static inline int is_SEV(uint32_t instr)            { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && ( ( ((instr) >> 16) & 0xF) == 0x0) && ( ( ((instr) >> 0) & 0xFF) == 0x4); } // 0b0 and 0b0000 and 0b00000100
static inline int is_DBG(uint32_t instr)            { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && ( ( ((instr) >> 16) & 0xF) == 0x0) && ( ( ((instr) >> 0) & 0xFF) == 0xF0); } // 0b0 and 0b0000 and 0b1111xxxx
static inline int is_MSR_imm_app(uint32_t instr)    { return ( ( ((instr) >> 22) & 0x1) == 0x0 ) && 
                                                                ( ( ( ((instr) >> 16) & 0xF) == 0x4) || ( ( ((instr) >> 16) & 0xB) == 0x8) ); } // 0b0 and (0100 or 1x00)
static inline int is_MSR_imm_sys(uint32_t instr)    { return ( ( ( ((instr) >> 22) & 0x1) == 0x0 ) && 
                                                                ( ( ( ((instr) >> 16) & 0x3) == 0x1) || ( ( ((instr) >> 16) & 0x2) == 0x2) ) ) ||
                                                                ( ( ((instr) >> 22) & 0x1) == 0x1 ); } // (0b0 and (xx01 or xx1x)) or 0b1
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
static inline int is_STR_imm(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) != 0x2 ) ); } // 0b0 and (0bxx0x0 and not 0b0x010)
static inline int is_STR_reg(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) != 0x2 ) ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b1 and (0bxx0x0 and not 0b0x010) and 0b0
static inline int is_STRT(uint32_t instr)                    { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) == 0x2 ); } // 0b0 and 0b0x010
static inline int is_STRT_2(uint32_t instr)                  { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) == 0x2 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b0 and 0b0x010 and 0b0

static inline int is_LDR_imm(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) != 0x3 ) ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } // 0b0 and (0bxx0x1 and not 0b0x011) and not 0b1111
static inline int is_LDR_lit(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) != 0x3 ) ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } // 0b0 and (0bxx0x1 and not 0b0x011) and 0b1111
static inline int is_LDR_reg(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) != 0x3 ) ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b1 and (0bxx0x1 and not 0b0x011) and 0b0
static inline int is_LDRT(uint32_t instr)                    { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) == 0x3 ); } // 0b0 and 0b0x010
static inline int is_LDRT_2(uint32_t instr)                  { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) == 0x3 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b0 and 0b0x010 and 0b0

static inline int is_STRB_imm(uint32_t instr)                { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x4 ) && ( ( ((instr) >> 20) & 0x17) != 0x6 ) ); } // 0b0 and (0bxx1x0 and not 0b0x110)
static inline int is_STRB_reg(uint32_t instr)                { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) != 0x2 ) ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b1 and (0bxx1x0 and not 0b0x110) and 0b0
static inline int is_STRBT(uint32_t instr)                   { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) == 0x6 ); } // 0b0 and 0b0x110
static inline int is_STRBT_2(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) == 0x6 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b0 and 0b0x110 and 0b0

static inline int is_LDRB_imm(uint32_t instr)                { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 20) & 0x17) != 0x7 ) ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } // 0b0 and (0bxx1x1 and not 0b0x111) and not 0b1111
static inline int is_LDRB_lit(uint32_t instr)                { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 20) & 0x17) != 0x7 ) ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } // 0b0 and (0bxx1x1 and not 0b0x111) and 0b1111
static inline int is_LDRB_reg(uint32_t instr)                { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ( ((instr) >> 20) & 0x5) == 0x5 ) && ( ( ((instr) >> 20) & 0x17) != 0x7 ) ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b1 and (0bxx1x1 and not 0b0x111) and 0b0
static inline int is_LDRBT(uint32_t instr)                   { return ( ( ((instr) >> 25) & 0x1) == 0x0 ) && ( ( ((instr) >> 20) & 0x17) == 0x7 ); } // 0b0 and 0b0x111
static inline int is_LDRBT_2(uint32_t instr)                 { return ( ( ((instr) >> 25) & 0x1) == 0x1 ) && ( ( ((instr) >> 20) & 0x17) == 0x7 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b0 and 0b0x111 and 0b0
//======================================

//>> layer 2
// instruction is media instructions
#define IS_MED(instr)               ( ( ( ((instr) >> 25) & 0x7) == 0x3 ) && ( ( ((instr) >> 4) & 0x1) == 0x1 ) ) // 0b011 and 0b1

//>> layer 3
// instruction is parallel addition and subtraction, signed
#define IS_PAS_S(instr)             ( ( ( ((instr) >> 20) & 0x1C) == 0x0 ) ) // 0b000xx

//>> layer 4
// instruction is normal signed
#define IS_PAS_S_NORM(instr)        ( ( ( ((instr) >> 20) & 0x3) == 0x1 ) ) // 0b01
//======================================
//>> layer 5
static inline int is_ADD16(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x7) == 0x0 ); } // 0b000
static inline int is_ASX(uint32_t instr)        { return ( ( ((instr) >> 5) & 0x7) == 0x1 ); } // 0b001
static inline int is_SAX(uint32_t instr)        { return ( ( ((instr) >> 5) & 0x7) == 0x2 ); } // 0b010
static inline int is_SUB16(uint32_t instr)      { return ( ( ((instr) >> 5) & 0x7) == 0x3 ); } // 0b011
static inline int is_ADD8(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x7) == 0x4 ); } // 0b100
static inline int is_SUB8(uint32_t instr)       { return ( ( ((instr) >> 5) & 0x7) == 0x7 ); } // 0b111
//======================================

//>> layer 4
// instruction is saturating signed
#define IS_PAS_S_SAT(instr)        ( ( ( ((instr) >> 20) & 0x3) == 0x2 ) ) // 0b10
//==============
//>> layer 5
// same layer 5 as previous
//==============

//>> layer 4
// instruction is halving signed
#define IS_PAS_S_HALV(instr)        ( ( ( ((instr) >> 20) & 0x3) == 0x3 ) ) // 0b11
//==============
//>> layer 5
// same layer 5 as previous
//==============

//>> layer 3
// instruction is parallel addition and subtraction, unsigned
#define IS_PAS_U(instr)             ( ( ( ((instr) >> 20) & 0x1C) == 0x4 ) ) // 0b001xx

//>> layer 4
// instruction is normal unsigned
#define IS_PAS_U_NORM(instr)        ( ( ( ((instr) >> 20) & 0x3) == 0x1 ) ) // 0b01
//==============
//>> layer 5
// same layer 5 as previous
//==============

//>> layer 4
// instruction is saturating unsigned
#define IS_PAS_U_SAT(instr)        ( ( ( ((instr) >> 20) & 0x3) == 0x2 ) ) // 0b10
//==============
//>> layer 5
// same layer 5 as previous
//==============

//>> layer 4
// instruction is halving unsigned
#define IS_PAS_U_HALV(instr)        ( ( ( ((instr) >> 20) & 0x3) == 0x3 ) ) // 0b11
//==============
//>> layer 5
// same layer 5 as previous
//==============

//>> layer 3
// instruction is packing, unpacking, saturation, and reversal
#define IS_PUSR(instr)             ( ( ( ((instr) >> 20) & 0x18) == 0x8 ) ) // 0b01xxx
//--- helper macros ---
// is extend and add byte base instruction (used several times in layer 4)
#define IS_XTAB(instr)                ( ( ( ((instr) >> 5) & 0x7) == 0x3 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ) ) // 0b011 and not 0b1111
// is extend byte base instruction (used several times in layer 4)
#define IS_XTB(instr)                ( ( ( ((instr) >> 5) & 0x7) == 0x3 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ) ) // 0b011 and 0b1111
//---------------------
//=========================
//>> layer 4
static inline int is_PKH(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7) == 0x0 ) && ( ( ((instr) >> 5) & 0x1) == 0x0 ); } // 0b000 and 0bxx0
static inline int is_SXTAB16(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x7) == 0x0 ) && IS_XTAB(instr); } // 0b000 and IS_XTAB()
static inline int is_SXTB16(uint32_t instr)         { return ( ( ((instr) >> 20) & 0x7) == 0x0 ) && IS_XTB(instr); } // 0b000 and IS_XTB()
static inline int is_SEL(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7) == 0x0 ) && ( ( ((instr) >> 5) & 0x7) == 0x5 ); } // 0b000 and 0b101

static inline int is_SSAT(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x6) == 0x2 ) && ( ( ((instr) >> 5) & 0x1) == 0x0 ); } // 0b01x and 0bxx0

static inline int is_SSAT16(uint32_t instr)         { return ( ( ((instr) >> 20) & 0x7) == 0x2 ) && ( ( ((instr) >> 5) & 0x7) == 0x1 ); } // 0b010 and 0b001
static inline int is_SXTAB(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7) == 0x2 ) && IS_XTAB(instr); } // 0b010 and IS_XTAB()
static inline int is_SXTB(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x7) == 0x2 ) && IS_XTB(instr); } // 0b010 and IS_XTB()

static inline int is_REV(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7) == 0x3 ) && ( ( ((instr) >> 5) & 0x7) == 0x1 ); } // 0b011 and 0b001
static inline int is_SXTAH(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7) == 0x3 ) && IS_XTAB(instr); } // 0b011 and IS_XTAB()
static inline int is_SXTH(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x7) == 0x3 ) && IS_XTB(instr); } // 0b011 and IS_XTB()
static inline int is_REV16(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7) == 0x3 ) && ( ( ((instr) >> 5) & 0x7) == 0x5 ); } // 0b011 and 0b101

static inline int is_UXTAB16(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x7) == 0x4 ) && IS_XTAB(instr); } // 0b011 and IS_XTAB()
static inline int is_UXTB16(uint32_t instr)         { return ( ( ((instr) >> 20) & 0x7) == 0x4 ) && IS_XTB(instr); } // 0b011 and IS_XTB()

static inline int is_USAT(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x6) == 0x6 ) && ( ( ((instr) >> 5) & 0x1) == 0x0 ); } // 0b01x and 0bxx0

static inline int is_USAT16(uint32_t instr)         { return ( ( ((instr) >> 20) & 0x7) == 0x6 ) && ( ( ((instr) >> 5) & 0x7) == 0x1 ); } // 0b011 and 0b001
static inline int is_UXTAB(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7) == 0x6 ) && IS_XTAB(instr); } // 0b011 and IS_XTAB()
static inline int is_UXTB(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x7) == 0x6 ) && IS_XTB(instr); } // 0b011 and IS_XTB()

static inline int is_RBIT(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x7) == 0x7 ) && ( ( ((instr) >> 5) & 0x7) == 0x1 ); } // 0b111 and 0b001
static inline int is_UXTAH(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7) == 0x7 ) && IS_XTAB(instr); } // 0b011 and IS_XTAB()
static inline int is_UXTH(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x7) == 0x7 ) && IS_XTB(instr); } // 0b011 and IS_XTB()
static inline int is_REVSH(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7) == 0x7 ) && ( ( ((instr) >> 5) & 0x7) == 0x5 ); } // 0b011 and 0b101
//=========================

//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Branch, branch with link, and block data transfer instructions
#define IS_BR_BLK(instr)            ( ( ((instr) >> 25) & 0x6) == 0x4 ) // 0b10x
//===========================
//>> layer 2
static inline int is_STMDA(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x3D) == 0x0 ); } // 0b0000x0
static inline int is_LDMDA(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x3D) == 0x1 ); } // 0b0000x1
static inline int is_STM(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x3D) == 0x8 ); } // 0b0010x0
static inline int is_LDM(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x3F) == 0x9 ) || ( ( ( ((instr) >> 20) & 0x3F) == 0xB ) && ( ( ((instr) >> 16) & 0xF) != 0xD ) ); } // 0b0010x0 or (0b001011 and not 0b1101)
static inline int is_POP(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x3F) == 0xB ) && ( ( ((instr) >> 16) & 0xF) == 0xD ); } // 0b001011 and 0b1101
static inline int is_STMDB(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x3F) == 0x10 ) || ( ( ( ((instr) >> 20) & 0x3F) == 0x12 ) && ( ( ((instr) >> 16) & 0xF) != 0xD ) ); } // 0b010000 or (0b010010 and not 0b1101)
static inline int is_PUSH(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x3F) == 0x12 ) && ( ( ((instr) >> 16) & 0xF) == 0xD ); } // 0b010010 and not 0b1101

static inline int is_LDMDB(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x3D) == 0x11 ); } // 0b0100x1
static inline int is_STMIB(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x3D) == 0x18 ); } // 0b0110x0
static inline int is_LDMIB(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x3D) == 0x19 ); } // 0b0110x0
static inline int is_STM_usr(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x25) == 0x4 ); } // 0b0xx1x0
static inline int is_LDM_usr(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x25) == 0x5 ) && ( ( ((instr) >> 15) & 0x1) == 0x0 ); } // 0b0xx1x1 and 0b0
static inline int is_LDM_exc(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x25) == 0x5 ) && ( ( ((instr) >> 15) & 0x1) == 0x1 ); } // 0b0xx1x1 and 0b0
static inline int is_B(uint32_t instr)              { return ( ( ((instr) >> 20) & 0x30) == 0x20 ); } // 0b10xxxx
static inline int is_BL(uint32_t instr)             { return ( ( ((instr) >> 20) & 0x30) == 0x30 ); } // 0b11xxxx
//===========================



//>>>>>>>>>>>>>
//>> layer 1 <<
//>>>>>>>>>>>>>
// Coprocessor instructions and Supervisor Call and Floating-point and Advanced SIMD data transfers
#define IS_CO_SPR(instr)            ( ( ((instr) >> 25) & 0x6) == 0x6 ) // 0b11x
//========================
//>> layer 2
static inline int is_UNDEF_4(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x3E) == 0x0 ); } // 0b00000x
static inline int is_SVC(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x30) == 0x30 ); } // 0b11xxxx
//========================

//>> layer 2
#define IS_COPROC(instr)            ( ( ((instr) >> 8) & 0xE) != 0xA ) // 0b101x
//=======================
//>> layer 3
static inline int is_STC(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x21) == 0x0 ) && ( ( ((instr) >> 20) & 0x3B) == 0x0 ); } // not 0b101x and (0bxxxx0 and not 000x00)
static inline int is_LDC_imm(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x21) == 0x1 ) && ( ( ((instr) >> 20) & 0x3B) == 0x1 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } // not 0b101x and 0bxxxx0 and not 000x00 and not 0b1111
static inline int is_LDC_lit(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x21) == 0x1 ) && ( ( ((instr) >> 20) & 0x3B) == 0x1 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } // not 0b101x and 0bxxxx0 and not 000x00 and 0b1111
static inline int is_MCRR(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x3F) == 0x4 ); } // 0b000100
static inline int is_MRRC(uint32_t instr)           { return ( ( ((instr) >> 20) & 0x3F) == 0x5 ); } // 0b000101
static inline int is_CDP(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x30) == 0x20 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b10xxxx and 0b0
static inline int is_MCR(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x31) == 0x20 ) && ( ( ((instr) >> 4) & 0x1) == 0x1 ); } // 0b10xxx0 and 0b1
static inline int is_MRC(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x31) == 0x21 ) && ( ( ((instr) >> 4) & 0x1) == 0x1 ); } // 0b10xxx1 and 0b1
//=======================

//>> layer 2
#define IS_SIMD(instr)            ( ( ((instr) >> 8) & 0xE) == 0xA ) // 0b101x
//=====================
// TODO: SIMD and Floating Point are not implemented
//=====================

//... Layer 0 ...
// unconditional instructions
#define IS_UNCOND(instr)       ( ( ((instr) >> 28) & 0xF) == 0xF ) // 0b1111

//>> layer 1
// memory hints, advanced SIMD instructions, and miscellaneous instructions
#define IS_MH_ASIMD_MISC(instr)       ( ( ((instr) >> 20) & 0x80) == 0x0 ) // 0b0xxxxxxx
//=======================
//>> layer 2
static inline int is_CPS(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7F) == 0x10 ) && ( ( ((instr) >> 4) & 0x2) == 0x0 ) && ( ( ((instr) >> 16) & 0x1) == 0x0 ); } // 0b0010000 and xx0x and xxx0
static inline int is_SETEND(uint32_t instr)         { return ( ( ((instr) >> 20) & 0x7F) == 0x10 ) && ( ( ((instr) >> 4) & 0xF) == 0x0 ) && ( ( ((instr) >> 16) & 0x1) == 0x1 ); } // 0b0010000 and 0000 and xxx0
// 0b01xxxxx TODO: advanced SIMD data-proccessing not implemented
// 0b100xxx0 TODO: advanced SIMD element or structure load/store not implemented
static inline int is_NOP_2(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x77) == 0x41 ); } // 0b100x001
static inline int is_PLI(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x77) == 0x45 ); } // 0b100x101
static inline int is_UNPRED_2(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x73) == 0x43 ); } // 0b100xx11
static inline int is_PLDW_imm(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x77) == 0x51 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } // 0b101x001 and not 0b1111
static inline int is_UNPRED_3(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x77) == 0x51 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } // 0b101x001 and 0b1111
static inline int is_PLD_imm(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x77) == 0x55 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } // 0b101x101 and not 0b1111
static inline int is_PLD_lit(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x77) == 0x55 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } // 0b101x101 and 0b1111
static inline int is_UNPRED_4(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x7F) == 0x53 ); } // 0b1010011
static inline int is_UNPRED_5(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x7F) == 0x57 ) && 
                                                        ( ( ( ((instr) >> 4) & 0xF) == 0x0 ) || ( ( ((instr) >> 4) & 0xE) == 0x2 ) || ( ( ((instr) >> 4) & 0xF) == 0x7 ) || ( ( ((instr) >> 4) & 0x8) == 0x8 ) ); } // 0b1010111 and (0b0000 or 001x or 0b0111 or 0b1xxx)
static inline int is_CLREX(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x7F) == 0x57 ) && ( ( ((instr) >> 4) & 0xF) == 0x1 ); } // 0b1010111 and 0b0001
static inline int is_DSB(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7F) == 0x57 ) && ( ( ((instr) >> 4) & 0xF) == 0x4 ); } // 0b1010111 and 0b0100
static inline int is_DMB(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7F) == 0x57 ) && ( ( ((instr) >> 4) & 0xF) == 0x5 ); } // 0b1010111 and 0b0101
static inline int is_ISB(uint32_t instr)            { return ( ( ((instr) >> 20) & 0x7F) == 0x57 ) && ( ( ((instr) >> 4) & 0xF) == 0x6 ); } // 0b1010111 and 0b0110
static inline int is_UNPRED_6(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x7B) == 0x5B ); } // 0b1011x11
static inline int is_NOP_3(uint32_t instr)          { return ( ( ((instr) >> 20) & 0x77) == 0x61 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b110x001 and 0bxxx0
static inline int is_PLI_reg(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x77) == 0x65 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b110x101 and 0bxxx0
static inline int is_PLDW_reg(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x77) == 0x71 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b111x001 and 0bxxx0
static inline int is_PLD_reg(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x77) == 0x75 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b111x101 and 0bxxx0
static inline int is_UNPRED_7(uint32_t instr)       { return ( ( ((instr) >> 20) & 0x63) == 0x63 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b11xxx11 and 0bxxx0
static inline int is_UNDEF_5(uint32_t instr)        { return ( ( ((instr) >> 20) & 0x7F) == 0x7F ) && ( ( ((instr) >> 4) & 0xF) == 0xF ); } // 0b1111111 and 0b1111
//=======================


//=======================
//>> layer 1
static inline int is_SRS(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xE5) == 0x84 ); } // 0b100xx1x0
static inline int is_RFE(uint32_t instr)            { return ( ( ((instr) >> 20) & 0xE5) == 0x81 ); } // 0b100xx0x1
static inline int is_BL_imm(uint32_t instr)         { return ( ( ((instr) >> 20) & 0xE0) == 0xA0 ); } // 0b101xxxxx
static inline int is_STC2(uint32_t instr)           { return ( ( ((instr) >> 20) & 0xE1) == 0xC0 ) && ( ( ((instr) >> 20) & 0xFB) != 0xC0 ); } // 0b110xxxx0 and not 11000x00
static inline int is_LDC2_imm(uint32_t instr)       { return ( ( ((instr) >> 20) & 0xE1) == 0xC1 ) && ( ( ((instr) >> 20) & 0xFB) != 0xC1 ) && ( ( ((instr) >> 16) & 0xF) != 0xF ); } // 0b110xxxx1 and not 11000x01 and not 0b1111
static inline int is_LDC2_lit(uint32_t instr)       { return ( ( ((instr) >> 20) & 0xE1) == 0xC1 ) && ( ( ((instr) >> 20) & 0xFB) != 0xC1 ) && ( ( ((instr) >> 16) & 0xF) == 0xF ); } // 0b110xxxx1 and not 11000x01 and not 0b1111
static inline int is_MCRR2(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xFF) == 0xC4 ); } // 0b11000100
static inline int is_MRRC2(uint32_t instr)          { return ( ( ((instr) >> 20) & 0xFF) == 0xC5 ); } // 0b11000101
static inline int is_CDP2(uint32_t instr)           { return ( ( ((instr) >> 20) & 0xF0) == 0xE0 ) && ( ( ((instr) >> 4) & 0x1) == 0x0 ); } // 0b1110xxxx and 0b0
static inline int is_MCR2(uint32_t instr)           { return ( ( ((instr) >> 20) & 0xF1) == 0xE0 ) && ( ( ((instr) >> 4) & 0x1) == 0x1 ); } // 0b1110xxx0 and 0b1
static inline int is_MRC2(uint32_t instr)           { return ( ( ((instr) >> 20) & 0xF1) == 0xE1 ) && ( ( ((instr) >> 4) & 0x1) == 0x1 ); } // 0b1110xxx1 and 0b1
//=======================

// start indices in the proc_instr_table 
#define DP_REG_START 0
#define DP_REG_END 21
#define DP_RSR_START 21
#define DP_RSR_END 40
#define MISC_START 40

// lookup table for processing instructions
extern int (*proc_instr_table[][2])(uint32_t);





#endif