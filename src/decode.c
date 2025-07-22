/*
 * breakarm – A32 Instruction Disassembler
 * Copyright (c) 2025 Maximilian Kallas
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */

/*
 * Author: Maximilian Kallas
 * File: decode.c
 * Description: Decodes instructions by grouping them in categories based on the
 *              ARM architecture reference manual. Uses macros in bit_matching.h
 *              to find the appropriate group before calling its dispatch table
 *              which is defined in a32_tables.c
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


// ===============
// === Decoder ===
// ===============

// responsible for finding the correct instruction in the group's dispatch table
// and calling its *_instr() function which begins operand processing
void find_and_process(uint32_t instr, IGroup igroup) {
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

// all instructions here have bits 27-25 == 0b000
void decode_dp_op_0(uint32_t instr) {
    if (IS_DP_REG_OR_RSR(instr) && (IS_DP_REG(instr) || IS_DP_RSR(instr))) {           // layer 2
        if (IS_DP_REG(instr)) {               // layer 3
            find_and_process(instr, GROUP_DP_REG);
        }
        else if (IS_DP_RSR(instr)) {          // layer 3
            find_and_process(instr, GROUP_DP_RSR);
        }
        else {
            printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
        }
    }
    else if (IS_MISC_OR_HALF_MULT(instr) && (IS_MISC(instr) || IS_HALF_MULT(instr))) {  // layer 2
        if (IS_MISC(instr)) {                 // layer 3
            find_and_process(instr, GROUP_MISC);
        }
        else if (IS_HALF_MULT(instr)) {     // layer 3
            find_and_process(instr, GROUP_HM);
        }
        else {
            printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
        }
    }
    else if (IS_MULT_MULT(instr)) {          // layer 2
        find_and_process(instr, GROUP_MULT);
    }
    else if (IS_SYNC(instr)) {               // layer 2
        find_and_process(instr, GROUP_SYNC);
    }
    else if (IS_EX_LD_STR(instr)) {          // layer 2
        find_and_process(instr, GROUP_EX_LD_STR);
    }
    else if (IS_EX_LD_STR_UNP(instr)) {      // layer 2
        find_and_process(instr, GROUP_EX_LD_STR_UNP);
    }
    else {
        printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}

// all instructions here have bits 27-25 == 0b001
void decode_dp_op_1(uint32_t instr) {
    if (IS_DP_IMM(instr)) {                  // layer 2
        find_and_process(instr, GROUP_DP_IMM);
    }
    else if (IS_16_IMM_LD(instr)) {          // layer 2
        find_and_process(instr, GROUP_DP_IMM16);
    }
    else if (IS_MSR_HINTS(instr)) {          // layer 2
        find_and_process(instr, GROUP_MISC_HINTS);
    }
    else {
        printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}

// all instructions here have bits 27-25 == 0b01x
void decode_ld_str_med(uint32_t instr) {
    if (IS_LD_STR(instr)) {
        find_and_process(instr, GROUP_LD_STR);
    }
    else if (IS_MED(instr)) {
        if (IS_PAS_S(instr) || IS_PAS_U(instr)) {
            find_and_process(instr, GROUP_PAS);
        }
        else if (IS_PUSR(instr)) {
            find_and_process(instr, GROUP_PUSR);
        }
        else if (IS_SMSUD(instr)) {
            find_and_process(instr, GROUP_SIGNED_MULT);
        }
        else {
            find_and_process(instr, GROUP_OTHER_MEDIA);
        }
    }
    else {
        printf("%s 0x%08x\n", DEFAULT_STR, curr_instr);
    }
}

// all instructions here have bits 27-25 == 0b10x
void decode_br_blk(uint32_t instr) {
    find_and_process(instr, GROUP_BRANCH_BLK);
}

// all instructions here have bits 27-25 == 0b11x
void decode_co_spr(uint32_t instr) {
    if (IS_SIMD(instr) && !is_SVC(instr) && !is_UNDEF_4(instr)) { // is SIMD but not one of the general instructions
        printf("%s\n", NOT_IMP_STR);
    }
    else {
        find_and_process(instr, GROUP_COPROC);
    }
}


void decode_uncond(uint32_t instr) {
    if (IS_MH_ASIMD_MISC(instr)) {
        find_and_process(instr, GROUP_UNCOND_MISC);
    }
    else {
        find_and_process(instr, GROUP_UNCOND);
    }
}

// top-level function that finds the instruction's group by peeling back layers
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