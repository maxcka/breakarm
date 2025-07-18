#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


// ---------------------------------------
// --- Coprocessor and Supervisor Call ---
// ---------------------------------------

void print_coproc_instr(Instr *instr_s) {
    switch (instr_s->itype) {

        case TYPE_COPROC_0:
        {
            printf("%s%s %s\n",
                instr_s->mnemonic,
                cond_codes[instr_s->c],
                instr_s->imm_str);
            break;
        }

        case TYPE_COPROC_1:
        {
            printf("%s%s%s%s p%d, c%d, [%s%s, %s%s%s\n",
                instr_s->mnemonic,
                (instr_s->c == UNCOND) ? "2" : "",
                (instr_s->L) ? "L" : "",
                cond_codes[instr_s->c],

                instr_s->coproc,
                instr_s->CRd,

                core_reg[instr_s->Rn],
                (instr_s->index == FALSE) ? "]" : "", // post-indexed

                instr_s->imm_str,
                (instr_s->index == TRUE) ? "]" : "", // offset or pre-indexed
                (instr_s->index == TRUE && instr_s->wback == TRUE) ? "!" : "");
            break;
        }

        case TYPE_COPROC_2: // syntax: <MNEMONIC>{2}{<c>} <coproc>, {#}<opc1>, <Rt>, <Rt2>, <CRm>
        {
            printf("%s%s%s p%d, #%d, %s, %s, c%d\n",
                instr_s->mnemonic,
                (instr_s->c == UNCOND) ? "2" : "",
                cond_codes[instr_s->c],

                instr_s->coproc,
                instr_s->opc1,
                core_reg[instr_s->Rt],
                core_reg[instr_s->Rt2],
                instr_s->CRm);
            break;
        }

        case TYPE_COPROC_3_OPC2: // syntax: <MNEMONIC>{2}<c> <coproc>, <opc1>, <CRd>, <CRn>, <CRm>, <opc2>
        {
            printf("%s%s%s p%d, #%d, c%d, c%d, c%d, #%d\n",
                instr_s->mnemonic,
                (instr_s->c == UNCOND) ? "2" : "",
                cond_codes[instr_s->c],

                instr_s->coproc,
                instr_s->opc1,
                instr_s->CRd,
                instr_s->CRn,
                instr_s->CRm,
                instr_s->opc2);
            break;
        }

        case TYPE_COPROC_4_OPC2: // syntax: <MNEMONIC>{2}<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}
        {
            printf("%s%s%s p%d, #%d, %s, c%d, c%d, #%d\n",
                instr_s->mnemonic,
                (instr_s->c == UNCOND) ? "2" : "",
                cond_codes[instr_s->c],

                instr_s->coproc,
                instr_s->opc1,
                core_reg[instr_s->Rt],
                instr_s->CRn,
                instr_s->CRm,
                instr_s->opc2);
            break;
        }

        case TYPE_UNPRED:
        {
            instr_s->mnemonic = UNPRED_STR;
            printf("%s\n", instr_s->mnemonic);
            break;
        }

        case TYPE_UNDEF:
        {
            instr_s->mnemonic = UNDEF_STR;
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

int process_coproc_instr(uint32_t instr, Instr *instr_s) {
    instr_s->c = (instr >> 28) & 0xF;

    instr_s->index = (instr >> 24) & 0x1;
    instr_s->add = (instr >> 23) & 0x1;
    instr_s->L = (instr >> 22) & 0x1; // technically D
    instr_s->wback = (instr >> 21) & 0x1;
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->CRd = (instr >> 12) & 0xF;
    instr_s->coproc = (instr >> 8) & 0xF;
    uint8_t imm8 = (instr >> 0) & 0xFF;

    instr_s->CRm = (instr >> 0) & 0xF;
    instr_s->CRn = (instr >> 16) & 0xF;

    instr_s->opc1 = (instr >> 20) & 0xF;
    instr_s->opc2 = (instr >> 5) & 0x7;

    instr_s->Rt = (instr >> 12) & 0xF;
    instr_s->Rt2 = (instr >> 16) & 0xF;
    

    uint32_t imm24 = (instr >> 0) & 0xFFFFFF;

    if (IS_ITYPE(instr_s->itype, TYPE_COPROC_0)) {
        get_imm_str(instr_s, imm24, 0, 0, TRUE);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_COPROC_1)) {
        if (instr_s->index == FALSE && instr_s->wback == FALSE) {
            get_option_str(instr_s, imm8);
        }
        else {
            get_imm_str(instr_s, imm8 << 2, 0, 0, instr_s->add); // << 2 is to convert from word to bytes
        }
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_COPROC_2)) {
        instr_s->opc1 = (instr >> 4) & 0xF;
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_COPROC_4_OPC2)) {
        instr_s->opc1 >>= 1;
    }

    // unpred/undef handling
    if (IS_ITYPE(instr_s->itype, TYPE_COPROC_1) &&
        instr_s->Rn == PC && instr_s->wback) {

        instr_s->itype = TYPE_UNPRED;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_COPROC_1, TYPE_COPROC_2, TYPE_COPROC_4_OPC2) &&
        ((!instr_s->index && !instr_s->add && !instr_s->L && !instr_s->wback) ||
        (instr_s->coproc & 0xE) == 0xA)) {

        instr_s->itype = TYPE_UNDEF;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_COPROC_2) &&
        (IS_TARGET_REG(PC, instr_s->Rt, instr_s->Rt2) || (instr_s->Rt == instr_s->Rt2))) {
        
        instr_s->itype = TYPE_UNPRED;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_COPROC_4_OPC2) && 
        IS_TARGET_REG(PC, instr_s->Rt)) {

        instr_s->itype = TYPE_UNPRED;
    }
    

    print_asm_instr(instr_s);
    return 0;
}





// syntax: SVC<c> #<imm24>
int SVC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_0;

    instr_s.mnemonic = "SVC";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: STC{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
int STC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_1;

    instr_s.mnemonic = "STC";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: immediate: LDC{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
int LDC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_1;

    instr_s.mnemonic = "LDC";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: MCRR<c> <coproc>, <opc1>, <Rt>, <Rt2>, <CRm>
int MCRR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_2;

    instr_s.mnemonic = "MCRR";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: MRRC<c> <coproc>, <opc1>, <Rt>, <Rt2>, <CRm>
int MRRC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_2;

    instr_s.mnemonic = "MRRC";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: CDP<c> <coproc>, <opc1>, <CRd>, <CRn>, <CRm>, <opc2>
int CDP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_3_OPC2;

    instr_s.mnemonic = "CDP";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: MCR<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}
int MCR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_4_OPC2;

    instr_s.mnemonic = "MCR";

    return process_coproc_instr(instr, &instr_s);

}

// syntax: MRC<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}
int MRC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.igroup = GROUP_COPROC;
    instr_s.itype = TYPE_COPROC_4_OPC2;

    instr_s.mnemonic = "MRC";

    return process_coproc_instr(instr, &instr_s);

}

//> UNCONDITIONAL instructions

// syntax: SRS{<amode>} SP{!}, #<mode>
// syntax: RFE{<amode>} <Rn>{!}
// syntax: BLX <label> (put in instr_branch_block.c)
// syntax: STC2{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
// syntax: LDC2{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
// syntax: MCRR2<c> <coproc>, <opc1>, <Rt>, <Rt2>, <CRm>
// syntax: MRRC2<c> <coproc>, <opc1>, <Rt>, <Rt2>, <CRm>
// syntax: CDP2<c> <coproc>, <opc1>, <CRd>, <CRn>, <CRm>, <opc2>
// syntax: MCR2<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}
// syntax: MRC2<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}

