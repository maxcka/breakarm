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

static void add_acc_sub_affix(uint32_t instr, char *buf);

// ----------------------------------------
// --- Multiply and multiply accumulate ---
// ----------------------------------------

void print_mult_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_MULT_0_X:
        case TYPE_MULT_0_R:
        case TYPE_MULT_0:
        {
            printf("%s%s%s %s, %s, %s",
                instr_s->mnemonic,
                instr_s->str_suffix,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }
        
        case TYPE_MULT_1_X:
        case TYPE_MULT_1_R:
        case TYPE_MULT_1:
        {
            printf("%s%s%s %s, %s, %s, %s",
                instr_s->mnemonic,
                instr_s->str_suffix,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                core_reg[instr_s->Ra]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MULT_2:
        {
            printf("%s%s %s, %s, %s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->RdLo],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MULT_3:
        {
            printf("%s%s %s, %s, %s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                core_reg[instr_s->Ra]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MULT_4_X:
        case TYPE_MULT_4:
        {
            printf("%s%s%s %s, %s, %s, %s",
                instr_s->mnemonic,
                instr_s->str_suffix,
                cond_codes[instr_s->c],
                core_reg[instr_s->RdLo],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_UNPRED:
        {
            instr_s->mnemonic = UNPRED_STR;
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

// main processing function
int process_mult_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rd = (instr >> 16) & 0xF; // Rd is also used as RdHigh
    instr_s->Rn = (instr >> 0) & 0xF;
    instr_s->Rm = (instr >> 8) & 0xF;

    instr_s->Ra = (instr >> 12) & 0xF;
    instr_s->RdLo = (instr >> 12) & 0xF;

    
    if (IS_ITYPE(instr_s->itype, TYPE_MULT_1, TYPE_MULT_4)) {
        instr_s->S = ((instr >> 20) & 0x1) ? 'S' : 0; // new way of handling char suffix
    }
    if (IS_ITYPE(instr_s->itype, TYPE_MULT_0_X, TYPE_MULT_1_X, TYPE_MULT_4_X)) {
        instr_s->M = ((instr >> 5) & 0x1) ? 'X' : 0;
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_MULT_0_R, TYPE_MULT_1_R)) {
        instr_s->R = ((instr >> 5) & 0x1) ? 'R' : 0;
    }
    get_char_suffix(instr_s);

    if (IS_TARGET_REG(PC, instr_s->Rd, instr_s->Rn, instr_s->Rm, instr_s->Ra, instr_s->RdLo)) {
        instr_s->is_unpred = TRUE;
    }

    print_asm_instr(instr_s);

    return 0;
}


// process instruction based on mnemonic
// syntax: MUL<c> <Rd>, <Rn>, <Rm>
int MUL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MUL";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_0;

    return process_mult_instr(instr, &instr_s);
}

// syntax: MLA{S}<c> <Rd>, <Rn>, <Rm>, <Ra>
int MLA_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MLA";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_1;

    return process_mult_instr(instr, &instr_s);
}

// syntax: UMAAL<c> <RdLo>, <RdHi>, <Rn>, <Rm>
int UMAAL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "UMAAL";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_2;

    return process_mult_instr(instr, &instr_s);
}

// syntax: MLS<c> <Rd>, <Rn>, <Rm>, <Ra>
int MLS_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MLS";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_3;

    return process_mult_instr(instr, &instr_s);
}

// syntax: UMULL{S}<c> <RdLo>, <RdHi>, <Rn>, <Rm>
int UMULL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "UMULL";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_4;

    return process_mult_instr(instr, &instr_s);
}

// syntax: UMLAL{S}<c> <RdLo>, <RdHi>, <Rn>, <Rm>
int UMLAL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "UMLAL";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_4;

    return process_mult_instr(instr, &instr_s);
}

// syntax: SMULL{S}<c> <RdLo>, <RdHi>, <Rn>, <Rm>
int SMULL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMULL";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_4;

    return process_mult_instr(instr, &instr_s);
}

// syntax: SMLAL{S}<c> <RdLo>, <RdHi>, <Rn>, <Rm>
int SMLAL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMLAL";

    instr_s.igroup = GROUP_MULT;
    instr_s.itype = TYPE_MULT_4;

    return process_mult_instr(instr, &instr_s);
}

// ===================================================
// === signed multiply, signed and unsigned divide ===
// ===================================================

// === helper functions ===
static void add_acc_sub_affix(uint32_t instr, char *buf) {
    uint8_t op2 = (instr >> 5) & 0x7;
    if ((op2 & 0x2) == 0) { // bit 1 is 0 (means accumulate)
        strcat(buf, "A");
    }
    else {
        strcat(buf, "S");
    }
}
// ========================


// syntax: SMLAD{X}<c> <Rd>, <Rn>, <Rm>, <Ra>
// syntax: SMLSD{X}<c> <Rd>, <Rn>, <Rm>, <Ra>
int SMLXD_instr(uint32_t instr) {
    Instr instr_s = {0};

    instr_s.igroup = GROUP_SIGNED_MULT;
    instr_s.itype = TYPE_MULT_1_X;

    char mnemonic_buf[BUF_20] = "SML";
    add_acc_sub_affix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "D");

    instr_s.mnemonic = mnemonic_buf;
    return process_mult_instr(instr, &instr_s);
}

// syntax: SMUAD{X}<c> <Rd>, <Rn>, <Rm>
// syntax: SMUSD{X}<c> <Rd>, <Rn>, <Rm>
int SMUXD_instr(uint32_t instr) {
    Instr instr_s = {0};

    instr_s.igroup = GROUP_SIGNED_MULT;
    instr_s.itype = TYPE_MULT_0_X;

    char mnemonic_buf[BUF_20] = "SMU";
    add_acc_sub_affix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "D");

    instr_s.mnemonic = mnemonic_buf;
    return process_mult_instr(instr, &instr_s);
}


// syntax: SDIV<c> <Rd>, <Rn>, <Rm>
// syntax: UDIV<c> <Rd>, <Rn>, <Rm>
int XDIV_instr(uint32_t instr) {
    Instr instr_s = {0};

    instr_s.igroup = GROUP_SIGNED_MULT;
    instr_s.itype = TYPE_MULT_0;

    if (instr >> 21 == 0) { // bit 1 of op1 is 0 (means signed)
        instr_s.mnemonic = "SDIV";
    }
    else {
        instr_s.mnemonic = "UDIV";
    }

    return process_mult_instr(instr, &instr_s);
}

// syntax: SMLALD{X}<c> <RdLo>, <RdHi>, <Rn>, <Rm>
// syntax: SMLSLD{X}<c> <RdLo>, <RdHi>, <Rn>, <Rm>
int SMLXLD_instr(uint32_t instr) {
    Instr instr_s = {0};

    instr_s.igroup = GROUP_SIGNED_MULT;
    instr_s.itype = TYPE_MULT_4_X;

    char mnemonic_buf[BUF_20] = "SML";
    add_acc_sub_affix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "LD");

    instr_s.mnemonic = mnemonic_buf;
    return process_mult_instr(instr, &instr_s);
}

// syntax: SMMLA{R}<c> <Rd>, <Rn>, <Rm>, <Ra>
// syntax: SMMLS{R}<c> <Rd>, <Rn>, <Rm>, <Ra>
int SMMLX_instr(uint32_t instr) {
    Instr instr_s = {0};

    instr_s.igroup = GROUP_SIGNED_MULT;
    instr_s.itype = TYPE_MULT_1_R;

    char mnemonic_buf[BUF_20] = "SMML";
    add_acc_sub_affix(instr, mnemonic_buf);

    instr_s.mnemonic = mnemonic_buf;
    return process_mult_instr(instr, &instr_s);
}

// syntax: SMMUL{R}<c> <Rd>, <Rn>, <Rm>
int SMMUL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMMUL";

    instr_s.igroup = GROUP_SIGNED_MULT;
    instr_s.itype = TYPE_MULT_0_R;
    
    return process_mult_instr(instr, &instr_s);
}
