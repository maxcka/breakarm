#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"

// ------------------
// --- Load/Store ---
// ------------------

void print_load_store_instr(Instr *instr_s) {
    switch (instr_s->itype) {

        case TYPE_LS_0:
        {
            printf("%s%s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rn],
                (instr_s->add == 1) ? "" : "-",
                core_reg[instr_s->Rm],
                (instr_s->index == 1 && instr_s->wback == 1) ? "!" : "");
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
int process_load_store_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;
    instr_s->Rt = (instr >> 12) & 0xF;
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;
    uint8_t P = (instr >> 24) & 0x1;
    uint8_t U = (instr >> 23) & 0x1;
    uint8_t W = (instr >> 21) & 0x1;
    instr_s->index = P;
    instr_s->add = U;
    instr_s->wback = (P == 0) || (W == 1);

    if (instr_s->Rt == PC || instr_s->Rm == PC ||
        (instr_s->wback && (instr_s->Rn == PC || instr_s->Rn == instr_s->Rt))) {
        instr_s->itype = TYPE_UNPRED;
    }

    return 0;

}

// process instruction based on mnemonic

//=== Extra Load/Store ===

// syntax: STRH<c> <Rt>, [<Rn>,+/-<Rm>]{!}
int STRH_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "STRH";

    instr_s.igroup = GROUP_LD_STR;
    instr_s.itype = TYPE_LS_0;

    return process_load_store_instr(instr, &instr_s);
}