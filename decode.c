// Notes:
// - noticing a pattern between AND and ADD processing.
//   will maybe make helper functions/structs depending on how repetitive

#include <stdio.h>
#include <stdint.h>
#include <string.h>
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
        case TYPE_0: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
            printf("%s%s%s %s, %s, %s%s\n", 
            instr_s->mnemonic, 
            (instr_s->S) ? "S" : "", 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);

            break;
        
        case TYPE_1: // syntax: <MNEMONIC><c> <Rn>, <Rm>{, <shift>}
            printf("%s%s %s, %s%s\n", 
            instr_s->mnemonic, 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);

            break;

        case TYPE_2: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>
            printf("%s%s%s %s, %s\n", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm]);

            break;

        case TYPE_3: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, #<imm5>
            printf("%s%s%s %s, %s, #%d\n", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm],
            instr_s->shift.shift_n);

            break;

        case TYPE_S: // syntax: <MNEMONIC>S{<c>} <Rd>, <Rm>{, <shift>}
            printf("%s%s%s %s, %s%s\n", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
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


    // special case Encoding A2: <opc2>S{<c>}{<q>} <Rd>, <Rm> {, <shift>}
    if (instr_s->i_type != TYPE_1 && instr_s->S == 0x1 && instr_s->Rd == PC) {
        //instr_s->special = 1;
        instr_s->i_type = TYPE_S;
        instr_s->mnemonic = "MVN";
    }

    get_shift_str(instr_s->shift, instr_s->shift_str, BUF_20);

    print_asm_instr(instr_s);

    return 0;

}

// process AND (register) instruction
// syntax: AND{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
// for A32, <q> has no effect
int AND_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "AND";
    return process_data_proc_instr(instr, &instr_s); // return dummy value because functions need to have same return type to be stored in the same fn ptr array
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
int EOR_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "EOR";
    return process_data_proc_instr(instr, &instr_s);
}

// process SUB (register) instruction
// syntax: SUB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int SUB_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SUB";
    return process_data_proc_instr(instr, &instr_s);
}

// process RSB (register) instruction
// syntax: RSB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int RSB_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSB";
    return process_data_proc_instr(instr, &instr_s);
}

// process ADD (register, ARM) instruction
// syntax: ADD{S}{<c>}{<q>} {<Rd,} <Rn>, <Rm> {, <shift>}
int ADD_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADD";
    return process_data_proc_instr(instr, &instr_s);
}

// process ADC (register) instruction
// syntax: ADC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int ADC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADC";
    return process_data_proc_instr(instr, &instr_s);
}

// process SBC (register) instruction
// syntax: SBC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int SBC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SBC";
    return process_data_proc_instr(instr, &instr_s);
}

// process RSC (register) instruction
// syntax: RSC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int RSC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSC";
    return process_data_proc_instr(instr, &instr_s);
}

// process TST (register) instruction
// syntax: TST<c> <Rn>, <Rm>{, <shift>}
int TST_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TST";
    instr_s.i_type = TYPE_1;
    return process_data_proc_instr(instr, &instr_s);
}
// process TEQ (register) instruction
int TEQ_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TEQ";
    instr_s.i_type = TYPE_1;
    return process_data_proc_instr(instr, &instr_s);
}
// process CMP (register) instruction
int CMP_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMP";
    instr_s.i_type = TYPE_1;
    return process_data_proc_instr(instr, &instr_s);
}
// process CMN (register) instruction
int CMN_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMN";
    instr_s.i_type = TYPE_1;
    return process_data_proc_instr(instr, &instr_s);
}
// process ORR (register) instruction
int ORR_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ORR";
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
int LSL_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LSL";
    instr_s.i_type = TYPE_3;
    return process_data_proc_instr(instr, &instr_s);
}
// process LSR (immediate) instruction
int LSR_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LSR";
    instr_s.i_type = TYPE_3;
    return process_data_proc_instr(instr, &instr_s);
}
// process ASR (immediate) instruction
int ASR_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ASR";
    instr_s.i_type = TYPE_3;
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
int ROR_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ROR";
    instr_s.i_type = TYPE_3;
    return process_data_proc_instr(instr, &instr_s);
}
// process BIC (register) instruction
// syntax: BIC{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
int BIC_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BIC";
    return process_data_proc_instr(instr, &instr_s);
}
// process MVN (register) instruction
// syntax: MVN{S}<c> <Rd>, <Rm>{, <shift>}
int MVN_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MVN";
    instr_s.i_type = TYPE_S;
    return process_data_proc_instr(instr, &instr_s);
}


// if arr[0][0](instr) then arr[0][1](instr)

// ===============
// === Decoder ===
// ===============
void decode_dp_reg(uint32_t instr) {
    int num_rows = sizeof(proc_instr_table) / sizeof(proc_instr_table[0]);
    for (int i = 0; i < num_rows; i++) {
        if (proc_instr_table[i][0](instr)) { // if this instruction matches the current A32 instruction
            proc_instr_table[i][1](instr); // process the instruction
            return;
        }
    }
    printf("%s\n", default_str); // we don't have the corresponding A32 instruction
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