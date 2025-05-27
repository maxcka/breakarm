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


// ====================
// === INSTRUCTIONS ===
// ====================

// ----------------------------------
// --- Data-processing (register) ---
// ----------------------------------

// process AND (register) instruction
// syntax: AND{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
// for A32, <q> has no effect
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

// process EOR (register) instruction

// process SUB (register) instruction

// process RSB (register) instruction

// process ADD (register, ARM) instruction
// syntax: ADD{S}{<c>}{<q>} {<Rd,} <Rn>, <Rm> {, <shift>}
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

// process ADC (register) instruction

// process SBC (register) instruction

// process RSC (register) instruction





// ===============
// === Decoder ===
// ===============
void decode_dp_reg(uint32_t instr) {
    if (IS_AND_REG(instr)) {
        AND_reg_instr(instr); // process AND (register) instr
    }
    else if (IS_ADD_REG(instr)) {
        ADD_reg_instr(instr);
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