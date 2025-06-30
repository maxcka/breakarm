#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


void print_misc_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_MISC_BANKED_0: // syntax: <MNEMONIC><c> <Rd>, <banked_reg>
        {
            printf("%s%s %s, %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            instr_s->banked_reg_str);
            break;
        }

        case TYPE_MISC_BANKED_1: // syntax: <MNEMONIC><c> <banked_reg>, <Rn>
        {
            printf("%s%s %s, %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->banked_reg_str,
            core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_MISC_1: // syntax: <MNEMONIC><c> <Rd>, <spec_reg>
        {
            printf("%s%s %s, %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            spec_reg[instr_s->R]);
            break;
        }

        case TYPE_MISC_2_APP: // syntax: <MNEMONIC><c> <spec_reg>, <Rn>
        case TYPE_MISC_2_SYS:
        {
            printf("%s%s %s, %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->spec_reg_str,
            core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_MISC_3:
        case TYPE_MISC_3_1:
        {
            printf("%s%s %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rm]);
            break;
        }

        case TYPE_MISC_4:
        {
            printf("%s%s %s, %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            core_reg[instr_s->Rm]);
            break;
        }

        case TYPE_MISC_5:
        {
            printf("%s%s %s, %s, %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            core_reg[instr_s->Rm],
            core_reg[instr_s->Rn]);
            break;
        }

        case TYPE_MISC_6:
        {
            printf("%s%s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c]);
            break;
        }

        case TYPE_MISC_7: // syntax: <MNEMONIC> #<imm16>
        {
            printf("%s %s\n",
            instr_s->mnemonic,
            instr_s->imm_str);
            break;
        }

        case TYPE_MISC_8: // syntax: <MNEMONIC><c> #<imm4>
        {
            printf("%s%s %s\n",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->imm_str);
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


int process_misc_instr(uint32_t instr, Instr *instr_s) {
    uint8_t m = (instr >> 8) & 0x1;
    uint8_t m1 = (instr >> 16) & 0xF;
    instr_s->R = (instr >> 22) & 0x1;
    uint8_t mask;
    uint8_t imm4 = (instr >> 0) & 0xF;
    uint8_t imm12 = (instr >> 8) & 0xFFF;

    if (instr_s->itype == TYPE_MISC_7 || instr_s->itype == TYPE_MISC_8) {
        get_imm_str(instr_s, imm4, imm12, FALSE);
    }

    instr_s->c =  (instr >> 28) & 0xF; // c is condition
    instr_s->Rd = (instr >> 12) & 0xF; // 0b1111
    instr_s->Rn = (instr >> 0) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;

    if (instr_s->itype == TYPE_MISC_5) {
        instr_s->Rn = (instr >> 16) & 0xF;
    }

    // so far, misc instr are not reused, so set the group in here
    instr_s->igroup = GROUP_MISC; 

    if (instr_s->itype == TYPE_MISC_BANKED_0 || instr_s->itype == TYPE_MISC_BANKED_1) {
        get_banked_reg_str(m, m1, instr_s->R, instr_s->banked_reg_str, sizeof(instr_s->banked_reg_str));
    }
    else if (instr_s->itype == TYPE_MISC_2_APP) { // APSR
        mask = (instr >> 18) & 0x3;
        get_app_sr_str(instr_s, mask);
    }
    else if (instr_s->itype == TYPE_MISC_2_SYS) { // SPSR or CPSR
        mask = (instr >> 16) & 0xF;
        get_sys_sr_str(instr_s, mask);
    }
    else if (instr_s->itype == TYPE_MISC_3_1 && instr_s->Rm == PC) {
        instr_s->itype = TYPE_UNPRED;
    }
    else if (instr_s->itype == TYPE_MISC_4 && (instr_s->Rd == PC || instr_s->Rm == PC)) {
        instr_s->itype = TYPE_UNPRED;
    }
    else if (instr_s->itype == TYPE_MISC_5 && (instr_s->Rd == PC || instr_s->Rn == PC || instr_s->Rm == PC)) {
        instr_s->itype = TYPE_UNPRED;
    }
    else if (instr_s->itype == TYPE_MISC_7 && instr_s->c != AL) {
        instr_s->itype = TYPE_UNPRED;
    }
    
    print_asm_instr(instr_s);

    return 0;
}


// MRS{<c>}{<q>} <Rd>, <banked_reg>
// SYSm = m:m1  ":" is concatenation
int MRS_BANKED_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MRS";

    instr_s.itype = TYPE_MISC_BANKED_0;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <banked_reg>, <Rn>
int MSR_BANKED_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MSR";

    instr_s.itype = TYPE_MISC_BANKED_1;

    return process_misc_instr(instr, &instr_s);
}

// MRS<c> <Rd>, <spec_reg>
int MRS_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MRS";

    instr_s.itype = TYPE_MISC_1;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <spec_reg>, <Rn>
int MSR_reg_app_instr(uint32_t instr) {
    Instr instr_s= {0};
    instr_s.mnemonic = "MSR";

    instr_s.itype = TYPE_MISC_2_APP;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <spec_reg>, <Rn>
int MSR_reg_sys_instr(uint32_t instr) {
    Instr instr_s= {0};
    instr_s.mnemonic = "MSR";

    instr_s.itype = TYPE_MISC_2_SYS;

    return process_misc_instr(instr, &instr_s);
}

// BX<c> <Rm>
int BX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BX";

    instr_s.itype = TYPE_MISC_3;

    return process_misc_instr(instr, &instr_s);
}

// CLZ<c> <Rd>, <Rm>
int CLZ_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CLZ";

    instr_s.itype = TYPE_MISC_4;

    return process_misc_instr(instr, &instr_s);
}

// BXJ<c> <Rm>
int BXJ_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BXJ";

    instr_s.itype = TYPE_MISC_3_1;

    return process_misc_instr(instr, &instr_s);
}

// BLX<c> <Rm>
int BLX_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BLX";

    instr_s.itype = TYPE_MISC_3_1;

    return process_misc_instr(instr, &instr_s);
}

// QADD<c> <Rd>, <Rm>, <Rn>
int QADD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QADD";

    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// QSUB<c> <Rd>, <Rm>, <Rn>
int QSUB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QSUB";

    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// QDADD<c> <Rd>, <Rm>, <Rn>
int QDADD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QDADD";

    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// QDSUB<c> <Rd>, <Rm>, <Rn>
int QDSUB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QDSUB";

    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// ERET<c>
int ERET_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ERET";

    instr_s.itype = TYPE_MISC_6;

    return process_misc_instr(instr, &instr_s);
}

// BKPT #<imm16>
int BKPT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BKPT";

    instr_s.itype = TYPE_MISC_7;

    return process_misc_instr(instr, &instr_s);
}

// HVC #<imm16>
int HVC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "HVC";

    instr_s.itype = TYPE_MISC_7;

    return process_misc_instr(instr, &instr_s);
}

// SMC<c> #<imm4>
int SMC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMC";

    instr_s.itype = TYPE_MISC_8;

    return process_misc_instr(instr, &instr_s);
}




    



