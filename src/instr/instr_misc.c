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


void print_misc_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_MISC_BANKED_0: // syntax: <MNEMONIC><c> <Rd>, <banked_reg>
        {
            printf("%s%s %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            instr_s->banked_reg_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_BANKED_1: // syntax: <MNEMONIC><c> <banked_reg>, <Rn>
        {
            printf("%s%s %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->banked_reg_str,
            core_reg[instr_s->Rn]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_1: // syntax: <MNEMONIC><c> <Rd>, <spec_reg>
        {
            printf("%s%s %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            spec_reg[instr_s->R]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_2_APP: // syntax: <MNEMONIC><c> <spec_reg>, <Rn>
        case TYPE_MISC_2_SYS:
        {
            printf("%s%s %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->spec_reg_str,
            core_reg[instr_s->Rn]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_2_IMM_APP: // syntax: <MNEMONIC><c> <spec_reg>, #<imm12>
        case TYPE_MISC_2_IMM_SYS:
        {
            printf("%s%s %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->spec_reg_str,
            instr_s->imm_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_3:
        case TYPE_MISC_3_1:
        {
            printf("%s%s %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_4:
        {
            printf("%s%s %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_5:
        {
            printf("%s%s %s, %s, %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            core_reg[instr_s->Rd],
            core_reg[instr_s->Rm],
            core_reg[instr_s->Rn]);
            print_unpred_or_newline(instr_s);
			break;
        }
        
        case TYPE_MISC_HINT_0: // syntax: <MNEMONIC><c>
        case TYPE_MISC_6:
        {
            printf("%s%s",
            instr_s->mnemonic,
            cond_codes[instr_s->c]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_7: // syntax: <MNEMONIC> #<imm16>
        {
            printf("%s %s",
            instr_s->mnemonic,
            instr_s->imm_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_MISC_HINT_1:
        case TYPE_MISC_8: // syntax: <MNEMONIC><c> #<imm4>
        {
            printf("%s%s %s",
            instr_s->mnemonic,
            cond_codes[instr_s->c],
            instr_s->imm_str);
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


int process_misc_instr(uint32_t instr, Instr *instr_s) {
    uint8_t m = (instr >> 8) & 0x1;
    uint8_t m1 = (instr >> 16) & 0xF;
    instr_s->R = (instr >> 22) & 0x1;
    uint8_t mask;
    uint8_t imm4 = (instr >> 0) & 0xF;
    uint16_t imm12 = (instr >> 8) & 0xFFF;

    if (instr_s->itype == TYPE_MISC_7) {
        get_imm_str(instr_s, imm12, imm4, 4, TRUE);
    }
    if (IS_ITYPE(instr_s->itype, TYPE_MISC_8, TYPE_MISC_HINT_1)) {
        get_imm_str(instr_s, imm4, 0, 0, TRUE);
    }
    if (instr_s->itype == TYPE_MISC_2_IMM_APP || instr_s->itype == TYPE_MISC_2_IMM_SYS) {
        imm12 = (instr >> 0) & 0xFFF;
        get_rotated_imm_str(instr_s, imm12);
    }

    instr_s->c =  (instr >> 28) & 0xF; // c is condition
    instr_s->Rd = (instr >> 12) & 0xF; // 0b1111
    instr_s->Rn = (instr >> 0) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;

    if (instr_s->itype == TYPE_MISC_5) {
        instr_s->Rn = (instr >> 16) & 0xF;
    }
    

    if (instr_s->itype == TYPE_MISC_BANKED_0 || instr_s->itype == TYPE_MISC_BANKED_1) {
        get_banked_reg_str(m, m1, instr_s->R, instr_s->banked_reg_str, sizeof(instr_s->banked_reg_str));
    }
    else if (instr_s->itype == TYPE_MISC_2_APP || instr_s->itype == TYPE_MISC_2_IMM_APP) { // APSR
        
        mask = (instr >> 18) & 0x3;
        get_app_sr_str(instr_s, mask);
    }
    else if (instr_s->itype == TYPE_MISC_2_SYS || instr_s->itype == TYPE_MISC_2_IMM_SYS) { // SPSR or CPSR
        mask = (instr >> 16) & 0xF;
        get_sys_sr_str(instr_s, mask);
    }
    
    if (instr_s->itype == TYPE_MISC_3_1 && instr_s->Rm == PC) {
        instr_s->is_unpred = TRUE;
    }
    else if (instr_s->itype == TYPE_MISC_4 && (instr_s->Rd == PC || instr_s->Rm == PC)) {
        instr_s->is_unpred = TRUE;
    }
    else if (instr_s->itype == TYPE_MISC_5 && (instr_s->Rd == PC || instr_s->Rn == PC || instr_s->Rm == PC)) {
        instr_s->is_unpred = TRUE;
    }
    else if (instr_s->itype == TYPE_MISC_7 && instr_s->c != AL) {
        instr_s->is_unpred = TRUE;
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_MISC_2_APP, TYPE_MISC_2_SYS) && (instr_s->Rn == PC || mask == 0)) {
        instr_s->is_unpred = TRUE;
    }
    else if (IS_ITYPE(instr_s->itype, TYPE_MISC_2_IMM_APP, TYPE_MISC_2_IMM_SYS) && (mask == 0)) {
        instr_s->is_unpred = TRUE;
    }


    
    print_asm_instr(instr_s);

    return 0;
}


// MRS{<c>}{<q>} <Rd>, <banked_reg>
// SYSm = m:m1  ":" is concatenation
int MRS_BANKED_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MRS";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_BANKED_0;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <banked_reg>, <Rn>
int MSR_BANKED_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MSR";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_BANKED_1;

    return process_misc_instr(instr, &instr_s);
}

// MRS<c> <Rd>, <spec_reg>
int MRS_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MRS";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_1;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <spec_reg>, <Rn>
int MSR_reg_app_instr(uint32_t instr) {
    Instr instr_s= {0};
    instr_s.mnemonic = "MSR";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_2_APP;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <spec_reg>, <Rn>
int MSR_reg_sys_instr(uint32_t instr) {
    Instr instr_s= {0};
    instr_s.mnemonic = "MSR";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_2_SYS;

    return process_misc_instr(instr, &instr_s);
}

// BX<c> <Rm>
int BX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BX";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_3;

    return process_misc_instr(instr, &instr_s);
}

// CLZ<c> <Rd>, <Rm>
int CLZ_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CLZ";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_4;

    return process_misc_instr(instr, &instr_s);
}

// BXJ<c> <Rm>
int BXJ_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BXJ";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_3_1;

    return process_misc_instr(instr, &instr_s);
}

// BLX<c> <Rm>
int BLX_reg_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BLX";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_3_1;

    return process_misc_instr(instr, &instr_s);
}

// QADD<c> <Rd>, <Rm>, <Rn>
int QADD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QADD";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// QSUB<c> <Rd>, <Rm>, <Rn>
int QSUB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QSUB";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// QDADD<c> <Rd>, <Rm>, <Rn>
int QDADD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QDADD";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// QDSUB<c> <Rd>, <Rm>, <Rn>
int QDSUB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "QDSUB";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_5;

    return process_misc_instr(instr, &instr_s);
}

// ERET<c>
int ERET_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ERET";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_6;

    return process_misc_instr(instr, &instr_s);
}

// BKPT #<imm16>
int BKPT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BKPT";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_7;

    return process_misc_instr(instr, &instr_s);
}

// HVC #<imm16>
int HVC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "HVC";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_7;

    return process_misc_instr(instr, &instr_s);
}

// SMC<c> #<imm4>
int SMC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SMC";

    instr_s.igroup = GROUP_MISC; 
    instr_s.itype = TYPE_MISC_8;

    return process_misc_instr(instr, &instr_s);
}



//> MSR immediate and hints

// MSR<c> <spec_reg>, #<imm12>
int MSR_imm_app_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MSR";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_2_IMM_APP;

    return process_misc_instr(instr, &instr_s);
}

// MSR<c> <spec_reg>, #<imm12>
int MSR_imm_sys_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MSR";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_2_IMM_SYS;

    return process_misc_instr(instr, &instr_s);
}

// NOP<c>
int NOP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "NOP";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_HINT_0;

    return process_misc_instr(instr, &instr_s);
}

// YIELD<c>
int YIELD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "YIELD";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_HINT_0;

    return process_misc_instr(instr, &instr_s);
}

// WFE<c>
int WFE_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "WFE";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_HINT_0;

    return process_misc_instr(instr, &instr_s);
}

// WFI<c>
int WFI_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "WFI";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_HINT_0;

    return process_misc_instr(instr, &instr_s);
}

// SEV<c>
int SEV_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SEV";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_HINT_0;

    return process_misc_instr(instr, &instr_s);
}

// DBG<c>
int DBG_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "DBG";

    instr_s.igroup = GROUP_MISC_HINTS;
    instr_s.itype = TYPE_MISC_HINT_1;

    return process_misc_instr(instr, &instr_s);
}













    



