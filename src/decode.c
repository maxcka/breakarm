// Notes:
// - noticing a pattern between AND and ADD processing.
//   will maybe make helper functions/structs depending on how repetitive

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


static const char *default_str = "UNKNOWN";

// ====================
// === INSTRUCTIONS ===
// ====================



// ---------------------
// --- Miscellaneous ---
// ---------------------

// ===============
// === Decoder ===
// ===============
void find_and_decode(uint32_t instr, int start_idx, int end_idx) {
    //int num_rows = sizeof(proc_instr_table) / sizeof(proc_instr_table[0]);
    for (int i = start_idx; i < end_idx; i++) {
        if (proc_instr_table[i][0](instr)) { // if this instruction matches the current A32 instruction
            proc_instr_table[i][1](instr); // process the instruction
            return;
        }
    }
    printf("%s\n", default_str); // we don't have the corresponding A32 instruction
}

void decode_dp_op_0(uint32_t instr) {
    if (IS_DP_REG_OR_RSR(instr)) {           // layer 2
        if (IS_DP_REG(instr)) {               // layer 3
            find_and_decode(instr, DP_REG_START, DP_REG_END);
        }
        else if (IS_DP_RSR(instr)) {          // layer 3
            find_and_decode(instr, DP_RSR_START, DP_RSR_END);
        }
        else {
            printf("%s\n", default_str);
        }
    }
    else if (IS_MISC_OR_HALF_MULT(instr)) {  // layer 2
        if (IS_MISC(instr)) {                 // layer 3
            find_and_decode(instr, MISC_START, 42);
        }
        else if (IS_HALF_MULT(instr)) {     // layer 3
            
        }
        else {
            printf("%s\n", default_str);
        }
    }
    //else if (IS_MULT_MULT(instr)) {          // layer 2
    //
    //}
    //else if (IS_SYNC(instr)) {               // layer 2
    //
    //}
    //else if (IS_EX_LD_STR(instr)) {          // layer 2
    //
    //}
    //else if (IS_EX_LD_STR_UNP(instr)) {      // layer 2
    //
    //}
    else {
        printf("%s\n", default_str);
    }
}

void decode_dp_op_1(uint32_t instr) {
    if (IS_DP_IMM(instr)) {                  // layer 2

    }
    else if (IS_16_IMM_LD(instr)) {          // layer 2

    }
    else if (IS_MSR_HINTS(instr)) {          // layer 2

    }
    else {
        printf("%s\n", default_str);
    }
}

void decode_ld_str_med(uint32_t instr) {
    if (IS_LD_STR(instr)) {

    }
    else if (IS_MED(instr)) {

    }
    else {
        printf("%s\n", default_str);
    }
}

void decode_br_blk(uint32_t instr) {
    instr += 1;
}

void decode_co_spr(uint32_t instr) {
    instr += 1;
}

void decode_instr(uint32_t instr) {
    
    if (IS_COND(instr)) {                          // layer 0
        if (IS_DP_OP_0(instr)) {                    // layer 1
            decode_dp_op_0(instr);
        }
        //else if (IS_DP_OP_1(instr)) {               // layer 1
        //    decode_dp_op_1(instr);
        //}
        //else if (IS_LD_STR_MED(instr)) {            // layer 1
        //    decode_ld_str_med(instr);
        //}
        //else if (IS_BR_BLK(instr)) {
        //    decode_br_blk(instr);
        //}
        //else if (IS_CO_SPR(instr)) {
        //    decode_co_spr(instr);
        //}
        else {
            printf("%s\n", default_str);
        }
    }
    else if (IS_UNCOND(instr)) {

    }
    else {
            printf("%s\n", default_str);
    }
}