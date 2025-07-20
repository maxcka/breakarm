/*
 * breakarm – A32 Instruction Disassembler
 * Copyright (c) 2025 Maximilian Kallas
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */


#include "decode.h"
#include "bit_matching.h"


const char *core_reg[NUM_REG] = {
    "R0", "R1", "R2", "R3",
    "R4", "R5", "R6", "R7",
    "R8", "R9", "SL", "FP",
    "IP", "SP", "LR", "PC"  // R12 can be called IP or R12
};

const char *spec_reg[NUM_SPEC_REG] = {
    "CPSR",
    "SPSR",
    "APSR"
};



const char *shift_codes[NUM_SHIFT_TYPES] = {
    "LSL",
    "LSR",
    "ASR",
    "ROR",
    "RRX"
};



const char *cond_codes[NUM_REG] = {
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
    "",  // AL is not printed bc it is the default
    "",  // UNCOND not printed
};

const char *amode_table[AMODE_SIZE][AMODE_SIZE] = {
    { "DA", "IA" },
    { "DB", "IB" }
};

// used in DSB, DMB, ISB
const char *option_table[OPTION_SIZE] = {
    "", // 0b0000
    "", // 0b0001
    "OSHST", // 0b0010
    "OSH", // 0b0011
    "", // 0b0100
    "", // 0b0101
    "NSHST", // 0b0110
    "NSH", // 0b0111
    "", // 0b1000
    "", // 0b1001
    "ISHST", // 0b1010
    "ISH", // 0b1011
    "", // 0b1100
    "", // 0b1101
    "ST", // 0b1110
    "SY" // 0b1111
};


// [R][SYSm<2:0>][SYSm<4:3>]
const char *banked_reg_table[][BANKED_REG_TABLE_ROWS][BANKED_REG_TABLE_COLS] = {
    {
        { "R8_usr",     "R8_fiq",   "LR_irq", "UNPRED" },
        { "R9_usr",     "R9_fiq",   "SP_irq", "UNPRED" },
        { "R10_usr",    "R10_fiq",  "LR_svc", "UNPRED" },
        { "R11_usr",    "R11_fiq",  "SP_svc", "UNPRED" },
        { "R12_usr",    "R12_fiq",  "LR_abt", "LR_mon" },
        { "SP_usr",     "SP_fiq",   "SP_abt", "SP_mon" },
        { "LR_usr",     "LR_fiq",   "LR_und", "ELR_hyp" },
        { "UNPRED",     "UNPRED",   "SP_und", "SP_hyp" }
    },
    {
        { "UNPRED", "UNPRED",   "SPSR_irq", "UNPRED" },
        { "UNPRED", "UNPRED",   "UNPRED",   "UNPRED" },
        { "UNPRED", "UNPRED",   "SPSR_svc", "UNPRED" },
        { "UNPRED", "UNPRED",   "UNPRED",   "UNPRED" },
        { "UNPRED", "UNPRED",   "SPSR_abt", "SPSR_mon" },
        { "UNPRED", "UNPRED",   "UNPRED",   "UNPRED" },
        { "UNPRED", "SPSR_fiq", "SPSR_und", "SPSR_hyp" },
        { "UNPRED", "UNPRED",   "UNPRED",   "UNPRED" }
    }
};



//int (*data_proc_reg_table[][2])(uint32_t) = {
InstrHandler proc_dp_reg_table[][IH_ARR_SIZE] = {
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
    { is_MOV_reg, MOV_instr },
    { is_LSL_imm, LSL_instr },
    { is_LSR_imm, LSR_instr },
    { is_ASR_imm, ASR_instr },
    { is_RRX    , RRX_instr },
    { is_ROR_imm, ROR_instr },
    { is_BIC    , BIC_instr },
    { is_MVN    , MVN_instr },
};

InstrHandler proc_dp_rsr_table[][IH_ARR_SIZE] = {
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

InstrHandler proc_misc_table[][IH_ARR_SIZE] = {
    // miscellaneous
    { is_MRS_BANKED , MRS_BANKED_instr },
    { is_MSR_BANKED , MSR_BANKED_instr },
    { is_MRS        , MRS_instr }, 
    { is_MSR_reg_app, MSR_reg_app_instr }, 
    { is_MSR_reg_sys, MSR_reg_sys_instr }, 
    { is_BX         , BX_instr }, 
    { is_CLZ        , CLZ_instr },
    { is_BXJ        , BXJ_instr }, 
    { is_BLX_reg    , BLX_reg_instr }, 
    { is_QADD       , QADD_instr }, 
    { is_QSUB       , QSUB_instr }, 
    { is_QDADD      , QDADD_instr }, 
    { is_QDSUB      , QDSUB_instr }, 
    { is_ERET       , ERET_instr }, 
    { is_BKPT       , BKPT_instr }, 
    { is_HVC        , HVC_instr }, 
    { is_SMC        , SMC_instr }
};

InstrHandler proc_hm_table[][IH_ARR_SIZE] = {
    // halfword multiply and multiply accumulate
    { is_SMLA   , SMLA_instr },
    { is_SMLAW  , SMLAW_instr },
    { is_SMULW  , SMULW_instr },
    { is_SMLALXY, SMLALXY_instr },
    { is_SMUL   , SMUL_instr }
};

InstrHandler proc_mult_table[][IH_ARR_SIZE] = {
    // multiply and multiply accumulate
    { is_MUL    , MUL_instr },
    { is_MLA    , MLA_instr },
    { is_UMAAL  , UMAAL_instr },
    { is_UNDEF  , UNDEF_instr },
    { is_MLS    , MLS_instr },
    { is_UNDEF_2, UNDEF_instr },
    { is_UMULL  , UMULL_instr },
    { is_UMLAL  , UMLAL_instr },
    { is_SMULL  , SMULL_instr },
    { is_SMLAL  , SMLAL_instr }
};

InstrHandler proc_sync_table[][IH_ARR_SIZE] = {
    // synchronization
    { is_SWP    , SWP_instr },
    { is_STREX  , STREX_instr },
    { is_LDREX  , LDREX_instr },
    { is_STREXD , STREXD_instr },
    { is_LDREXD , LDREXD_instr },
    { is_STREXB , STREXB_instr },
    { is_LDREXB , LDREXB_instr },
    { is_STREXH , STREXH_instr },
    { is_LDREXH , LDREXH_instr }
};

InstrHandler proc_ex_ld_str_table[][IH_ARR_SIZE] = {
    // extra load/store
    { is_STRH_reg   , STRH_reg_instr },
    { is_LDRH_reg   , LDRH_reg_instr },
    { is_STRH_imm   , STRH_imm_instr },
    { is_LDRH_imm   , LDRH_imm_instr },
    { is_LDRD_reg   , LDRD_reg_instr },
    { is_LDRSB_reg  , LDRSB_reg_instr },
    { is_LDRD_imm   , LDRD_imm_instr },
    { is_LDRSB_imm  , LDRSB_imm_instr },
    { is_STRD_reg   , STRD_reg_instr },
    { is_LDRSH_reg  , LDRSH_reg_instr },
    { is_STRD_imm   , STRD_imm_instr },
    { is_LDRSH_imm  , LDRSH_imm_instr }
};

InstrHandler proc_ex_ld_str_unp_table[][IH_ARR_SIZE] = {
    // extra load/store unprivileged
    { is_STRHT  , STRHT_instr },
    { is_LDRHT  , LDRHT_instr },
    { is_UNPRED , UNPRED_instr },
    { is_UNDEF_3, UNDEF_instr },
    { is_LDRSBT , LDRSBT_instr },
    { is_LDRSHT , LDRSHT_instr }
};

InstrHandler proc_dp_imm_table[][IH_ARR_SIZE] = {
    // data-processing imm
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
    { is_MOV_imm, MOV_instr },
    { is_BIC    , BIC_instr },
    { is_MVN    , MVN_instr },
};

InstrHandler proc_dp_imm16_table[][IH_ARR_SIZE] = {
    // data-processing imm16
    { is_MOVW   , MOVW_instr },
    { is_MOVT   , MOVT_instr }
};

InstrHandler proc_misc_hints_table[][IH_ARR_SIZE] = {
    // msr and hints
    { is_NOP,           NOP_instr},
    { is_YIELD,         YIELD_instr},
    { is_WFE,           WFE_instr},
    { is_WFI,           WFI_instr},
    { is_SEV,           SEV_instr},
    { is_DBG,           DBG_instr},
    { is_MSR_imm_app, MSR_imm_app_instr},
    { is_MSR_imm_sys, MSR_imm_sys_instr}
};

InstrHandler proc_ld_str_table[][IH_ARR_SIZE] = {
    // load store
    { is_STR_imm,   STR_instr},
    { is_STR_reg,   STR_instr},
    { is_STRT,      STRT_instr},
    { is_STRT_2,    STRT_instr},
    { is_LDR_imm,   LDR_instr},
    { is_LDR_reg,   LDR_instr},
    { is_LDRT,      LDRT_instr},
    { is_LDRT_2,    LDRT_instr},
    { is_STRB_imm,  STRB_instr},
    { is_STRB_reg,  STRB_instr},
    { is_STRBT,     STRBT_instr},
    { is_STRBT_2,   STRBT_instr},
    { is_LDRB_imm,  LDRB_instr},
    { is_LDRB_reg,  LDRB_instr},
    { is_LDRBT,     LDRBT_instr},
    { is_LDRBT_2,   LDRBT_instr}
};

InstrHandler proc_pas_table[][IH_ARR_SIZE] = {
    // parallel add and sub
    { is_ADD16, ADD16_instr},
    { is_ASX,   ASX_instr},
    { is_SAX,   SAX_instr},
    { is_SUB16, SUB16_instr},
    { is_ADD8,  ADD8_instr},
    { is_SUB8,  SUB8_instr}
};

InstrHandler proc_pusr_table[][IH_ARR_SIZE] = {
    // packing, unpacking, saturation, and reversal
    { is_PKH, PKH_instr},
    { is_SXTAB16, XTAB_instr},
    { is_UXTAB16, XTAB_instr},
    { is_SXTAB, XTAB_instr},
    { is_UXTAB, XTAB_instr},
    { is_SXTB16, XTB_instr},
    { is_UXTB16, XTB_instr},
    { is_SXTB, XTB_instr},
    { is_UXTB, XTB_instr},
    { is_SSAT, SAT_instr},
    { is_SSAT16, SAT_instr},
    { is_USAT, SAT_instr},
    { is_USAT16, SAT_instr},
    { is_SEL, SEL_instr},
    { is_REV, REV_instr},
    { is_REV16, REV_instr},
    { is_SXTAH, XTAH_instr},
    { is_UXTAH, XTAH_instr},
    { is_SXTH, XTH_instr},
    { is_UXTH, XTH_instr},
    { is_RBIT, RBIT_instr},
    { is_REVSH, REVSH_instr}
};

InstrHandler proc_signed_mult_table[][IH_ARR_SIZE] = {
    // signed multiply, signed and unsigned divide
    { is_SMLAD, SMLXD_instr },
    { is_SMLSD, SMLXD_instr },
    { is_SMUAD, SMUXD_instr },
    { is_SMUSD, SMUXD_instr },
    { is_SDIV, XDIV_instr },
    { is_UDIV, XDIV_instr },
    { is_SMLAD, SMLXD_instr },
    { is_SMLSD, SMLXD_instr },
    { is_SMLALD, SMLXLD_instr },
    { is_SMLSLD, SMLXLD_instr },
    { is_SMMLA, SMMLX_instr },
    { is_SMMLS, SMMLX_instr },
    { is_SMMUL, SMMUL_instr }
};

InstrHandler proc_other_media_table[][IH_ARR_SIZE] = {
    // other media instructions
    { is_USAD8, USAD8_instr},
    { is_USADA8, USADA8_instr},
    { is_SBFX, SBFX_instr},
    { is_BFC, BFC_instr},
    { is_BFI, BFI_instr},
    { is_UBFX, UBFX_instr},
    { is_UDF, UDF_instr}
};

InstrHandler proc_branch_block_table[][IH_ARR_SIZE] = {
    // branch, branch and link, and block data transfer
    { is_STMDA, STMDX_instr },
    { is_LDMDA, LDMDX_instr },
    { is_STMIA, STMXX_instr },
    { is_LDMIA, LDMXX_instr },
    { is_POP, POP_instr },
    { is_STMDB, STMDX_instr },
    { is_PUSH, PUSH_instr },
    { is_LDMDB, LDMDX_instr },
    { is_STMIB, STMXX_instr },
    { is_LDMIB, LDMXX_instr },
    { is_STM_usr, STM_instr },
    { is_LDM_usr, LDM_instr },
    { is_LDM_exc, LDM_instr },
    { is_B, B_instr },
    { is_BL, BL_imm_instr }
};


InstrHandler proc_coproc_table[][IH_ARR_SIZE] = {
    // coprocessor and supervisor call instructions
    { is_UNDEF_4, UNDEF_instr},
    { is_SVC, SVC_instr},
    { is_STC, STC_instr},
    { is_LDC, LDC_instr},
    { is_MCRR, MCRR_instr},
    { is_MRRC, MRRC_instr},
    { is_CDP, CDP_instr},
    { is_MCR, MCR_instr},
    { is_MRC, MRC_instr}
};

InstrHandler proc_uncond_misc_table[][IH_ARR_SIZE] = {
    // unconditional instructions (misc)
    { is_CPS, CPS_instr},
    { is_SETEND, SETEND_instr},
    { is_SIMD_DP, NOT_IMP_instr},
    { is_SIMD_LD_STR, NOT_IMP_instr},   
    { is_NOP_2, NOP_instr},
    { is_PLI_imm, PLI_imm_instr},
    { is_UNPRED_2, UNPRED_instr},   
    { is_PLDW_imm, PLD_imm_instr},
    { is_UNPRED_3, UNPRED_instr},
    { is_PLD_imm, PLD_imm_instr},
    { is_UNPRED_4, UNPRED_instr},
    { is_UNPRED_5, UNPRED_instr},
    { is_CLREX, CLREX_instr},   
    { is_DSB, DSB_instr},   
    { is_DMB, DMB_instr},  
    { is_ISB, ISB_instr},
    { is_UNPRED_6, UNPRED_instr},
    { is_NOP_3, NOP_instr}, 
    { is_PLI_reg, PLI_reg_instr},   
    { is_PLD_reg, PLD_reg_instr},   
    { is_UNPRED_7, UNPRED_instr},
    { is_UNDEF_5, UNDEF_instr}
};

InstrHandler proc_uncond_table[][IH_ARR_SIZE] = {
    { is_SRS,       SRS_instr},   
    { is_RFE,       RFE_instr},
    { is_BLX_imm,   BL_imm_instr },
    { is_STC2,      STC_instr },
    { is_LDC2_imm,  LDC_instr },
    { is_MCRR2,     MCRR_instr },
    { is_MRRC2,     MRRC_instr },
    { is_CDP2,      CDP_instr },
    { is_MCR2,      MCR_instr },
    { is_MRC2,      MRC_instr }
};

 

// lookup table for processing instructions
// format: { bit-matching fn, processing function }
//int (*proc_instr_table[][2])(uint32_t) = {

// an array where each element is an InstrHandlerTable struct
// index has to match with group enum
InstrHandlerTable proc_instr_group_table[] = {  
    { proc_dp_reg_table, sizeof(proc_dp_reg_table) / sizeof(proc_dp_reg_table[0]) },
    { proc_dp_rsr_table, sizeof(proc_dp_rsr_table) / sizeof(proc_dp_rsr_table[0]) },
    { proc_misc_table, sizeof(proc_misc_table) / sizeof(proc_misc_table[0]) },
    { proc_hm_table, sizeof(proc_hm_table) / sizeof(proc_hm_table[0]) },
    { proc_mult_table, sizeof(proc_mult_table) / sizeof(proc_mult_table[0]) },
    { proc_sync_table, sizeof(proc_sync_table) / sizeof(proc_sync_table[0]) },
    { proc_ex_ld_str_table, sizeof(proc_ex_ld_str_table) / sizeof(proc_ex_ld_str_table[0]) },
    { proc_ex_ld_str_unp_table, sizeof(proc_ex_ld_str_unp_table) / sizeof(proc_ex_ld_str_unp_table[0]) },
    { proc_dp_imm_table, sizeof(proc_dp_imm_table) / sizeof(proc_dp_imm_table[0]) },
    { proc_dp_imm16_table, sizeof(proc_dp_imm16_table) / sizeof(proc_dp_imm16_table[0]) },
    { proc_misc_hints_table, sizeof(proc_misc_hints_table) / sizeof(proc_misc_hints_table[0]) },
    { proc_ld_str_table, sizeof(proc_ld_str_table) / sizeof(proc_ld_str_table[0]) },
    { proc_pas_table, sizeof(proc_pas_table) / sizeof(proc_pas_table[0]) },
    { proc_pusr_table, sizeof(proc_pusr_table) / sizeof(proc_pusr_table[0]) },
    { proc_signed_mult_table, sizeof(proc_signed_mult_table) / sizeof(proc_signed_mult_table[0]) },
    { proc_other_media_table, sizeof(proc_other_media_table) / sizeof(proc_other_media_table[0]) },
    { proc_branch_block_table, sizeof(proc_branch_block_table) / sizeof(proc_branch_block_table[0]) },
    { proc_coproc_table, sizeof(proc_coproc_table) / sizeof(proc_coproc_table[0]) },
    { proc_uncond_misc_table, sizeof(proc_uncond_misc_table) / sizeof(proc_uncond_misc_table[0]) },
    { proc_uncond_table, sizeof(proc_uncond_table) / sizeof(proc_uncond_table[0]) }
};


// index has to match with group enum
void (*print_instr_table[])(Instr *) = {
    print_data_proc_instr,      // GROUP_DP_REG
    print_data_proc_instr,      // GROUP_DP_RSR
    print_misc_instr,            // GROUP_MISC
    print_half_mult_instr,     // GROUP_HM
    print_mult_instr,           // GROUP_MULT
    print_sync_instr,           // GROUP_SYNC
    print_load_store_instr,        // GROUP_EX_LD_STR
    print_load_store_instr,        // GROUP_EX_LD_STR_UNP
    print_data_proc_instr, // GROUP_DP_IMM
    print_data_proc_instr, // GROUP_DP_IMM16
    print_misc_instr, // GROUP_MISC_HINTS
    print_load_store_instr,   // GROUP_LD_STR
    print_parallel_add_sub_instr, // GROUP_PAS
    print_pusr_instr, // GROUP_PUSR
    print_mult_instr, // GROUP_SIGNED_MULT
    print_parallel_add_sub_instr, // GROUP_OTHER_MEDIA
    print_branch_block_instr, // GROUP_BRANCH_BLK
    print_coproc_instr, // GROUP_COPROC
    print_uncond_instr, // GROUP_UNCOND_MISC
    print_uncond_instr, // GROUP_UNCOND
    print_default_instr
};