#ifndef A32_INFO_H
#define A


// A32 instructions are 4 bytes wide
#define A32_INSTR_SIZE 4

// decoding
// 1. check cond
// 2. check op1
// 3. check op

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
#define IS_DATA_PROC_0(instr)       ( ( ((instr) >> 25) & 0x7) == 0x0 ) // 0b000
// Data-processing (immediate)
#define IS_DATA_PROC_1(instr)       ( ( ((instr) >> 25) & 0x7) == 0x1 ) // 0b001
// load/store word and unsigned byte (usually immediate)
#define IS_LD_STR_WRD(instr)        ( ( ((instr) >> 25) & 0x7) == 0x2 ) // 0b010
// load/store word and unsigned byte (usually register) or media instr
#define IS_LD_STR_WRD_MEDIA(instr)  ( ( ((instr) >> 25) & 0x7) == 0x3 ) // 0b011
//--> mask bits 27-26
// branch, branch with link, and block data transfer
#define IS_BR_BLK(instr)            ( ( ((instr) >> 26) & 0x3) == 0x2 ) // 0b10
// Coprocessor instr, and supervisor call
#define IS_CPROC_SPR(instr)         ( ( ((instr) >> 26) & 0x3) == 0x3 ) // 0b11

//=== 

//==============================
//=== DATA PROC Instructions ===
//==============================

//=== op1 field (bits 24 - 20)

//*** for DATA_PROC_0 ***
// Note: 0b11001 == 0x19
// Data-processing (register or register-shifted register)
#define IS_DP_REG(instr)            ( ( ((instr) >> 20) & 0x19) != 0xF ) // not 0b10xx0
#define IS_MISC_HALF_MULT(instr) // 0b10xx0
#define IS_MULT_ACC(instr) // 0xxxx
#define IS_SYNC_PRIM(instr) // 1xxxx
#define EX_LD_STR(instr) // not 0xx1x

//*** for DATA_PROC_1 ***

#endif