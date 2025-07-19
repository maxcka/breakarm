/*
 * breakarm – A32 Instruction Disassembler
 * Copyright (c) 2025 Maximilian Kallas
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


// ====================
// === INSTRUCTIONS ===
// ====================

// only used for undefined instruction
// undefined is used in several instruction groups so better to put it in a central place
void print_default_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_UNDEF:
        case TYPE_UNPRED:
        case TYPE_NOT_IMP:
        {
            printf("%s 0x%08x\t; %s\n", DEFAULT_STR, curr_instr, instr_s->mnemonic);
            break;
        }

        default: 
        {
            instr_s->mnemonic = DEFAULT_STR;
            printf("%s 0x%08x\n", instr_s->mnemonic, curr_instr);
            break;
        }
    }
}


int UNDEF_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_DEFAULT;
    instr_s.itype = TYPE_UNDEF;
    instr_s.mnemonic = UNDEF_STR;

    (void)instr; // silence warning

    print_asm_instr(&instr_s);
    return 0;
}

int UNPRED_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_DEFAULT;
    instr_s.itype = TYPE_UNPRED;
    instr_s.mnemonic = UNPRED_STR;

    (void)instr; // silence warning

    print_asm_instr(&instr_s);
    return 0;
}

int NOT_IMP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_DEFAULT;
    instr_s.itype = TYPE_NOT_IMP;
    instr_s.mnemonic = NOT_IMP_STR;

    (void)instr; // silence warning

    print_asm_instr(&instr_s);
    return 0;
}


// ===============
// === Decoder ===
// ===============
void find_and_decode(uint32_t instr, IGroup igroup) {
    InstrHandlerTable IH_table = proc_instr_group_table[igroup]; // find the correct dispatch table based on the igroup
    int num_rows = IH_table.num_rows;
    InstrHandler (*proc_instr_table)[2] = IH_table.table;
    for (int i = 0; i < num_rows; i++) {
        if (proc_instr_table[i][0](instr)) { // if this instruction matches the current A32 instruction
            proc_instr_table[i][1](instr); // process the instruction
            return;
        }
    }
    printf("%s 0x%08x\n", DEFAULT_STR, curr_instr); // we don't have the corresponding A32 instruction
}

void decode_dp_op_0(uint32_t instr) {
    if (IS_DP_REG_OR_RSR(instr) && (IS_DP_REG(instr) || IS_DP_RSR(instr))) {           // layer 2
        if (IS_DP_REG(instr)) {               // layer 3
            find_and_decode(instr, GROUP_DP_REG);
        }
        else if (IS_DP_RSR(instr)) {          // layer 3
            find_and_decode(instr, GROUP_DP_RSR);
        }
        else {
            printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
        }
    }
    else if (IS_MISC_OR_HALF_MULT(instr) && (IS_MISC(instr) || IS_HALF_MULT(instr))) {  // layer 2
        if (IS_MISC(instr)) {                 // layer 3
            find_and_decode(instr, GROUP_MISC);
        }
        else if (IS_HALF_MULT(instr)) {     // layer 3
            find_and_decode(instr, GROUP_HM);
        }
        else {
            printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
        }
    }
    else if (IS_MULT_MULT(instr)) {          // layer 2
        find_and_decode(instr, GROUP_MULT);
    }
    else if (IS_SYNC(instr)) {               // layer 2
        find_and_decode(instr, GROUP_SYNC);
    }
    else if (IS_EX_LD_STR(instr)) {          // layer 2
        find_and_decode(instr, GROUP_EX_LD_STR);
    }
    else if (IS_EX_LD_STR_UNP(instr)) {      // layer 2
        find_and_decode(instr, GROUP_EX_LD_STR_UNP);
    }
    else {
        printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}

void decode_dp_op_1(uint32_t instr) {
    if (IS_DP_IMM(instr)) {                  // layer 2
        find_and_decode(instr, GROUP_DP_IMM);
    }
    else if (IS_16_IMM_LD(instr)) {          // layer 2
        find_and_decode(instr, GROUP_DP_IMM16);
    }
    else if (IS_MSR_HINTS(instr)) {          // layer 2
        find_and_decode(instr, GROUP_MISC_HINTS);
    }
    else {
        printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}

void decode_ld_str_med(uint32_t instr) {
    if (IS_LD_STR(instr)) {
        find_and_decode(instr, GROUP_LD_STR);
    }
    else if (IS_MED(instr)) {
        if (IS_PAS_S(instr) || IS_PAS_U(instr)) {
            find_and_decode(instr, GROUP_PAS);
        }
        else if (IS_PUSR(instr)) {
            find_and_decode(instr, GROUP_PUSR);
        }
        else if (IS_SMSUD(instr)) {
            find_and_decode(instr, GROUP_SIGNED_MULT);
        }
        else {
            find_and_decode(instr, GROUP_OTHER_MEDIA);
        }
    }
    else {
        printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}

void decode_br_blk(uint32_t instr) {
    find_and_decode(instr, GROUP_BRANCH_BLK);
}

void decode_co_spr(uint32_t instr) {
    if (IS_SIMD(instr) && !is_SVC(instr) && !is_UNDEF_4(instr)) { // is SIMD but not one of the general instructions
        printf("%s\n", NOT_IMP_STR);
    }
    else {
        find_and_decode(instr, GROUP_COPROC);
    }
}

void decode_uncond(uint32_t instr) {
    if (IS_MH_ASIMD_MISC(instr)) {
        find_and_decode(instr, GROUP_UNCOND_MISC);
    }
    else {
        find_and_decode(instr, GROUP_UNCOND);
    }
}

void decode_instr(uint32_t instr) {
    
    if (IS_COND(instr)) {                          // layer 0
        if (IS_DP_OP_0(instr)) {                    // layer 1
            decode_dp_op_0(instr);
        }
        else if (IS_DP_OP_1(instr)) {               // layer 1
            decode_dp_op_1(instr);
        }
        else if (IS_LD_STR_MED(instr)) {            // layer 1
            decode_ld_str_med(instr);
        }
        else if (IS_BR_BLK(instr)) {
            decode_br_blk(instr);
        }
        else if (IS_CO_SPR(instr)) {
            decode_co_spr(instr);
        }
        else {
            printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
        }
    }
    else if (IS_UNCOND(instr)) {
        decode_uncond(instr);
    }
    else {
            printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}