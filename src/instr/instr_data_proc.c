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


static void print_dp_2_syntactic_sugar(Instr *instr_s);
static void check_sus_instr(Instr *instr_s);

// -----------------------
// --- Data-processing ---
// -----------------------

// print
void print_data_proc_instr(Instr *instr_s) {
    switch (instr_s->itype) {
        case TYPE_DP_0: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
        {
            printf("%s%s%s %s, %s, %s%s", 
            instr_s->mnemonic, 
            (instr_s->S) ? "S" : "", 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);
            check_sus_instr(instr_s);
            print_unpred_or_newline(instr_s);
			break;
        }
        
        case TYPE_DP_0_RSR: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>
        {
            printf("%s%s%s %s, %s, %s, %s %s", 
            instr_s->mnemonic, 
            (instr_s->S) ? "S" : "", 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            shift_codes[instr_s->shift.shift_t],
            core_reg[instr_s->Rs]);
            check_sus_instr(instr_s);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_0_IMM: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, #<const>
        {
            printf("%s%s%s %s, %s, %s",
                instr_s->mnemonic, 
                (instr_s->S) ? "S" : "",
                cond_codes[instr_s->c], 
                core_reg[instr_s->Rd], 
                core_reg[instr_s->Rn],
                instr_s->imm_str);
            print_unpred_or_newline(instr_s);
			break;
        }
        
        case TYPE_DP_1: // syntax: <MNEMONIC><c> <Rn>, <Rm>{, <shift>}
        {
            printf("%s%s %s, %s%s",
            instr_s->mnemonic, 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            instr_s->shift_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_1_RSR: // syntax: <MNEMONIC><c> <Rn>, <Rm>, <type> <Rs>
        {
            printf("%s%s %s, %s, %s %s",
            instr_s->mnemonic, 
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rn], 
            core_reg[instr_s->Rm], 
            shift_codes[instr_s->shift.shift_t],
            core_reg[instr_s->Rs]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_1_IMM: // syntax: <MNEMONIC><c> <Rn>, #<const>
        {
            printf("%s%s %s, %s",
                instr_s->mnemonic, 
                cond_codes[instr_s->c], 
                core_reg[instr_s->Rn],
                instr_s->imm_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_2: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>
        {
            print_dp_2_syntactic_sugar(instr_s);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_2_IMM: // syntax: <MNEMONIC>{S}<c> <Rd>, #<const>
        {
            printf("%s%s%s %s, %s",
                instr_s->mnemonic,
                (instr_s->S) ? "S" : "",
                cond_codes[instr_s->c], 
                core_reg[instr_s->Rd],
                instr_s->imm_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_2_IMM16: // syntax: <MNEMONIC><c> <Rd>, #<imm16>
        {
            printf("%s%s %s, %s",
                instr_s->mnemonic,
                cond_codes[instr_s->c], 
                core_reg[instr_s->Rd],
                instr_s->imm_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_3: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, #<imm5>
        {
            printf("%s%s%s %s, %s, #%d",
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm],
            instr_s->shift.shift_n);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_3_RSR: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rn>, <Rm>
        {
            printf("%s%s%s %s, %s, %s",
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd],
            core_reg[instr_s->Rn],
            core_reg[instr_s->Rm]);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_4: // syntax: <MNEMONIC>S{<c>} <Rd>, <Rm>{, <shift>}
        {
            printf("%s%s%s %s, %s%s",
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm],
            instr_s->shift_str);
            print_unpred_or_newline(instr_s);
			break;
        }

        case TYPE_DP_4_RSR: // syntax: <MNEMONIC>{S}<c> <Rd>, <Rm>, <type> <Rs>
        {
            printf("%s%s%s %s, %s, %s %s",
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm],
            shift_codes[instr_s->shift.shift_t],
            core_reg[instr_s->Rs]);
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
// syntactic sugar for nop
static void print_dp_2_syntactic_sugar(Instr *instr_s) {
    char buf[BUF_40];

    sprintf(buf, "%s%s%s %s, %s", 
            instr_s->mnemonic,
            (instr_s->S) ? "S" : "",
            cond_codes[instr_s->c], 
            core_reg[instr_s->Rd], 
            core_reg[instr_s->Rm]);

    int used_sugar = 0;
    if (strncmp(buf, "MOV R0, R0", 10) == 0) {
        printf("NOP\t\t@ (");
        used_sugar = 1;
    }

    printf("%s%s", buf, (used_sugar) ? ")" : "");
}

static void check_sus_instr(Instr *instr_s) {
    // ANDEQ is likely data/garbage not code
    if (IS_ITYPE(instr_s->itype, TYPE_DP_0, TYPE_DP_0_RSR) &&
        strcmp(instr_s->mnemonic, "AND") == 0 &&
        instr_s->c == EQ) {
        
        printf("\t; [!] suspicious encoding - possibly junk");
    }
}

// =======================================


// main processing function
int process_data_proc_instr(uint32_t instr, Instr *instr_s) {

    // maybe add these vars into decode_imm_shift() depending on how repetitive
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm4 = (instr >> 16) & 0xF; // used for movw and movt
    uint8_t imm5 = (instr >> 7) & 0x1F;
    uint16_t imm12 = (instr >> 0) & 0xFFF; // used for immediate instr
    instr_s->c =  (instr >> 28) & 0xF; // c is condition
    instr_s->Rd = (instr >> 12) & 0xF; // 0b1111
    instr_s->Rn = (instr >> 16) & 0xF;
    instr_s->Rm = (instr >> 0) & 0xF;
    instr_s->S =  (instr >> 20) & 0x1;
    // Rs used for register-shifted register instructions
    instr_s->Rs = (instr >> 8) & 0xF;


    // special case Encoding A2: <opc2>S{<c>}{<q>} <Rd>, <Rm> {, <shift>}
    if ((instr_s->itype == TYPE_DP_0 || instr_s->itype == TYPE_DP_2 || instr_s->itype == TYPE_DP_3) 
        && instr_s->S == 0x1 && instr_s->Rd == PC) {
        instr_s->itype = TYPE_DP_4;
        instr_s->mnemonic = "MVN";
    }

    if (instr_s->igroup == GROUP_DP_IMM) {
        get_rotated_imm_str(instr_s, imm12);
    }
    else if (instr_s->igroup == GROUP_DP_IMM16) {
        if (instr_s->Rd == PC) {
            instr_s->is_unpred = TRUE;
        }
        get_imm_str(instr_s, imm4, imm12, 12, TRUE);
    }
    else {
        get_shift_str(instr_s, type, imm5);
    }
    print_asm_instr(instr_s);

    return 0;

}

// process instruction based on mnemonic

// process AND (register) instruction
// syntax: AND{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
// for A32, <q> has no effect
int AND_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "AND";
    // this is kind of redundant but I think it's better than creating multiple functions for AND
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {
        instr_s.igroup = GROUP_DP_RSR;
        instr_s.itype = TYPE_DP_0_RSR; // syntax: AND{S}<c> <Rd>, <Rn>, <Rm>, <type> <Rs>
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s); // return dummy value because functions need to have same return type to be stored in the same fn ptr array
}

// process EOR (register) instruction
// syntax: EOR{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int EOR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "EOR";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {
        instr_s.igroup = GROUP_DP_RSR;   
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process SUB (register) instruction
// syntax: SUB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int SUB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SUB";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process RSB (register) instruction
// syntax: RSB{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int RSB_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSB";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process ADD (register, ARM) instruction
// syntax: ADD{S}{<c>}{<q>} {<Rd,} <Rn>, <Rm> {, <shift>}
int ADD_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADD";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process ADC (register) instruction
// syntax: ADC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int ADC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ADC";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process SBC (register) instruction
// syntax: SBC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int SBC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "SBC";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process RSC (register) instruction
// syntax: RSC{S}{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, <shift>}
int RSC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RSC";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}

// process TST (register) instruction
// syntax: TST<c> <Rn>, <Rm>{, <shift>}
int TST_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TST";
    
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_1;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_1_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_1_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process TEQ (register) instruction
int TEQ_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "TEQ";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_1;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_1_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_1_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process CMP (register) instruction
int CMP_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMP";
    
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_1;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_1_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_1_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process CMN (register) instruction
int CMN_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "CMN";
    
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_1;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_1_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_1_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process ORR (register) instruction
int ORR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ORR";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process MOV (register) instruction
// syntax: MOV{S}<c> <Rd>, <Rm>
int MOV_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MOV";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_2;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_2_IMM;
    }
    return process_data_proc_instr(instr, &instr_s);
}
// process LSL (immediate) instruction
// syntax: LSL{S}<c> <Rd>, <Rm>, #<imm5>
int LSL_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LSL";
    
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_3;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process LSR (immediate) instruction
int LSR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "LSR";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_3;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process ASR (immediate) instruction
int ASR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ASR";
    
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_3;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process RRX instruction
// syntax: RRX{S}<c> <Rd>, <Rm>
int RRX_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "RRX";
    instr_s.igroup = GROUP_DP_REG;
    instr_s.itype = TYPE_DP_2;
    return process_data_proc_instr(instr, &instr_s);
}
// process ROR (immediate) instruction
// syntax: ROR{S}<c> <Rd>, <Rm>, #<imm>
int ROR_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "ROR";
    
    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_3;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_3_RSR;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process BIC (register) instruction
// syntax: BIC{S}<c> <Rd>, <Rn>, <Rm>{, <shift>}
int BIC_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "BIC";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_0;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {
        instr_s.igroup = GROUP_DP_RSR;
        instr_s.itype = TYPE_DP_0_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_0_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}
// process MVN (register) instruction
// syntax: MVN{S}<c> <Rd>, <Rm>{, <shift>}
int MVN_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MVN";

    if (IS_DP_OP_0(instr) && IS_DP_REG(instr)) {
        instr_s.igroup = GROUP_DP_REG;
        instr_s.itype = TYPE_DP_4;
    }
    else if (IS_DP_OP_0(instr) && IS_DP_RSR(instr)) {   
        instr_s.igroup = GROUP_DP_RSR; 
        instr_s.itype = TYPE_DP_4_RSR;
    }
    else if (IS_DP_OP_1(instr) && IS_DP_IMM(instr)) {
        instr_s.igroup = GROUP_DP_IMM;
        instr_s.itype = TYPE_DP_2_IMM;
    }

    return process_data_proc_instr(instr, &instr_s);
}


// 16-bit mov and movt
// syntax: MOVW<c> <Rd>, #<imm16>
int MOVW_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MOVW";

    instr_s.igroup = GROUP_DP_IMM16;
    instr_s.itype = TYPE_DP_2_IMM16;

    return process_data_proc_instr(instr, &instr_s);
}

// syntax: MOVT<c> <Rd>, #<imm16>
int MOVT_instr(uint32_t instr) {
    Instr instr_s = {0};
    instr_s.mnemonic = "MOVT";

    instr_s.igroup = GROUP_DP_IMM16;
    instr_s.itype = TYPE_DP_2_IMM16;

    return process_data_proc_instr(instr, &instr_s);
}