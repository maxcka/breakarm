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
    TYPE_0,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    TYPE_0_RSR, // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>

    TYPE_1,     // syntax: <MNEMONIC><c> <Rn>, <Rm>{, <shift>}
    TYPE_1_RSR, // syntax: <MNEMONIC><c> <Rn>, <Rm>, <type> <Rs>

    TYPE_2,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>

    TYPE_3,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, #<imm5>
    TYPE_3_RSR, // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>

    TYPE_4,      // syntax: <MNEMONIC>{S}{<c>} <Rd>, <Rm> {, <shift>}
    TYPE_4_RSR,  // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, <type> <Rs>
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
    Register Rs; //^ this overlaps with imm5 (make union??) (used in RSR instructions)
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

//============================================
// MAYBE BETTER IDEA IS TO USE INLINE FUNCTIONS INSTEAD OF MACROS. THEN HAVE A LOOKUP TABLE MATCHING EACH FUNCTION TO THE PROCESSING FUNCTION
//=======================


//===================================================
//===================================================



// need comments for fn declarations

// auxiliary functions
Shift decode_imm_shift(ShiftType type, uint8_t imm5);
void get_shift_str(Shift shift, char *shift_str, int buf_sz);
void print_asm_instr(Instr *instr_s);

// instruction processing functions
//> data-processing (register)
int process_data_proc_instr(uint32_t instr, Instr *instr_s);
int AND_instr(uint32_t instr);
int EOR_instr(uint32_t instr);
int SUB_instr(uint32_t instr);
int RSB_instr(uint32_t instr);
int ADD_instr(uint32_t instr);
int ADC_instr(uint32_t instr);
int SBC_instr(uint32_t instr);
int RSC_instr(uint32_t instr);
int TST_instr(uint32_t instr);
int TEQ_instr(uint32_t instr);
int CMP_instr(uint32_t instr);
int CMN_instr(uint32_t instr);
int ORR_instr(uint32_t instr);
int MOV_reg_instr(uint32_t instr);
int LSL_instr(uint32_t instr);
int LSR_instr(uint32_t instr);
int ASR_instr(uint32_t instr);
int RRX_instr(uint32_t instr);
int ROR_instr(uint32_t instr);
int BIC_instr(uint32_t instr);
int MVN_instr(uint32_t instr);

// main functions
void find_and_decode(uint32_t instr, int start_idx, int end_idx);

void decode_instr(uint32_t instr);




#endif