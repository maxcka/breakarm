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

// -----------------------
// --- Data-processing ---
// -----------------------

int process_data_proc_instr(uint32_t instr, Instr *instr_s) {

    // maybe add these vars into decode_imm_shift() depending on how repetitive
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;
    instr_s->shift = decode_imm_shift(type, imm5);
    instr_s->c =  (instr >> 28) & 0xF; // c is condition
    instr_s->Rd = (instr >> 12) & 0xF; // 0b1111
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;
    instr_s->S =  (instr >> 20) & 0x1;
    // Rs used for register-shifted register instructions
    instr_s->Rs = (instr >> 8) & 0xF;


    // special case Encoding A2: <opc2>S{<c>}{<q>} <Rd>, <Rm> {, <shift>}
    if ((instr_s->i_type == TYPE_0 || instr_s->i_type == TYPE_2 || instr_s->i_type == TYPE_3) 
        && instr_s->S == 0x1 && instr_s->Rd == PC) {
        //instr_s->special = 1;
        instr_s->i_type = TYPE_4;
        instr_s->mnemonic = "MVN";
    }

    get_shift_str(instr_s->shift, instr_s->shift_str, BUF_20);

    print_asm_instr(instr_s);

    return 0;

}

// process AND (register) instruction
// syntax: AND{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
// for A32, <q> has no effect
int AND_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "AND";
    // this is kind of redundant but I think it's better than creating multiple functions for AND
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {
        instr_s.i_type = TYPE_0_RSR; // syntax: AND{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>
    }


    return process_data_proc_instr(instr, &instr_s); // return dummy value because functions need to have same return type to be stored in the same fn ptr array
}

// process EOR (register) instruction
// syntax: EOR{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int EOR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "EOR";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process SUB (register) instruction
// syntax: SUB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int SUB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SUB";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process RSB (register) instruction
// syntax: RSB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int RSB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSB";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process ADD (register, ARM) instruction
// syntax: ADD{S}{<c>}{<q>} {<Rd,} <Rn>, <Rm> {, <shift>}
int ADD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADD";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process ADC (register) instruction
// syntax: ADC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int ADC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADC";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process SBC (register) instruction
// syntax: SBC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int SBC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SBC";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process RSC (register) instruction
// syntax: RSC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int RSC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSC";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process TST (register) instruction
// syntax: TST<c> <Rn>, <Rm>{, <shift>}
int TST_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TST";
    
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_1;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_1_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process TEQ (register) instruction
int TEQ_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TEQ";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_1;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_1_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process CMP (register) instruction
int CMP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMP";
    
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_1;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_1_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process CMN (register) instruction
int CMN_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMN";
    
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_1;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_1_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process ORR (register) instruction
int ORR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ORR";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process MOV (register) instruction
// syntax: MOV{S}<c> <Rd>, <Rm>
int MOV_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MOV";
    instr_s.i_type = TYPE_2;
    return process_data_proc_instr(instr, &instr_s);
}
// process LSL (immediate) instruction
// syntax: LSL{S}<c> <Rd>, <Rm>, #<imm5>
int LSL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LSL";
    
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_3;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process LSR (immediate) instruction
int LSR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LSR";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_3;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process ASR (immediate) instruction
int ASR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ASR";
    
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_3;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process RRX instruction
// syntax: RRX{S}<c> <Rd>, <Rm>
int RRX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RRX";
    instr_s.i_type = TYPE_2;
    return process_data_proc_instr(instr, &instr_s);
}
// process ROR (immediate) instruction
// syntax: ROR{S}<c> <Rd>, <Rm>, #<imm>
int ROR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ROR";
    
    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_3;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process BIC (register) instruction
// syntax: BIC{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
int BIC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BIC";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_0;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_0_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process MVN (register) instruction
// syntax: MVN{S}<c> <Rd>, <Rm>{, <shift>}
int MVN_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MVN";

    if (IS_DP_REG(instr)) {
        instr_s.i_type = TYPE_4;
    }
    else if (IS_DP_RSR(instr)) {   
        instr_s.i_type = TYPE_4_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}


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