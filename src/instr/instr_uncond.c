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

// handles some unconditional instructions that don't fit into other
// categories

// ---------------------
// --- Unconditional ---
// ---------------------

void print_uncond_instr(Instr *instr_s) {
    switch (instr_s->itype) {

        case TYPE_UNC_MISC_0:
        {
            printf("%s%s%s %s%s%s%s %s\n",
                instr_s->mnemonic,
                (instr_s->imod == 2) ? "IE" : "",
                (instr_s->imod == 3) ? "ID" : "",

                (instr_s->A) ? "a" : "",
                (instr_s->I) ? "i" : "",
                (instr_s->F) ? "f" : "",

                (instr_s->M) ? "," : "",
                (instr_s->M) ? instr_s->imm_str : "");
            break;
        }

        case TYPE_UNC_MISC_1:
        {
            printf("%s %s\n",
                instr_s->mnemonic,
                (instr_s->E) ? "BE" : "LE");

            break;
        }

        case TYPE_UNC_MISC_2_IMM:
        {
            printf("%s%s [%s, %s]\n",
                instr_s->mnemonic,
                (instr_s->W) ? "W" : "",
                core_reg[instr_s->Rn],
                instr_s->imm_str);

            break;
        }

        case TYPE_UNC_MISC_2_REG: // syntax: <MNEMONIC> [<Rn>,+/-<Rm>{, <shift>}]
        {
            printf("%s%s [%s, %s%s%s]\n",
                instr_s->mnemonic,
                (instr_s->W) ? "W" : "",
                core_reg[instr_s->Rn],
                (instr_s->add == 1) ? "" : "-",
                core_reg[instr_s->Rm],
                instr_s->shift_str);
            break;
        }

        case TYPE_UNC_MISC_3:
        {
            printf("%s\n", instr_s->mnemonic);

            break;
        }

        case TYPE_UNC_MISC_4:
        {
            printf("%s %s\n", 
                instr_s->mnemonic, 
                option_table[instr_s->option]);

            break;
        }

        case TYPE_UNC_0: // syntax: <MNEMONIC>{<amode>} SP{!}, #<mode>
        {
            printf("%s%s SP%s, %s\n", 
                instr_s->mnemonic, 
                amode_table[instr_s->P][instr_s->U],
                (instr_s->W) ? "!" : "",
                instr_s->mode_str);

            break;
        }

        case TYPE_UNC_1: // syntax: <MNEMONIC>{<amode>} <Rn>{!}
        {
            printf("%s%s %s%s\n", 
                instr_s->mnemonic, 
                amode_table[instr_s->P][instr_s->U],
                core_reg[instr_s->Rn],
                (instr_s->W) ? "!" : "");

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

int process_uncond_instr(uint32_t instr, Instr *instr_s) {
    instr_s->imod = (instr >> 18) & 0x3;
    instr_s->A = (instr >> 8) & 0x1;
    instr_s->I = (instr >> 7) & 0x1;
    instr_s->F = (instr >> 6) & 0x1;
    uint8_t mode = (instr >> 0) & 0x1F;
    instr_s->M = (instr >> 17) & 0x1;
    instr_s->E = (instr >> 9) & 0x1;

    uint16_t imm12 = (instr >> 0) & 0xFFF;
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->add = (instr >> 23) & 0x1;
    instr_s->option = (instr >> 0) & 0xF;

    instr_s->Rm = (instr >> 0) & 0xF;
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;

    uint8_t R = (instr >> 22) & 0x1;
    if (IS_IGROUP(instr_s->igroup, GROUP_UNCOND_MISC)) {
        instr_s->W = !R; // W is opposite of R
    }
    else {
        instr_s->W = (instr >> 21) & 0x1;
    }
    
    instr_s->P = (instr >> 24) & 0x1;
    instr_s->U = (instr >> 23) & 0x1;
    


    if (IS_ITYPE(instr_s->itype, TYPE_UNC_MISC_0, TYPE_UNC_0)) {
        get_imm_str(instr_s, mode, 0, 0, TRUE);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_UNC_MISC_2_IMM)) {
        get_imm_str(instr_s, imm12, 0, 0, instr_s->add);
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_UNC_MISC_2_REG)) {
        get_shift_str(instr_s, type, imm5);
    }

    if (IS_ITYPE(instr_s->itype, TYPE_UNC_MISC_0) &&
        (
         (mode != 0 && instr_s->M == 0) ||
         (instr_s->imod >> 1 == 1 && ((instr >> 6) & 0x7) == 0) ||
         (instr_s->imod >> 1 == 0 && ((instr >> 6) & 0x7) != 0) ||
         (instr_s->imod == 0 && instr_s->M == 0) ||
         (instr_s->imod == 1)
        )) {

        instr_s->itype = TYPE_UNPRED;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_UNC_MISC_2_REG) &&
        instr_s->Rm == PC) {
        
        instr_s->itype = TYPE_UNPRED;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_UNC_0) && mode == 0x1A) { // 0x1A is hyp mode
        instr_s->itype = TYPE_UNPRED;
    }
    if (IS_ITYPE(instr_s->itype, TYPE_UNC_1) && instr_s->Rn == PC) {
        instr_s->itype = TYPE_UNPRED;
    }

    print_asm_instr(instr_s);

    return 0;
}


// syntax: CPS<effect> <iflags>{, #<mode>}
int CPS_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CPS";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_0;

    return process_misc_instr(instr, &instr_s);
}

// syntax: SETEND <endian_specifier>
int SETEND_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SETEND";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_1;

    return process_misc_instr(instr, &instr_s);
}


// syntax: PLI [<Rn>, #+/-<imm12>]
int PLI_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "PLI";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_2_IMM;

    return process_misc_instr(instr, &instr_s);
}

// syntax: PLD{W} [<Rn>, #+/-<imm12>]
int PLD_imm_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "PLD";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_2_IMM;

    return process_misc_instr(instr, &instr_s);
}

// syntax: CLREX
int CLREX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CLREX";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_3;

    return process_misc_instr(instr, &instr_s);
}


// syntax: DSB <option>
int DSB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "DSB";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_4;

    return process_misc_instr(instr, &instr_s);
}
// syntax: DMB <option>
int DMB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "DMB";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_4;

    return process_misc_instr(instr, &instr_s);
}

// syntax: ISB <option>
int ISB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ISB";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_4;

    return process_misc_instr(instr, &instr_s);
}

// syntax: PLI [<Rn>,+/-<Rm>{, <shift>}]
int PLI_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "PLI";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_2_REG;

    return process_misc_instr(instr, &instr_s);
}

// syntax: PLD{W} [<Rn>,+/-<Rm>{, <shift>}]
int PLD_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "PLD";

    instr_s.igroup = GROUP_UNCOND_MISC;
    instr_s.itype = TYPE_UNC_MISC_2_REG;

    return process_misc_instr(instr, &instr_s);
}

//syntax: SRS{<amode>}{<c>}{<q>} SP{!}, #<mode>
int SRS_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SRS";

    instr_s.igroup = GROUP_UNCOND;
    instr_s.itype = TYPE_UNC_0;

    return process_misc_instr(instr, &instr_s);
}

int RFE_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RFE";

    instr_s.igroup = GROUP_UNCOND;
    instr_s.itype = TYPE_UNC_1;

    return process_misc_instr(instr, &instr_s);
}




