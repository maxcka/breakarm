/*
 * breakarm – A32 Instruction Disassembler
 * Copyright (c) 2025 Maximilian Kallas
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */

/*
 * Author: Maximilian Kallas
 * File: instr_default.c
 * Description: 
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


// ====================
// === INSTRUCTIONS ===
// ====================

// undefined, unpredictable, not implemented, and default instructions are general instructions that are used
// by several instruction groups (igroups).
void print_default_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_UNDEF:
        case TYPE_UNPRED:
        case TYPE_NOT_IMP:
        {
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


int UNDEF_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_DEFAULT;
    instr_s.itype = TYPE_UNDEF;
    instr_s.mnemonic = UNDEF_STR;

    (void)instr; // silence warning

    print_asm_instr(&instr_s);
    return 0;
}

int UNPRED_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_DEFAULT;
    instr_s.itype = TYPE_UNPRED;
    instr_s.mnemonic = UNPRED_STR;

    (void)instr; // silence warning

    print_asm_instr(&instr_s);
    return 0;
}

int NOT_IMP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_DEFAULT;
    instr_s.itype = TYPE_NOT_IMP;
    instr_s.mnemonic = NOT_IMP_STR;

    (void)instr; // silence warning

    print_asm_instr(&instr_s);
    return 0;
}