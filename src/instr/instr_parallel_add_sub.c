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
        case TYPE_PAS_0: // syntax: <MNEMONIC><c> <Rd>, <Rn>, <Rm>
        {
            printf("%s%s %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                core_reg[instr_s->Rm]);
            break;
        }

        case TYPE_PAS_1: // syntax: <MNEMONIC><c> <Rd>, <Rn>, <Rm>, <Ra>
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

        case TYPE_PAS_FIELD_0: // syntax: <MNEMONIC><c> <Rd>, <Rn>, #<lsb>, #<width>
        case TYPE_PAS_FIELD_0_1: // syntax: <MNEMONIC><c> <Rd>, <Rn>, #<lsb>, #<width>
        {
            printf("%s%s %s, %s, #%d, #%d\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rn],
                instr_s->lsb,
                instr_s->width);
            break;
        }

        case TYPE_PAS_FIELD_1: // syntax: <MNEMONIC><c> <Rd>, #<lsb>, #<width>
        {
            printf("%s%s %s, #%d, #%d\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                instr_s->lsb,
                instr_s->width);
            break;
        }

        case TYPE_PAS_UDF: // syntax: <MNEMONIC><c> #<imm16>
        {
            printf("%s%s %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                instr_s->imm_str);
            break;
        }

        case TYPE_UNPRED:
        {
            instr_s->mnemonic = UNPRED_STR;
            printf("%s\n", instr_s->mnemonic);
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


int process_parallel_add_sub_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    if (IS_IGROUP(instr_s->igroup, GROUP_PAS)) {
        instr_s->Rm = (instr >> 0) & 0xF;
        instr_s->Rd = (instr >> 12) & 0xF;
        instr_s->Rn = (instr >> 16) & 0xF;
    }
    else {
        instr_s->Rm = (instr >> 8) & 0xF;
        instr_s->Rd = (instr >> 16) & 0xF;
        instr_s->Rn = (instr >> 0) & 0xF;
        instr_s->Ra = (instr >> 12) & 0xF;
    }

    instr_s->lsb = (instr >> 7) & 0x1F;
    uint8_t widthm1 = (instr >> 16) & 0x1F;
    uint8_t msb = (instr >> 16) & 0x1F;

    if (IS_ITYPE(instr_s->itype, TYPE_PAS_FIELD_0, TYPE_PAS_FIELD_0_1, TYPE_PAS_FIELD_1)) {
        instr_s->Rd = (instr >> 12) & 0xF;
    }

    if (IS_ITYPE(instr_s->itype, TYPE_PAS_FIELD_0)) {
        instr_s->width = widthm1 + 1;
    }
    else {
        instr_s->width = msb - instr_s->lsb + 1;
    }


    uint8_t imm4 = (instr >> 0) & 0xF;
    uint16_t imm12 = (instr >> 8) & 0xFFF;
    get_imm_str(instr_s, imm12, imm4, 4, TRUE);

    
    // unpred checking
    if (IS_ITYPE(instr_s->itype, TYPE_PAS_FIELD_0_1, TYPE_PAS_FIELD_1) && IS_TARGET_REG(PC, instr_s->Rd)) {
        instr_s->itype = TYPE_UNPRED;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_PAS_FIELD_0) && IS_TARGET_REG(PC, instr_s->Rd, instr_s->Rn)) {
        instr_s->itype = TYPE_UNPRED;
    }
    else if (IS_NOT_ITYPE(instr_s->itype, TYPE_PAS_UDF) && IS_TARGET_REG(PC, instr_s->Rd, instr_s->Rn, instr_s->Rm)) {
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
    instr_s.itype = TYPE_PAS_0;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "ADD16");
    instr_s.mnemonic = mnemonic_buf;

    return process_mult_instr(instr, &instr_s);
}

// syntax: ASX<c> <Rd>, <Rn>, <Rm>
int ASX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS_0;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "ASX");
    instr_s.mnemonic = mnemonic_buf;

    return process_mult_instr(instr, &instr_s);
}


// syntax: SAX<c> <Rd>, <Rn>, <Rm>
int SAX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS_0;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "SAX");
    instr_s.mnemonic = mnemonic_buf;

    return process_mult_instr(instr, &instr_s);
}

// syntax: SUB16<c> <Rd>, <Rn>, <Rm>
int SUB16_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS_0;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "SUB16");
    instr_s.mnemonic = mnemonic_buf;

    return process_mult_instr(instr, &instr_s);
}

// syntax: ADD8<c> <Rd>, <Rn>, <Rm>
int ADD8_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS_0;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "ADD8");
    instr_s.mnemonic = mnemonic_buf;

    return process_mult_instr(instr, &instr_s);
}

// syntax: SUB8<c> <Rd>, <Rn>, <Rm>
int SUB8_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_PAS;
    instr_s.itype = TYPE_PAS_0;

    char mnemonic_buf[BUF_20] = "";
    add_pas_prefix(instr, mnemonic_buf);
    strcat(mnemonic_buf, "SUB8");
    instr_s.mnemonic = mnemonic_buf;

    return process_mult_instr(instr, &instr_s);
}


//>> other media instructions

// USAD8, USADA8, SBFX, BFC, BFI, UBFX, UDF


// syntax: USAD8<c> <Rd>, <Rn>, <Rm>
int USAD8_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_0;

    instr_s.mnemonic = "USAD8";

    return process_mult_instr(instr, &instr_s);
}

// syntax: USADA8<c> <Rd>, <Rn>, <Rm>, <Ra>
int USADA8_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_1;

    instr_s.mnemonic = "USADA8";

    return process_mult_instr(instr, &instr_s);
}

// syntax: SBFX<c> <Rd>, <Rn>, #<lsb>, #<width>
int SBFX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_FIELD_0;

    instr_s.mnemonic = "SBFX";

    return process_mult_instr(instr, &instr_s);
}
// syntax: BFC<c> <Rd>, #<lsb>, #<width>
int BFC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_FIELD_1;

    instr_s.mnemonic = "BFC";

    return process_mult_instr(instr, &instr_s);
}
// syntax: BFI<c> <Rd>, <Rn>, #<lsb>, #<width> different
int BFI_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_FIELD_0_1;

    instr_s.mnemonic = "BFI";

    return process_mult_instr(instr, &instr_s);
}
// syntax: UBFX<c> <Rd>, <Rn>, #<lsb>, #<width>
int UBFX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_FIELD_0;

    instr_s.mnemonic = "UBFX";

    return process_mult_instr(instr, &instr_s);
}
// syntax: UDF<c> #<imm16>
int UDF_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_OTHER_MEDIA;
    instr_s.itype = TYPE_PAS_UDF;

    instr_s.mnemonic = "UDF";

    return process_mult_instr(instr, &instr_s);
}