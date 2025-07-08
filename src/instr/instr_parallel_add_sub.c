#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"

static void add_pas_prefix(uint32_t instr, char *buf);
// -----------------------------------------
// --- Parallel addition and subtraction ---
// -----------------------------------------

void print_parallel_add_sub_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_PAS: // syntax: <MNEMONIC><c> <Rd>, <Rn>, <Rm>
        {
            printf("%s%s %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
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

int process_parallel_add_sub_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;
    instr_s->Rd = (instr >> 12) & 0xF;
    instr_s->Rn = (instr >> 16) & 0xF;

    if (instr_s->Rd == PC || instr_s->Rn == PC || instr_s->Rm == PC) {
        instr_s->itype = TYPE_UNPRED;
    }

    print_asm_instr(instr_s);

    return 0;
}


// helper function specific to parallel addition and subtraction
static void add_pas_prefix(uint32_t instr, char *buf) {
    if (IS_PAS_S(instr) && !IS_PAS_S_SAT(instr)) {
       strcat(buf, "S");
    }
    else if (IS_PAS_U(instr)) {
        strcat(buf, "U");
    }

    if (IS_PAS_S_SAT(instr)) {
        strcat(buf, "Q");
    }
    else if (IS_PAS_S_HALV(instr)) {
        strcat(buf, "H");
    }
}

// syntax: ADD16<c> <Rd>, <Rn>, <Rm>
int ADD16_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "ADD16");

    return process_mult_instr(instr, &instr_s);
}

// syntax: ASX<c> <Rd>, <Rn>, <Rm>
int ASX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "ASX");

    return process_mult_instr(instr, &instr_s);
}


// syntax: SAX<c> <Rd>, <Rn>, <Rm>
int SAX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "SAX");

    return process_mult_instr(instr, &instr_s);
}

// syntax: SUB16<c> <Rd>, <Rn>, <Rm>
int SUB16_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "SUB16");

    return process_mult_instr(instr, &instr_s);
}

// syntax: ADD8<c> <Rd>, <Rn>, <Rm>
int ADD8_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "ADD8");

    return process_mult_instr(instr, &instr_s);
}

// syntax: SUB8<c> <Rd>, <Rn>, <Rm>
int SUB8_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "SUB8");

    return process_mult_instr(instr, &instr_s);
}


//>> other media instructions

// USAD8, USADA8, SBFX, BFC, BFI, UBFX, UDF


// syntax: USAD8<c> <Rd>, <Rn>, <Rm>

// syntax: USADA8<c> <Rd>, <Rn>, <Rm>, <Ra>

// syntax: SBFX<c> <Rd>, <Rn>, #<lsb>, #<width>
// syntax: BFC<c> <Rd>, #<lsb>, #<width>
// syntax: BFI<c> <Rd>, <Rn>, #<lsb>, #<width>
// syntax: UBFX<c> <Rd>, <Rn>, #<lsb>, #<width>