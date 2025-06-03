#include <stdio.h>
#include "decode.h"


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
        
        case TYPE_0_RSR: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>
            printf("%s%s%s %s, %s, %s, %s %s\n", 
            instr_s->mnemonic, 
            (instr_s->S) ? "S" : "", 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            shift_codes[instr_s->shift.shift_t],
            core_reg[instr_s->Rs]);
            break;
        
        case TYPE_1: // syntax: <MNEMONIC><c> <Rn>, <Rm>{, <shift>}
            printf("%s%s %s, %s%s\n", 
            instr_s->mnemonic, 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);
            break;

        case TYPE_1_RSR: // syntax: <MNEMONIC><c> <Rn>, <Rm>, <type> <Rs>
            printf("%s%s %s, %s, %s %s\n", 
            instr_s->mnemonic, 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            shift_codes[instr_s->shift.shift_t],
            core_reg[instr_s->Rs]);
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

        case TYPE_3_RSR: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>
            printf("%s%s%s %s, %s, %s\n", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd],
            core_reg[instr_s->Rn],
            core_reg[instr_s->Rm]);
            break;

        case TYPE_4: // syntax: <MNEMONIC>S{<c>} <Rd>, <Rm>{, <shift>}
            printf("%s%s%s %s, %s%s\n", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm],
            instr_s->shift_str);
            break;

        case TYPE_4_RSR: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, <type> <Rs>
            printf("%s%s%s %s, %s, %s %s\n", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm],
            shift_codes[instr_s->shift.shift_t],
            core_reg[instr_s->Rs]);
            break;
    }
}
