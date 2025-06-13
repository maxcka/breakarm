
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


void (*print_instr_table[])(Instr *) = {
    print_data_proc_instr,      // DATA_PROC_REG
    print_data_proc_instr      // DATA_PROC_RSR
    
};


//int (*data_proc_reg_table[][2])(uint32_t) = {
InstrHandler proc_dp_reg_table[][2] = {
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
};

InstrHandler proc_dp_rsr_table[][2] = {
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
    { is_MVN    , MVN_instr }
};

InstrHandler proc_misc_table[][2] = {
    // miscellaneous
    //{ is_MRS_BANKED, }, 
    //{ is_MSR_BANKED, }, 
    //{ is_MRS, }, 
    //{ is_MSR_REG_APP, }, 
    //{ is_MSR_REG_SYS, }, 
    //{ is_BX, }, 
    //{ is_CLZ, }, 
    //{ is_BXJ, }, 
    //{ is_BLX_REG, }, 
    //{ is_QADD, }, 
    //{ is_QSUB, }, 
    //{ is_QDADD, }, 
    //{ is_QDSUB, }, 
    //{ is_ERET, }, 
    //{ is_BKPT, }, 
    //{ is_HVC, }, 
    //{ is_SMC, }
};

// lookup table for processing instructions
// format: { bit-matching fn, processing function }
//int (*proc_instr_table[][2])(uint32_t) = {

// an array where each element is an InstrHandlerTable struct
InstrHandlerTable proc_instr_group_table[] = {  
    { proc_dp_reg_table, sizeof(proc_dp_reg_table) / sizeof(proc_dp_reg_table[0]) },
    { proc_dp_rsr_table, sizeof(proc_dp_rsr_table) / sizeof(proc_dp_rsr_table[0]) }
};