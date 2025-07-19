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


// ---------------------------------------------------------
// --- Branch, branch with link, and block data transfer ---
// ---------------------------------------------------------

void print_branch_block_instr(Instr *instr_s) {
    switch (instr_s->itype) {

        case TYPE_BR_BLK_0:
        case TYPE_BR_BLK_0_LDM:
        {
            printf("%s%s %s%s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rn],
                (instr_s->wback) ? "!" : "",
                instr_s->reg_list_str);
            print_unpred(instr_s);
			break;
        }

        case TYPE_BR_BLK_1:
        {
            printf("%s%s %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                instr_s->reg_list_str);
            print_unpred(instr_s);
			break;
        }

        case TYPE_BR_BLK_2:
        case TYPE_BR_BLK_2_LDM:
        {
            printf("%s%s%s %s%s, %s^",
                instr_s->mnemonic,
                amode_table[instr_s->P][instr_s->U],
                cond_codes[instr_s->c],
                core_reg[instr_s->Rn],
                (instr_s->wback) ? "!" : "",
                instr_s->reg_list_str);
            print_unpred(instr_s);
			break;
        }

        case TYPE_BR_BLK_3:
        {
            printf("%s%s 0x%x",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                instr_s->label);
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

int process_branch_block_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    uint16_t registers = (instr >> 0) & 0xFFFF;
    instr_s->wback = (instr >> 21) & 0x1;
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rt = (instr >> 12) & 0xF;
    instr_s->P = (instr >> 24) & 0x1;
    instr_s->U = (instr >> 23) & 0x1;
    uint32_t imm24 = (instr >> 0) & 0xFFFFFF;
    uint8_t H = (instr >> 24) & 0x1;

    if (instr_s->c == UNCOND) {
        instr_s->label = get_label((imm24 << 2) | (H << 1), 26);
    }
    else {
        instr_s->label = get_label(imm24 << 2, 26);
    }

    if (IS_NOT_ITYPE(instr_s->itype, TYPE_BR_BLK_3)) {
        get_reg_list(instr_s, registers);
    }

    // check for unpred
    if (IS_ITYPE(instr_s->itype, TYPE_BR_BLK_1) && IS_TARGET_REG(SP, instr_s->Rt)) {
        instr_s->is_unpred = TRUE;
    }
    if (IS_NOT_ITYPE(instr_s->itype, TYPE_BR_BLK_1, TYPE_BR_BLK_3) &&
        (registers == 0 || instr_s->Rn == PC)) {
        instr_s->is_unpred = TRUE;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_BR_BLK_0_LDM, TYPE_BR_BLK_2_LDM) &&
        (instr_s->wback == 1 && (registers >> instr_s->Rn) == 1)) {
        instr_s->is_unpred = TRUE;
    }


    print_asm_instr(instr_s);
    return 0;
}


// syntax: STMDA<c> <Rn>{!}, <registers>
// syntax: STMDB<c> <Rn>{!}, <registers>
int STMDX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_0;

    if (((instr >> 24) & 0x1) == 0) {
        instr_s.mnemonic = "STMDA";
    }
    else {
        instr_s.mnemonic = "STMDB";
    }

    return process_branch_block_instr(instr, &instr_s);
}


// syntax: LDMDA<c> <Rn>{!}, <registers>
// syntax: LDMDB<c> <Rn>{!}, <registers>
int LDMDX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_0_LDM;

    if (((instr >> 24) & 0x1) == 0) {
        instr_s.mnemonic = "LDMDA";
    }
    else {
        instr_s.mnemonic = "LDMDB";
    }

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: STM<c> <Rn>{!}, <registers>
// syntax: STMIB<c> <Rn>{!}, <registers>
int STMXX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_0;

    if (((instr >> 24) & 0x1) == 0) {
        instr_s.mnemonic = "STM";
    }
    else {
        instr_s.mnemonic = "STMIB";
    }

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: LDM<c> <Rn>{!}, <registers>
// syntax: LDMIB<c> <Rn>{!}, <registers>
int LDMXX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_0_LDM;

    if (((instr >> 24) & 0x1) == 0) {
        instr_s.mnemonic = "LDM";
    }
    else {
        instr_s.mnemonic = "LDMIB";
    }

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: POP<c> <registers>
int POP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_1;
    instr_s.mnemonic = "POP";

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: PUSH<c> <registers>
int PUSH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_1;
    instr_s.mnemonic = "PUSH";

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: STM{<amode>}<c> <Rn>, <registers>^
int STM_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_2;
    instr_s.mnemonic = "STM";

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: LDM{<amode>}<c> <Rn>, <registers_without_pc>^
// syntax: LDM{<amode>}<c> <Rn>{!}, <registers_with_pc>^
int LDM_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_2_LDM;
    instr_s.mnemonic = "LDM";

    return process_branch_block_instr(instr, &instr_s);
}


// syntax: B<c> <label>
int B_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_3;
    instr_s.mnemonic = "B";

    return process_branch_block_instr(instr, &instr_s);
}

// syntax: BL<c> <label>
int BL_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_BRANCH_BLK;
    instr_s.itype = TYPE_BR_BLK_3;

    if (IS_UNCOND(instr)) {
        instr_s.mnemonic = "BLX";
    }
    else {
        instr_s.mnemonic = "BL";
    }

    return process_branch_block_instr(instr, &instr_s);
}