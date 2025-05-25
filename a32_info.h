
// will create a struct that holds the instruction mask, string name, function handler, instruction word
// this could get tricky because of differing numbers of operands and how they are used


// what is the plan?:
//  decode top layers until we get to instruction
//      - example: 
//          check IS_DATA_PROC_0(instr) then
//          check IS_DP_REG(instr) then
//          keep doing that
//
//  I will decode like I'm reading it. I can optimize later once it is working

//**** this is getting really messy. start thinking of this as
// depth first search. need to reorganize the macros

// * there are many layers and it is uneven. there is almost no pattern

#ifndef A32_INFO_H
#define A32_INFO_H

#include <stdint.h>





/*

// Data-processing (immediate)
#define IS_DP_OP_1(instr)       ( ( ((instr) >> 25) & 0x7) == 0x1 ) // 0b001
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

//--- for DATA_PROC_0 ---
// Note: 0b11001 == 0x19
// Data-processing (register or register-shifted register)
#define IS_DP_REG(instr)            ( ( ((instr) >> 20) & 0x19) != 0xF ) // not 0b10xx0
#define IS_MISC_HALF_MULT(instr) // 0b10xx0
#define IS_MULT_ACC(instr) // 0xxxx
#define IS_SYNC_PRIM(instr) // 1xxxx
#define EX_LD_STR(instr) // not 0xx1x

//--- for DATA_PROC_1 ---



//-> Data-processing (Register)
#define IS_DPR_AND(instr)           ( ( ((instr) >> 20) & 0x1E) == 0)

*/

#endif