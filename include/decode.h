#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>


//===================================================

#define BUF_20 20

// A32 instructions are 4 bytes wide
#define A32_INSTR_SIZE 4


// start and end indices of different instruction categories in the proc_instr_table 
#define DP_REG_START 0
#define DP_REG_END 21
#define DP_RSR_START 21
#define DP_RSR_END 40
#define MISC_START 40




extern const char *core_reg[16];
extern const char *shift_codes[5];
extern const char *cond_codes[16];

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

// this might get really large
typedef enum {
    // data proc instructions
    TYPE_0,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    TYPE_0_RSR, // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>

    TYPE_1,     // syntax: <MNEMONIC><c> <Rn>, <Rm>{, <shift>}
    TYPE_1_RSR, // syntax: <MNEMONIC><c> <Rn>, <Rm>, <type> <Rs>

    TYPE_2,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>

    TYPE_3,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, #<imm5>
    TYPE_3_RSR, // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>

    TYPE_4,      // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm> {, <shift>}
    TYPE_4_RSR,  // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, <type> <Rs>
} Instr_type;

// TODO not used in code yet.
typedef enum {
    DATA_PROC
} Group;

// have a lookup table for group to print function like print_table[group] = print_fn

typedef struct {
    Group group; // TODO not used yet
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


// lookup table for processing instructions
extern int (*proc_instr_table[][2])(uint32_t);
// lookup table for printing an instruction by calling a function based on the instruction group that it is in
extern void (*print_instr_table[])(Instr *);


// need comments for fn declarations

// auxiliary functions
Shift decode_imm_shift(ShiftType type, uint8_t imm5);
void get_shift_str(Shift shift, char *shift_str, int buf_sz);
void print_asm_instr(Instr *instr_s);

// instruction processing functions
//> data-processing (register)
void print_data_proc_instr(Instr *instr_s);
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

void decode_dp_op_0(uint32_t instr);
void decode_dp_op_1(uint32_t instr);
void decode_ld_str_med(uint32_t instr);
void decode_br_blk(uint32_t instr);
void decode_co_spr(uint32_t instr);

void decode_instr(uint32_t instr);




#endif