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


static void get_pusr_shift(Instr *instr_s, uint8_t imm5, uint8_t sh);
static void add_XT_instr_affix(uint32_t instr, char *buf, char *base_name);
static IType add_SAT_instr_affix(uint32_t instr, char *buf, char *base_name);

// ----------------------------------------------------
// --- packing, unpacking, saturation, and reversal ---
// ----------------------------------------------------

void print_pusr_instr(Instr *instr_s) {
    switch (instr_s->itype) {

        case TYPE_PUSR_0: // syntax: <MNEMONIC><x><y><c> <Rd>, <Rn>, <Rm> {, <shift>}
        {
            printf("%s%c%c%s %s, %s, %s%s",
                instr_s->mnemonic,
                instr_s->x,
                instr_s->y,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                instr_s->shift_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_PUSR_1: // syntax: <MNEMONIC><c> <Rd>, <Rn>, <Rm>{, <rotation>}
        {
            printf("%s%s %s, %s, %s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                instr_s->shift_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_PUSR_2: // syntax: <MNEMONIC><c> <Rd>, <Rm>{, <rotation>}
        {
            printf("%s%s %s, %s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rm],
                instr_s->shift_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_PUSR_SAT: // syntax: <MNEMONIC><c> <Rd>, #<imm>, <Rn>{, <shift>}
        {
            printf("%s%s %s, %s, %s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                instr_s->imm_str,
                core_reg[instr_s->Rn],
                instr_s->shift_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_PUSR_SAT16: // syntax: <MNEMONIC><c> <Rd>, #<imm>, <Rn>
        {
            printf("%s%s %s, %s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                instr_s->imm_str,
                core_reg[instr_s->Rn]);
            print_unpred_or_newline(instr_s);
            break;
        }

        case TYPE_PUSR_4: // syntax: <MNEMONIC><c> <Rd>, <Rn>, <Rm>
        {
            printf("%s%s %s, %s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_PUSR_5: // syntax: <MNEMONIC><c> <Rd>, <Rm>
        {
            printf("%s%s %s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
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

int process_pusr_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rd = (instr >> 12) & 0xF;
    instr_s->Rn = (instr >> 16) & 0xF;
    uint8_t sat_imm = (instr >> 16) & 0x1F;
    instr_s->Rm = (instr >> 0) & 0xF;
    uint8_t tb = (instr >> 6) & 0x1;
    uint8_t sh = (instr >> 6) & 0x1;
    
    instr_s->x = (tb == 1) ? 'T' : 'B';
    instr_s->y = (tb == 1) ? 'B' : 'T';

    uint8_t imm5 = (instr >> 7) & 0x1F;

    uint8_t rotate = (instr >> 10) & 0x3;

    if (instr_s->itype == TYPE_PUSR_0) {
        get_pusr_shift(instr_s, imm5, tb);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_PUSR_1, TYPE_PUSR_2)) {
        if (rotate != 0) {
            get_shift_str(instr_s, ROR, rotate*8);
        }
        else {
            instr_s->shift_str[0] = '\0';
        }
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_PUSR_SAT, TYPE_PUSR_SAT16)) {
        instr_s->Rn = (instr >> 0) & 0xF;
        uint8_t extra_sat = (instr_s->mnemonic[0] == 'S') ? 1 : 0; // SSAT, and SSAT16 add +1 to imm
        get_imm_str(instr_s, sat_imm+extra_sat, 0, 0, TRUE);
        get_pusr_shift(instr_s, imm5, sh);
    }

    // handle unpredictable
    if (IS_NOT_ITYPE(instr_s->itype, TYPE_PUSR_2, TYPE_PUSR_5) &&
        IS_TARGET_REG(PC, instr_s->Rn)) {
        instr_s->is_unpred = TRUE;
    }
    if (IS_NOT_ITYPE(instr_s->itype, TYPE_PUSR_SAT, TYPE_PUSR_SAT16) &&
        IS_TARGET_REG(PC, instr_s->Rm)) {
        instr_s->is_unpred = TRUE;
    }
    if (IS_TARGET_REG(PC, instr_s->Rd)) {
        instr_s->is_unpred = TRUE;
    }


    print_asm_instr(instr_s);
    return 0;
}

// === Helper functions ===
static void get_pusr_shift(Instr *instr_s, uint8_t imm5, uint8_t sh) {
    if (sh == 1) {
        get_shift_str(instr_s, ASR, imm5);
    }
    else {
        get_shift_str(instr_s, LSL, imm5);
    }
}

static void add_XT_instr_affix(uint32_t instr, char *buf, char *base_name) {
    uint8_t op1 = (instr >> 20) & 0x7;
    if ((op1 & 0x4) == 0) { // bit 2 is 0 (means signed)
        strcat(buf, "S");
    }
    else {
        strcat(buf, "U");
    }
    strcat(buf, base_name);
    if ((op1 & 0x2) == 0) { // bit 1 is 0 (means 16-bit)
        strcat(buf, "16");
    }
}

static IType add_SAT_instr_affix(uint32_t instr, char *buf, char *base_name) {
    uint8_t op1 = (instr >> 20) & 0x7;
    uint8_t op2 = (instr >> 5) & 0x7;
    IType itype = TYPE_PUSR_SAT;
    if ((op1 & 0x4) == 0) { // bit 2 is 0 (means signed)
        strcat(buf, "S");
    }
    else {
        strcat(buf, "U");
    }   
    strcat(buf, base_name);
    if ((op2 & 0x1) == 1) { // bit 0 is 1 (means 16-bit)
        strcat(buf, "16");
        itype = TYPE_PUSR_SAT16;
    }
    return itype;
}
// ========================


// syntax: PKHBT{<c>} {<Rd>,} <Rn>, <Rm> {, LSL #<imm>}  (tbform == FALSE)
// syntax: PKHTB{<c>} {<Rd>,} <Rn>, <Rm> {, ASR #<imm>}  (tbform == TRUE)
int PKH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "PKH";


    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_0;

    return process_pusr_instr(instr, &instr_s);
}


// SXTAB
// UXTAB
// syntax: SXTAB16<c> <Rd>, <Rn>, <Rm>{, <rotation>}
// UXTAB16
int XTAB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_1;
    
    char mnemonic_buf[BUF_20] = "";

    add_XT_instr_affix(instr, mnemonic_buf, "XTAB");

    instr_s.mnemonic = mnemonic_buf;

    return process_pusr_instr(instr, &instr_s);


}


// SXTB
// UXTB
// syntax: SXTB16<c> <Rd>, <Rm>{, <rotation>}
// UXTB16
int XTB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_2;
    
    char mnemonic_buf[BUF_20] = "";

    add_XT_instr_affix(instr, mnemonic_buf, "XTB");

    instr_s.mnemonic = mnemonic_buf;

    return process_pusr_instr(instr, &instr_s);
}


// syntax: SSAT<c> <Rd>, #<imm>, <Rn>{, <shift>}
// SSAT16
// USAT
// USAT16
int SAT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_SAT;
    
    char mnemonic_buf[BUF_20] = "";

    // itype changes depending on affixes
    instr_s.itype = add_SAT_instr_affix(instr, mnemonic_buf, "SAT");

    instr_s.mnemonic = mnemonic_buf;

    return process_pusr_instr(instr, &instr_s); 
}



// syntax: SEL<c> <Rd>, <Rn>, <Rm>
int SEL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_4;
    instr_s.mnemonic = "SEL";


    return process_pusr_instr(instr, &instr_s); 
}

// syntax: REV<c> <Rd>, <Rm>
// REV16
int REV_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_5;

    if (((instr >> 5) & 0x4) == 0) { // bit 2 of op2 is 0
        instr_s.mnemonic = "REV";
    }
    else {
        instr_s.mnemonic = "REV16";
    }

    return process_pusr_instr(instr, &instr_s); 
}

// syntax: SXTAH<c> <Rd>, <Rn>, <Rm>{, <rotation>}
// UXTAH
int XTAH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_1;
    
    if (((instr >> 20) & 0x4) == 0) { // bit 2 of op1 is 0
        instr_s.mnemonic = "SXTAH";
    }
    else {
        instr_s.mnemonic = "UXTAH";
    }


    return process_pusr_instr(instr, &instr_s); 
}

// syntax: SXTH<c> <Rd>, <Rm>{, <rotation>}
// UXTH
int XTH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_2;
    
    if (((instr >> 20) & 0x4) == 0) { // bit 2 of op1 is 0
        instr_s.mnemonic = "SXTH";
    }
    else {
        instr_s.mnemonic = "UXTH";
    }

    return process_pusr_instr(instr, &instr_s); 
}

// syntax: RBIT<c> <Rd>, <Rm>
int RBIT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_5;
    instr_s.mnemonic = "RBIT";

    return process_pusr_instr(instr, &instr_s); 
}

// syntax: REVSH<c> <Rd>, <Rm>
int REVSH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PUSR;
    instr_s.itype = TYPE_PUSR_5;
    instr_s.mnemonic = "REVSH";

    return process_pusr_instr(instr, &instr_s); 
}


