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

// -----------------------
// --- Synchronization ---
// -----------------------

void print_sync_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_SYNC_0:
        {
            printf("%s%s%s %s, %s, %s\n",
                instr_s->mnemonic,
                (instr_s->B) ? "B" : "",
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rt2],
                core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_SYNC_1:
        {
            printf("%s%s %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_SYNC_2:
        {
            printf("%s%s %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_SYNC_3:
        {
            printf("%s%s %s, %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rd],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rt2],
                core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_SYNC_4:
        {
            printf("%s%s %s, %s, %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rt2],
                core_reg[instr_s->Rn]);
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

// main processing function
int process_sync_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->B = (instr >> 22) & 0x1;

    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rd = UNINIT;
    instr_s->Rt = UNINIT;
    instr_s->Rt2 = UNINIT;
    

    if (instr_s->itype == TYPE_SYNC_0) {
        instr_s->Rt = (instr >> 12) & 0xF;
        instr_s->Rt2 = (instr >> 0) & 0xF;
    }
    else if (instr_s->itype == TYPE_SYNC_1 || instr_s->itype == TYPE_SYNC_3) {
        instr_s->Rd = (instr >> 12) & 0xF;
        instr_s->Rt = (instr >> 0) & 0xF;
    }
    else if (instr_s->itype == TYPE_SYNC_2 || instr_s->itype == TYPE_SYNC_4) {
        instr_s->Rt = (instr >> 12) & 0xF;
    }

    if (instr_s->Rn == PC || instr_s->Rt == PC || instr_s->Rt2 == PC ||
        instr_s->Rn == instr_s->Rt || instr_s->Rn == instr_s->Rt2) {
        instr_s->itype = TYPE_UNPRED;
    }

    print_asm_instr(instr_s);

    return 0;
}

// process instruction based on mnemonic
// syntax: SWP{B}<c> <Rt>, <Rt2>, [<Rn>]
int SWP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SWP";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_0;

    return process_mult_instr(instr, &instr_s);
}

// syntax: STREX<c> <Rd>, <Rt>, [<Rn>]
int STREX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STREX";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_1;

    return process_mult_instr(instr, &instr_s);
}

// syntax: LDREX<c> <Rt>, [<Rn>]
int LDREX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDREX";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_2;

    return process_mult_instr(instr, &instr_s);
}

// syntax: STREXD<c> <Rd>, <Rt>, <Rt2>, [<Rn>]
int STREXD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STREXD";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_3;

    return process_mult_instr(instr, &instr_s);
}

// syntax: LDREXD<c> <Rt>, <Rt2>, [<Rn>]
int LDREXD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDREXD";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_4;

    return process_mult_instr(instr, &instr_s);
}

// syntax: STREXB<c> <Rd>, <Rt>, [<Rn>]
int STREXB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STREXB";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_1;

    return process_mult_instr(instr, &instr_s);
}

// syntax: LDREXB<c> <Rt>, [<Rn>]
int LDREXB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDREXB";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_2;

    return process_mult_instr(instr, &instr_s);
}

// syntax: STREXH<c> <Rd>, <Rt>, [<Rn>]
int STREXH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STREXH";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_1;

    return process_mult_instr(instr, &instr_s);
}

// syntax: LDREXH<c> <Rt>, [<Rn>]
int LDREXH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LDREXH";

    instr_s.igroup = GROUP_SYNC;
    instr_s.itype = TYPE_SYNC_2;

    return process_mult_instr(instr, &instr_s);
}





