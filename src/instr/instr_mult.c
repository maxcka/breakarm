#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"

// ----------------------------------------
// --- Multiply and multiply accumulate ---
// ----------------------------------------

void print_mult_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_MULT_0:
        {
            printf("%s%s %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            break;
        }

        case TYPE_MULT_1:
        {
            printf("%s%s%s %s, %s, %s, %s\n",
                instr_s->mnemonic,
                (instr_s->S) ? "S" : "",
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                core_reg[instr_s->Ra]);
            break;
        }

        case TYPE_MULT_2:
        {
            printf("%s%s %s, %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->RdLo],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            break;
        }

        case TYPE_MULT_3:
        {
            printf("%s%s %s, %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm],
                core_reg[instr_s->Ra]);
            break;
        }

        case TYPE_MULT_4:
        {
            printf("%s%s%s %s, %s, %s, %s\n",
                instr_s->mnemonic,
                (instr_s->S) ? "S" : "",
                cond_codes[instr_s->c],
                core_reg[instr_s->RdLo],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            break;
        }

        case TYPE_UNPRED:
        {
            printf("%s\n", UNPRED_STR);
            break;
        }

        default: 
        {
            printf("UNKNOWN\n");
            break;
        }
    }
}

// main processing function
int process_mult_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rd = (instr >> 16) & 0xF;
    instr_s->Rn = (instr >> 0) & 0xF;
    instr_s->Rm = (instr >> 8) & 0xF;

    instr_s->Ra = (instr >> 12) & 0xF;
    instr_s->RdLo = (instr >> 12) & 0xF;

    instr_s->S = (instr >> 20) & 0x1;


    if (instr_s->Rd == PC || instr_s->Rn == PC || instr_s->Rm == PC || 
        instr_s->Ra == PC || instr_s->RdLo == PC) {
        instr_s->itype = TYPE_UNPRED;
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

