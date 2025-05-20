#ifndef A32_INFO_H
#define A


// A32 instructions are 4 bytes wide
#define A32_INSTR_SIZE 4

// encoding

//=== cond field (bits 28 - 31)

// |31 30 29 28|27 26 25|24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5| 4 |3 2 1 0|
// |  cond     |  op1   |                                                      |op |       |

#define UNCOND  // 0b111

//=== op1 field (bits 25 - 27)

// Data-processing (register)
#define DATA_PROC_REG // 0b000
// Data-processing (immediate)
#define DATA_PROC_IMM // 0b001

//#define // 0b010
//#define // 0b011
//#define // 0b10
//#define // 0b11

//=== 

#endif