/*
 * breakarm – A32 Instruction Disassembler
 * Copyright (c) 2025 Maximilian Kallas
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "decode.h"

// print unpred and newline or just newline
void print_unpred_or_newline(Instr *instr_s) {
    if (instr_s->is_unpred) {
        printf("\t; %s\n", UNPRED_STR);
    }
    else {
        printf("\n");
    }
}

void get_reg_list(Instr *instr_s, uint16_t reg_list_bits) {
    int start = 0;
    int length = sizeof(reg_list_bits)*8;
    int i = 0;
    int interval_count = 0;
    strcat(instr_s->reg_list_str, "{");
    // form intervals
    while (i < length) {
        // bit not set at position i
        if (((reg_list_bits >> i) & 0x1) == 0) {
            i++;
            continue;
        }

        start = i;

        // put comma unless this is the first build
        if (interval_count != 0) {
            strcat(instr_s->reg_list_str, ", ");
        }
        // while consecutive indices have bit set
        while (i < length-1 && ((reg_list_bits >> (i+1)) & 0x1) == 1) {
            i++;
        }
        // if previous condition was true at least once, form interval
        if (start != i) {
            sprintf(instr_s->reg_list_str + strlen(instr_s->reg_list_str),
                "%s-%s", core_reg[start], core_reg[i]);
        }   
        else {  // else just print the reg corresponding to 'start'
            sprintf(instr_s->reg_list_str + strlen(instr_s->reg_list_str),
                "%s", core_reg[start]);
        }

        interval_count++;
        i++;
    }
    strcat(instr_s->reg_list_str, "}");
}

uint32_t get_label(uint32_t imm, uint8_t bitwidth) {
    int32_t offset = sign_extend(imm, bitwidth);
    uint32_t pc = (uint32_t)curr_addr + 8; // apparently actual PC is curr_addr + 8?
    uint32_t label = pc + offset; 
    return label;
}

int32_t sign_extend(uint32_t imm, uint8_t bitwidth) {
    // if msb is set, number is negative
    // using arithmetic shift of signed data type
    int32_t shift = 32 - bitwidth;
    return ((int32_t)imm << shift) >> shift;
}

void get_char_suffix(Instr *instr_s) {
    instr_s->str_suffix[0] = instr_s->char_suffix;
}

uint8_t is_not_itype(uint8_t itype, uint8_t count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        int target = va_arg(args, int);
        if (itype == target) {
            va_end(args);
            return FALSE;
        }
    }
    va_end(args);
    return TRUE;
}

uint8_t is_itype(uint8_t itype, uint8_t count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        int target = va_arg(args, int);
        if (itype == target) {
            va_end(args);
            return TRUE;
        }
    }
    va_end(args);
    return FALSE;
}

uint8_t is_any_reg_target_reg(Register target, uint8_t count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        Register reg = va_arg(args, int);
        if (reg == target) {
            va_end(args);
            return TRUE;
        }
    }
    va_end(args);
    return FALSE;
}

// rotate a 32-bit value to the right by n bits
uint32_t ror(uint32_t value, uint8_t n) {
    n = n % 32;
    return (value >> n) | (value << (32 - n));
}

// uses upper 4 bits of imm12 to rotate the lower imm8 as described in
// A5.2.4 Modified immediate constants in ARM instructions
void get_rotated_imm_str(Instr *instr_s, uint16_t imm12) {
    uint8_t rotate = (imm12 >> 8) & 0xF; // get upper 4 bits
    uint8_t imm8 = imm12 & 0xFF;
    uint32_t rotated_imm = ror(imm8, rotate*2); // need to rotate value by rotate*2 bits
    snprintf(instr_s->imm_str, sizeof(instr_s->imm_str), "#%u", rotated_imm);
}

// takes a high and low value and shifts
void get_imm_str(Instr *instr_s, uint32_t imm_high, uint16_t imm_low, uint8_t shift, uint8_t positive) {
    uint32_t mask = (1 << shift) - 1;
    uint32_t imm = (imm_high << shift) | (imm_low & mask);
    snprintf(instr_s->imm_str, sizeof(instr_s->imm_str), "#%s%d", (positive) ? "" : "-", imm);
}

void get_option_str(Instr *instr_s, uint8_t option) {
    snprintf(instr_s->option_str, sizeof(instr_s->option_str), "{%d}", option);
}

// sys special register string
void get_sys_sr_str(Instr *instr_s, uint8_t mask) {
    uint8_t mask_0 = (mask >> 0) & 0x1;
    uint8_t mask_1 = (mask >> 1) & 0x1;
    uint8_t mask_2 = (mask >> 2) & 0x1;
    uint8_t mask_3 = (mask >> 3) & 0x1;

    char suffix[BUF_10] = "";

    if (mask_0 == 1) {
        strcat(suffix, "c");
    }
    if (mask_1 == 1) {
        strcat(suffix, "x");
    }
    if (mask_2 == 1) {
        strcat(suffix, "s");
    }
    if (mask_3 == 1) {
        strcat(suffix, "f");
    }

    snprintf(instr_s->spec_reg_str, sizeof(instr_s->spec_reg_str), "%s_%s", spec_reg[instr_s->R], suffix);

}

// app special register string
void get_app_sr_str(Instr *instr_s, uint8_t mask) {
    uint8_t mask_0 = (mask >> 0) & 0x1;
    uint8_t mask_1 = (mask >> 1) & 0x1;

    char suffix[BUF_10] = "";

    if (mask_0 == 1) {
        strcat(suffix, "g");
    }
    if (mask_1 == 1) {
        strcat(suffix, "nzcvq");
    }

    snprintf(instr_s->spec_reg_str, sizeof(instr_s->spec_reg_str), "%s_%s", spec_reg[APSR_POS], suffix);
}

void get_banked_reg_str(uint8_t m, uint8_t m1, uint8_t R, char *banked_reg_str, int buf_sz) {
    uint8_t SYSm = (m << 4) | m1; // SYSm = m:m1 0b(m)(m1)(m1)(m1)(m1)
    uint8_t SYSm_2_0 = SYSm & 0x7;      // SYSm[2:0]
    uint8_t SYSm_4_3 = (SYSm >> 3) & 0x3;   // SYSm[4:3]

    snprintf(banked_reg_str, buf_sz, "%s", banked_reg_table[R][SYSm_2_0][SYSm_4_3]);
}


Shift decode_imm_shift(ShiftType type, uint8_t imm) {
    Shift shift;
    switch(type) {
        case LSL:
            shift.shift_t = LSL;
            shift.shift_n = imm;
            break;
        case LSR:
            shift.shift_t = LSR;
            shift.shift_n = (imm == 0) ? 32 : imm;
            break;
        case ASR:
            shift.shift_t = ASR;
            shift.shift_n = (imm == 0) ? 32 : imm;
            break;
        case ROR:
        case RRX: // doesn't need to be here but compiler complains
            if (imm == 0) {
                shift.shift_t = RRX;
                shift.shift_n = 1;
            }
            else {
                shift.shift_t = ROR;
                shift.shift_n = imm;
            }
            break;
    }

    return shift;
}

void get_shift_str(Instr *instr_s, ShiftType type, uint8_t imm) {
    instr_s->shift = decode_imm_shift(type, imm);
    if (instr_s->shift.shift_n == 0) {
        instr_s->shift_str[0] = '\0';
    }
    else {
        snprintf(instr_s->shift_str, sizeof(instr_s->shift_str), ", %s #%d", shift_codes[instr_s->shift.shift_t], instr_s->shift.shift_n);
    }
}

void print_asm_instr(Instr *instr_s) {
    print_instr_table[instr_s->igroup](instr_s);
}
