
#include "decode.h"
#include "bit_matching.h"


const char *core_reg[16] = {
    "R0", "R1", "R2", "R3",
    "R4", "R5", "R6", "R7",
    "R8", "R9", "SL", "FP",
    "IP", "SP", "LR", "PC"  // R12 can be called IP or R12
};



const char *shift_codes[5] = {
    "LSL",
    "LSR",
    "ASR",
    "ROR",
    "RRX"
};





const char *cond_codes[16] = {
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


// lookup table for processing instructions
int (*proc_instr_table[][2])(uint32_t) = {
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
    { is_RRX    , RRX_instr },
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
    // miscellaneous

};