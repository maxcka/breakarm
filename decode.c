// Notes:
// - noticing a pattern between AND and ADD processing.
//   will maybe make helper functions/structs depending on how repetitive

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "decode_lookup.h"


static const char *default_str = "UNKNOWN";

Shift decode_imm_shift(ShiftType type, uint8_t imm5) {
    Shift shift;
    switch(type) {
        case LSL:
            shift.shift_t = LSL;
            shift.shift_n = imm5;
            break;
        case LSR:
            shift.shift_t = LSR;
            shift.shift_n = (imm5 == 0) ? 32 : imm5;
            break;
        case ASR:
            shift.shift_t = ASR;
            shift.shift_n = (imm5 == 0) ? 32 : imm5;
            break;
        case ROR:
        case RRX: // doesn't need to be here but compiler complains
            if (imm5 == 0) {
                shift.shift_t = RRX;
                shift.shift_n = 1;
            }
            else {
                shift.shift_t = ROR;
                shift.shift_n = imm5;
            }
            break;
    }

    return shift;
}

void get_shift_str(Shift shift, char *shift_str, int buf_sz) {
    if (shift.shift_n == 0) {
        shift_str[0] = '\0';
    }
    else {
        snprintf(shift_str, buf_sz, ", %s #%d", shift_codes[shift.shift_t], shift.shift_n);
    }
}


void print_asm_instr(Instr *instr_s) {
    switch (instr_s->i_type) {
        case TYPE_0: // syntax: MNEMONIC{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
            printf("%s%s%s %s, %s%s %s%s\n", 
            instr_s->mnemonic, 
            (instr_s->S) ? "S" : "", 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            (instr_s->special) ? "" : core_reg[instr_s->Rn], 
            (instr_s->special) ? "" : ",", 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);

            break;
        
        case TYPE_1: // syntax: MNEMONIC<c> <Rn>, <Rm>{, <shift>}
            printf("%s%s %s, %s%s\n", 
            instr_s->mnemonic, 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);

            break;
    }
}


// ====================
// === INSTRUCTIONS ===
// ====================

// ----------------------------------
// --- Data-processing (register) ---
// ----------------------------------

void process_data_proc_instr(uint32_t instr, Instr *instr_s) {

    // maybe add these vars into decode_imm_shift() depending on how repetitive
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;
    instr_s->shift = decode_imm_shift(type, imm5);
    instr_s->c =  (instr >> 28) & 0xF; // c is condition
    instr_s->Rd = (instr >> 12) & 0xF; // 0b1111
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;
    instr_s->S =  (instr >> 20) & 0x1;


    // special case Encoding A2: <opc2>S{<c>}{<q>} <Rd>, <Rm> {, <shift>}
    if (instr_s->i_type == TYPE_0 && instr_s->S == 0x1 && instr_s->Rd == PC) {
        instr_s->special = 1;
        instr_s->mnemonic = "MVN";
    }

    get_shift_str(instr_s->shift, instr_s->shift_str, BUF_20);

    print_asm_instr(instr_s);

}

// process AND (register) instruction
// syntax: AND{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
// for A32, <q> has no effect
void AND_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "AND";
    process_data_proc_instr(instr, &instr_s);
}
/*
void AND_reg_instr(uint32_t instr) {
    uint8_t special = 0;    // if special is 1, omit Rn
    const char *mnemonic = "AND";
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;
    Shift shift = decode_imm_shift(type, imm5);
    // read A8.4.3 for pseudocode of DecodeImmShift()

    Cond c =  (instr >> 28) & 0xF; // c is condition
    Register Rd = (instr >> 12) & 0xF; // 0b1111
    Register Rn = (instr >> 16) & 0xF;
    Register Rm = (instr >> 0) & 0xF;
    uint8_t S =  (instr >> 20) & 0x1;

    // special case Encoding A2: <opc2>S{<c>}{<q>} <Rd>, <Rm> {, <shift>}
    if (S == 0x1 && Rd == PC) {
        mnemonic = "MVN";
        special = 1;
    }

    char shift_str[BUF_20];
    get_shift_str(shift, shift_str, BUF_20);

    printf("%s%s%s %s, %s%s %s%s\n", mnemonic, (S) ? "S" : "", 
        cond_codes[c], core_reg[Rd], (special) ? "" : core_reg[Rn], 
        (special) ? "" : ",", core_reg[Rm], shift_str);
    
}
*/
// process EOR (register) instruction
// syntax: EOR{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
void EOR_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "EOR";
    process_data_proc_instr(instr, &instr_s);
}

// process SUB (register) instruction
// syntax: SUB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
void SUB_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SUB";
    process_data_proc_instr(instr, &instr_s);
}

// process RSB (register) instruction
// syntax: RSB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
void RSB_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSB";
    process_data_proc_instr(instr, &instr_s);
}

// process ADD (register, ARM) instruction
// syntax: ADD{S}{<c>}{<q>} {<Rd,} <Rn>, <Rm> {, <shift>}
void ADD_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADD";
    process_data_proc_instr(instr, &instr_s);
}
/*
void ADD_reg_instr(uint32_t instr) {
    uint8_t special = 0;
    const char *mnemonic = "ADD";
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;
    Shift shift = decode_imm_shift(type, imm5);

    Cond c =  (instr >> 28) & 0xF; // c is condition
    Register Rd = (instr >> 12) & 0xF; // 0b1111
    Register Rn = (instr >> 16) & 0xF;
    Register Rm = (instr >> 0) & 0xF;
    uint8_t S =  (instr >> 20) & 0x1;

    if (S == 0x1 && Rd == PC) {
        mnemonic = "MVN";
        special = 1;
    }

    char shift_str[BUF_20];
    get_shift_str(shift, shift_str, BUF_20);

    printf("%s%s%s %s, %s%s %s%s\n", mnemonic, (S) ? "S" : "", 
        cond_codes[c], core_reg[Rd], (special) ? "" : core_reg[Rn], 
        (special) ? "" : ",", core_reg[Rm], shift_str);


}
*/
// process ADC (register) instruction
// syntax: ADC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
void ADC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADC";
    process_data_proc_instr(instr, &instr_s);
}

// process SBC (register) instruction
// syntax: SBC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
void SBC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SBC";
    process_data_proc_instr(instr, &instr_s);
}

// process RSC (register) instruction
// syntax: RSC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
void RSC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSC";
    process_data_proc_instr(instr, &instr_s);
}

// process TST (register) instruction
void TST_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TST";
    instr_s.i_type = TYPE_1;
    process_data_proc_instr(instr, &instr_s);
}
// process TEQ (register) instruction
void TEQ_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TEQ";
    instr_s.i_type = TYPE_1;
    process_data_proc_instr(instr, &instr_s);
}
// process CMP (register) instruction
void CMP_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMP";
    instr_s.i_type = TYPE_1;
    process_data_proc_instr(instr, &instr_s);
}
// process CMN (register) instruction
void CMN_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMN";
    instr_s.i_type = TYPE_1;
    process_data_proc_instr(instr, &instr_s);
}
// process ORR (register) instruction
void ORR_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ORR";
    process_data_proc_instr(instr, &instr_s);
}
// process MOV (register) instruction
// process LSL (immediate) instruction
// process LSR (immediate) instruction
// process ASR (immediate) instruction
// process RRX instruction
// process ROR (immediate) instruction
// process BIC (register) instruction
// process MVN (register) instruction





// ===============
// === Decoder ===
// ===============
void decode_dp_reg(uint32_t instr) {
    if (IS_AND_REG(instr)) {
        AND_reg_instr(instr); // process AND (register) instr
    }
    else if (IS_EOR_REG(instr)) {
        EOR_reg_instr(instr);
    }
    else if (IS_SUB_REG(instr)) {
        SUB_reg_instr(instr);
    }
    else if (IS_RSB_REG(instr)) {
        RSB_reg_instr(instr);
    }
    else if (IS_ADD_REG(instr)) {
        ADD_reg_instr(instr);
    }
    else if (IS_ADC_REG(instr)) {
        ADC_reg_instr(instr);
    }
    else if (IS_SBC_REG(instr)) {
        SBC_reg_instr(instr);
    }
    else if (IS_RSC_REG(instr)) {
        RSC_reg_instr(instr);
    }
    else if (IS_TST_REG(instr)) {
        TST_reg_instr(instr);
    }
    else if (IS_TEQ_REG(instr)) {
        TEQ_reg_instr(instr);
    }
    else if (IS_CMP_REG(instr)) {
        CMN_reg_instr(instr);
    }
    else {
        printf("%s\n", default_str);
    }
}

void decode_instr(uint32_t instr) {
    
    if (IS_DP_OP_0(instr)) {
        if (IS_DP_REG_OR_RSR(instr)) {
            if (IS_DP_REG(instr)) {
                decode_dp_reg(instr);
            }
            else {
                printf("%s\n", default_str);
            }
        }
        else {
            printf("%s\n", default_str);
        }
    }
    else {
        printf("%s\n", default_str);
    }
}