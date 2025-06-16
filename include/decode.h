#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>


//===================================================

#define BUF_10 10
#define BUF_20 20

// A32 instructions are 4 bytes wide
#define A32_INSTR_SIZE 4

#define IH_ARR_SIZE 2   // instruction handler array size
#define BANKED_REG_TABLE_ROWS 8
#define BANKED_REG_TABLE_COLS 4
#define NUM_REG 16
#define NUM_SPEC_REG 3
#define NUM_SHIFT_TYPES 5

#define APSR_POS 2 // application level special register position

#define UNPRED_STR "UNPRED"
#define UNDEF_STR "UNDEF"

extern const char *core_reg[NUM_REG];
extern const char *spec_reg[NUM_SPEC_REG];
extern const char *shift_codes[NUM_SHIFT_TYPES];
extern const char *cond_codes[NUM_REG];

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


//==> instructions groupings:
// 1. instruction class -- e.g. DATA_PROC
// 1. instruction group -- e.g. DATA_PROC_REG
// 2. instruction type -- e.g. TYPE_0

// this might get really large
typedef enum {
    // data proc instructions
    TYPE_DP_0,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    TYPE_DP_0_RSR, // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>

    TYPE_DP_1,     // syntax: <MNEMONIC><c> <Rn>, <Rm>{, <shift>}
    TYPE_DP_1_RSR, // syntax: <MNEMONIC><c> <Rn>, <Rm>, <type> <Rs>

    TYPE_DP_2,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>

    TYPE_DP_3,     // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, #<imm5>
    TYPE_DP_3_RSR, // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>

    TYPE_DP_4,      // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm> {, <shift>}
    TYPE_DP_4_RSR,  // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, <type> <Rs>

    // misc instructions
    TYPE_MISC_BANKED_0,    // syntax: <MNEMONIC><c> <Rd>, <banked_reg>
    TYPE_MISC_BANKED_1,    // syntax: <MNEMONIC><c> <banked_reg>, <Rn>

    TYPE_MISC_1,            // syntax: <MNEMONIC><c> <Rd>, <spec_reg>
    
    TYPE_MISC_2_APP,    // syntax: <MNEMONIC><c> <spec_reg>, <Rn>
    TYPE_MISC_2_SYS,    // syntax: <MNEMONIC><c> <spec_reg>, <Rn>

    TYPE_MISC_3,    // syntax: <MNEMONIC><c> <Rm>
    TYPE_MISC_3_1,  // syntax: <MNEMONIC><c> <Rm> (can be set to unpred if Rm == PC)

    TYPE_MISC_4,    // syntax: <MNEMONIC><c> <Rm>

    TYPE_MISC_5,    // syntax: <MNEMONIC><c> <Rd>, <Rm>, <Rn>

    TYPE_MISC_6,    // syntax: <MNEMONIC><c>

    TYPE_MISC_7,    // syntax: <MNEMONIC> #<imm16>

    TYPE_MISC_8,    // syntax: <MNEMONIC><c> #<imm4>

    TYPE_UNPRED,
    TYPE_UNDEF
} IType;

// TODO not used in code yet.
typedef enum {
    GROUP_DP_REG,
    GROUP_DP_RSR,
    GROUP_MISC
} IGroup;

// have a lookup table for group to print function like print_table[group] = print_fn

typedef struct {
    IGroup igroup; // TODO not used yet
    IType itype; // instruction type

    uint8_t special;
    const char *mnemonic;
    Shift shift;
    union { // extra string buffer
        char shift_str[BUF_20];
        char banked_reg_str[BUF_20];
        char spec_reg_str[BUF_20];
        char imm_str[BUF_20];
    };
    Cond c;
    Register Rd;
    Register Rn;
    Register Rm;
    Register Rs; //^ this overlaps with imm5 (make union??) (used in RSR instructions)
    uint8_t S;
    uint8_t R;
} Instr;



typedef int (*InstrHandler)(uint32_t);

typedef struct {
    InstrHandler (*table)[IH_ARR_SIZE];
    int num_rows;
} InstrHandlerTable;


// === elements of the proc_instr_group_table ===
extern InstrHandler proc_dp_reg_table[][IH_ARR_SIZE];
extern InstrHandler proc_dp_rsr_table[][IH_ARR_SIZE];
extern InstrHandler proc_misc_table[][IH_ARR_SIZE];
// ==============================================
// lookup table for processing instructions
//extern int (*proc_instr_table[][2])(uint32_t);
extern InstrHandlerTable proc_instr_group_table[];
// lookup table for printing an instruction by calling a function based on the instruction group that it is in
extern void (*print_instr_table[])(Instr *);



extern const char *banked_reg_table[][BANKED_REG_TABLE_ROWS][BANKED_REG_TABLE_COLS];


// need comments for fn declarations


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

//> miscellaneous
void print_misc_instr(Instr *instr_s);
int process_misc_instr(uint32_t instr, Instr *instr_s);
int MRS_BANKED_instr(uint32_t instr);
int MSR_BANKED_instr(uint32_t instr);
int MRS_instr(uint32_t instr);
int MSR_reg_app_instr(uint32_t instr);
int MSR_reg_sys_instr(uint32_t instr);
int BX_instr(uint32_t instr);
int CLZ_instr(uint32_t instr);
int BXJ_instr(uint32_t instr);
int BLX_reg_instr(uint32_t instr);
int QADD_instr(uint32_t instr);
int QSUB_instr(uint32_t instr);
int QDADD_instr(uint32_t instr);
int QDSUB_instr(uint32_t instr);
int ERET_instr(uint32_t instr);
int BKPT_instr(uint32_t instr);
int HVC_instr(uint32_t instr);
int SMC_instr(uint32_t instr);


// auxiliary functions
void get_imm_str(Instr *instr_s, uint8_t imm4, uint16_t imm12);
void get_sys_sr_str(Instr *instr_s, uint8_t mask);
void get_app_sr_str(Instr *instr_s, uint8_t mask);
void get_banked_reg_str(uint8_t m, uint8_t m1, uint8_t R, char *banked_reg_str, int buf_sz);
Shift decode_imm_shift(ShiftType type, uint8_t imm5);
void get_shift_str(Shift shift, char *shift_str, int buf_sz);
void print_asm_instr(Instr *instr_s);


// main functions
void find_and_decode(uint32_t instr, IGroup igroup);

void decode_dp_op_0(uint32_t instr);
void decode_dp_op_1(uint32_t instr);
void decode_ld_str_med(uint32_t instr);
void decode_br_blk(uint32_t instr);
void decode_co_spr(uint32_t instr);

void decode_instr(uint32_t instr);




#endif