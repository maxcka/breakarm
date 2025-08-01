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

static void print_ls_imm_syntactic_sugar(Instr *instr_s);

// ------------------
// --- Load/Store ---
// ------------------

void print_load_store_instr(Instr *instr_s) {
    switch (instr_s->itype) {

        case TYPE_LS_REG: // syntax: <MNEMONIC><c> <Rt>, [<Rn>,+/-<Rm>{, <shift>}]{!}
        {
            printf("%s%s %s, [%s%s, %s%s%s%s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rn],
                (instr_s->index == FALSE) ? "]" : "", // post-indexed
                (instr_s->add == 1) ? "" : "-",
                core_reg[instr_s->Rm],
                instr_s->shift_str,
                (instr_s->index == TRUE) ? "]" : "", // offset or pre-indexed
                (instr_s->index == TRUE && instr_s->wback == TRUE) ? "!" : "");
            print_unpred_or_newline(instr_s);
			break;
        }
        case TYPE_EX_LS_REG: // is an UNPRED edge case possible where index == False and wback == False??
        {
            printf("%s%s %s, [%s%s, %s%s%s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rn],
                (instr_s->index == FALSE) ? "]" : "", // post-indexed
                (instr_s->add == 1) ? "" : "-",
                core_reg[instr_s->Rm],
                (instr_s->index == TRUE) ? "]" : "", // offset or pre-indexed
                (instr_s->index == TRUE && instr_s->wback == TRUE) ? "!" : "");
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_LS_IMM:
        case TYPE_LS_IMM_STR:
        case TYPE_EX_LS_IMM_STR: // str imm
        case TYPE_EX_LS_IMM: // normal imm
        {
            print_ls_imm_syntactic_sugar(instr_s);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_EX_LS_DUAL_REG: // dual reg
        {
            printf("%s%s %s, %s, [%s%s, %s%s%s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rt2],
                core_reg[instr_s->Rn],
                (instr_s->index == FALSE) ? "]" : "", // post-indexed
                (instr_s->add == 1) ? "" : "-",
                core_reg[instr_s->Rm],
                (instr_s->index == TRUE) ? "]" : "", // offset or pre-indexed
                (instr_s->index == TRUE && instr_s->wback == TRUE) ? "!" : "");
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_EX_LS_DUAL_IMM: // dual imm
        case TYPE_EX_LS_DUAL_IMM_STR: 
        {
            printf("%s%s %s, %s, [%s%s, %s%s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rt2],
                core_reg[instr_s->Rn],
                (instr_s->index == FALSE) ? "]" : "", // post-indexed

                instr_s->imm_str,
                (instr_s->index == TRUE) ? "]" : "", // offset or pre-indexed
                (instr_s->index == 1 && instr_s->wback == 1) ? "!" : "");
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

// =============== Filtering/Syntactic Sugar ==============
// syntactic sugar for push and pop
static void print_ls_imm_syntactic_sugar(Instr *instr_s) {
    char buf[BUF_40];
    sprintf(buf, "%s%s %s, [%s%s, %s%s%s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rn],
                (instr_s->index == FALSE) ? "]" : "", // post-indexed
                instr_s->imm_str,
                (instr_s->index == TRUE) ? "]" : "", // offset or pre-indexed
                (instr_s->index == 1 && instr_s->wback == 1) ? "!" : "");
    
    int second_cmp_idx = 6;
    int used_sugar = 0;
    // compare with pop: LDR XX, [SP], #4
    if (strncmp(buf, "LDR ", 4) == 0 && strncmp(buf+second_cmp_idx, ", [SP], #4", 10) == 0) {
        printf("POP {%s}\t@ (", core_reg[instr_s->Rt]);
        used_sugar = 1;
    }
    // compare with push: STR XX, [SP, #-4]!
    else if (strncmp(buf, "STR ", 4) == 0 && strncmp(buf+second_cmp_idx, ", [SP, #-4]!", 12) == 0) {
        printf("PUSH {%s}\t@ (", core_reg[instr_s->Rt]);
        used_sugar = 1;
    }

    printf("%s%s", buf, (used_sugar) ? ")" : "");
}
// ==========================================================

// main processing function
int process_load_store_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rt = (instr >> 12) & 0xF;
    instr_s->Rt2 = UNINIT;
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rm = UNINIT;
    uint8_t P = (instr >> 24) & 0x1;
    uint8_t U = (instr >> 23) & 0x1;
    uint8_t W = (instr >> 21) & 0x1;
    instr_s->index = P;
    instr_s->add = U;
    instr_s->wback = (P == 0) || (W == 1);
    uint8_t imm4L = (instr >> 0) & 0xF;
    uint8_t imm4H = (instr >> 8) & 0xF;
    uint16_t imm12 = (instr >> 0) & 0xFFF;

    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;

    // doing itype-specific actions
    if (IS_ITYPE(instr_s->itype, TYPE_EX_LS_REG)) {
        instr_s->Rm = (instr >> 0) & 0xF;
    }
    else if (instr_s->itype == TYPE_LS_REG)  {
        instr_s->Rm = (instr >> 0) & 0xF;
        get_shift_str(instr_s, type, imm5);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_EX_LS_IMM_STR, TYPE_EX_LS_IMM)) {
        get_imm_str(instr_s, imm4H, imm4L, 4, instr_s->add);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_LS_IMM_STR, TYPE_LS_IMM)) {
        get_imm_str(instr_s, imm12, 0, 0, instr_s->add);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_EX_LS_DUAL_REG)) {
        instr_s->Rm = (instr >> 0) & 0xF;
        instr_s->Rt2 = instr_s->Rt + 1;
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_EX_LS_DUAL_IMM, TYPE_EX_LS_DUAL_IMM_STR)) {
        get_imm_str(instr_s, imm4H, imm4L, 4, instr_s->add);
        instr_s->Rt2 = instr_s->Rt + 1;
    }

    // checking if unpredictable
    if (IS_IGROUP(instr_s->igroup, GROUP_EX_LD_STR, GROUP_EX_LD_STR_UNP) && 
        instr_s->Rt == PC) {

        instr_s->is_unpred = TRUE;
    }
    if ((IS_TARGET_REG(PC, instr_s->Rm, instr_s->Rt2) ||
        (instr_s->wback && (instr_s->Rn == instr_s->Rt || instr_s->Rn == instr_s->Rt2)))) {

        instr_s->is_unpred = TRUE;
    }
    // applies to reg and str immediate instructions
    if ((IS_NOT_ITYPE(instr_s->itype, TYPE_EX_LS_IMM, TYPE_EX_LS_DUAL_IMM, TYPE_LS_IMM) && 
        (instr_s->wback && instr_s->Rn == PC))) {

         instr_s->is_unpred = TRUE;
    }
    // applies to dual instructions
    if (IS_ITYPE(instr_s->itype, TYPE_EX_LS_DUAL_REG, TYPE_EX_LS_DUAL_IMM, TYPE_EX_LS_DUAL_IMM_STR) &&
        (instr_s->Rm == instr_s->Rt || (instr_s->Rt & 0x1) == 1)) {
        instr_s->is_unpred = TRUE;
    }

    print_asm_instr(instr_s);
    return 0;

}

// process instruction based on mnemonic

//========================
//=== Extra Load/Store ===
//========================

// syntax: STRH<c> <Rt>, [<Rn>,+/-<Rm>]{!}
// syntax: STRH<c> <Rt>, [<Rn>],+/-<Rm>
int STRH_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRH";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_REG;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRH<c> <Rt>, [<Rn>,+/-<Rm>]{!}
int LDRH_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRH";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_REG;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: STRH<c> <Rt>, [<Rn>, #+/-<imm8>]{!}
int STRH_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRH";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_IMM_STR;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRH<c> <Rt>, [<Rn>, #+/-<imm8>]{!}
// syntax: LDRH<c> <Rt>, [<Rn>], #+/-<imm>
// literal is same as immediate just explicitly using PC as Rn
int LDRH_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRH";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_IMM;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRD<c> <Rt>, <Rt2>, [<Rn>,+/-<Rm>]{!}
int LDRD_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRD";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_DUAL_REG;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRSB<c> <Rt>, [<Rn>,+/-<Rm>]{!}
int LDRSB_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRSB";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_REG;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRD<c> <Rt>, [<Rn>, #+/-<imm8>]{!}
// also covers LDRD (literal)
int LDRD_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRD";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_DUAL_IMM;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRSB<c> <Rt>, [<Rn>, #+/-<imm8>]{!}
// syntax: LDRSB<c> <Rt>, [<Rn>], #+/-<imm>
// literal is same as immediate just explicitly using PC as Rn
int LDRSB_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRSB";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_IMM;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: STRD<c> <Rt>, <Rt2>, [<Rn>,+/-<Rm>]{!}
int STRD_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRD";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_DUAL_REG;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRSH<c> <Rt>, [<Rn>,+/-<Rm>]{!}
int LDRSH_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRSH";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_REG;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: STRD<c> <Rt>, <Rt2>, [<Rn>, #+/-<imm8>]!
int STRD_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRD";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_DUAL_IMM_STR;

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDRSH<c> <Rt>, [<Rn>, #+/-<imm8>]{!}
int LDRSH_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRSH";

    instr_s.igroup = GROUP_EX_LD_STR;
    instr_s.itype = TYPE_EX_LS_IMM;

    return process_load_store_instr(instr, &instr_s);
}

// NOTE: it seems like bit 22 determines if it is a register or an immediate instruction
// might have to change all previous functions in refactoring stage
// for next functions, "Extra load/store instructions, unprivileged", will handle more concisely


//======================================
//=== Extra Load/Store, unprivileged ===
//======================================
int STRHT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRHT";

    instr_s.igroup = GROUP_EX_LD_STR_UNP;
    // check bit 22
    if (((instr >> 22) & 0x1) == 0) {
        instr_s.itype = TYPE_EX_LS_REG;
    }
    else {
        instr_s.itype = TYPE_EX_LS_IMM_STR;
    }

    return process_load_store_instr(instr, &instr_s);
}


int LDRHT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRHT";

    instr_s.igroup = GROUP_EX_LD_STR_UNP;
    // check bit 22
    if (((instr >> 22) & 0x1) == 0) {
        instr_s.itype = TYPE_EX_LS_REG;
    }
    else {
        instr_s.itype = TYPE_EX_LS_IMM;
    }

    return process_load_store_instr(instr, &instr_s);
}

int LDRSBT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRSBT";

    instr_s.igroup = GROUP_EX_LD_STR_UNP;
    // check bit 22
    if (((instr >> 22) & 0x1) == 0) {
        instr_s.itype = TYPE_EX_LS_REG;
    }
    else {
        instr_s.itype = TYPE_EX_LS_IMM;
    }

    return process_load_store_instr(instr, &instr_s);
}

int LDRSHT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRSHT";

    instr_s.igroup = GROUP_EX_LD_STR_UNP;
    // check bit 22
    if (((instr >> 22) & 0x1) == 0) {
        instr_s.itype = TYPE_EX_LS_REG;
    }
    else {
        instr_s.itype = TYPE_EX_LS_IMM;
    }

    return process_load_store_instr(instr, &instr_s);
}


//=========================================
//=== Load/Store word and unsigned byte ===
//=========================================
// syntax: STR{<c>} <Rt>, [<Rn>, #+/-<imm12>]!
// syntax: STR<c> <Rt>, [<Rn>,+/-<Rm>{, <shift>}]{!}
int STR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STR";

    instr_s.igroup = GROUP_LD_STR;
    // check bit 25
    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM_STR;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}

// syntax: STRT<c> <Rt>, [<Rn>] {, +/-<imm12>}
// syntax: STRT<c> <Rt>, [<Rn>],+/-<Rm>{, <shift>}
int STRT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRT";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM_STR;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}

// syntax: LDR<c> <Rt>, [<Rn>, #+/-<imm12>]!
// syntax: LDR<c> <Rt>, [<Rn>,+/-<Rm>{, <shift>}]{!}
int LDR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDR";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}


int LDRT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRT";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}

// syntax: STRB<c> <Rt>, [<Rn>, #+/-<imm12>]!
// syntax: STRB<c> <Rt>, [<Rn>,+/-<Rm>{, <shift>}]{!}
int STRB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRB";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM_STR;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}

int STRBT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRBT";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM_STR;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}

int LDRB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRB";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}

int LDRBT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDRBT";

    instr_s.igroup = GROUP_LD_STR;

    if (((instr >> 25) & 0x1) == 0) {
        instr_s.itype = TYPE_LS_IMM;
    }
    else {
        instr_s.itype = TYPE_LS_REG;
    }

    return process_load_store_instr(instr, &instr_s);
}
