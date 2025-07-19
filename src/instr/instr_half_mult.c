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


// -------------------------------------------------
// --- Halfword multiply and multiply accumulate ---
// -------------------------------------------------

// print
void print_half_mult_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_HM_0:
        {
            printf("%s%c%c%s %s, %s, %s, %s",
                instr_s->mnemonic,
                instr_s->x,
                instr_s->y,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                core_reg[instr_s->Ra]);
            print_unpred(instr_s);
			break;
        }

        case TYPE_HM_1:
        {
            printf("%s%c%s %s, %s, %s, %s",
                instr_s->mnemonic,
                instr_s->y,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                core_reg[instr_s->Ra]);
            print_unpred(instr_s);
			break;
        }

        case TYPE_HM_2:
        {
            printf("%s%c%s %s, %s, %s",
                instr_s->mnemonic,
                instr_s->y,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred(instr_s);
			break;
        }

        case TYPE_HM_3:
        {
            printf("%s%c%c%s %s, %s, %s, %s",
                instr_s->mnemonic,
                instr_s->x,
                instr_s->y,
                cond_codes[instr_s->c],
                core_reg[instr_s->RdLo],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred(instr_s);
			break;
        }

        case TYPE_HM_4:
        {
            printf("%s%c%c%s %s, %s, %s",
                instr_s->mnemonic,
                instr_s->x,
                instr_s->y,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            print_unpred(instr_s);
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
int process_half_mult_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rd = (instr >> 16) & 0xF;
    instr_s->Rn = (instr >> 0) & 0xF;
    instr_s->Rm = (instr >> 8) & 0xF;

    instr_s->Ra = (instr >> 12) & 0xF;
    instr_s->RdLo = (instr >> 12) & 0xF;

    uint8_t N = (instr >> 5) & 0x1;
    uint8_t M = (instr >> 6) & 0x1;
    instr_s->x = (N == 1) ? 'T' : 'B'; // top half, T, or bottom half, B
    instr_s->y = (M == 1) ? 'T' : 'B';

    if (instr_s->Rd == PC || instr_s->Rn == PC || instr_s->Rm == PC || 
        instr_s->Ra == PC || instr_s->RdLo == PC) {
        instr_s->is_unpred = TRUE;
    }

    print_asm_instr(instr_s);

    return 0;
}

// process instruction based on mnemonic
// syntax: SMLA<x><y><c> <Rd>, <Rn>, <Rm>, <Ra>
int SMLA_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMLA";


    instr_s.igroup = GROUP_HM;
    instr_s.itype = TYPE_HM_0;

    return process_half_mult_instr(instr, &instr_s);
}

// syntax: SMLAW<y><c> <Rd>, <Rn>, <Rm>, <Ra>
int SMLAW_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMLAW";


    instr_s.igroup = GROUP_HM;
    instr_s.itype = TYPE_HM_1;

    return process_half_mult_instr(instr, &instr_s);
}

// syntax: SMULW<y><c> <Rd>, <Rn>, <Rm>
int SMULW_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMULW";


    instr_s.igroup = GROUP_HM;
    instr_s.itype = TYPE_HM_2;

    return process_half_mult_instr(instr, &instr_s);
}

// syntax: SMLAL<x><y><c> <RdLo>, <RdHi>, <Rn>, <Rm>
int SMLALXY_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMLAL";


    instr_s.igroup = GROUP_HM;
    instr_s.itype = TYPE_HM_3;

    return process_half_mult_instr(instr, &instr_s);
}

// syntax: SMUL<x><y><c> <Rd>, <Rn>, <Rm>
int SMUL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMUL";


    instr_s.igroup = GROUP_HM;
    instr_s.itype = TYPE_HM_4;

    return process_half_mult_instr(instr, &instr_s);
}

