#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "decode.h"

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

void get_imm_str(Instr *instr_s, uint16_t imm_high, uint8_t imm_low, uint8_t shift, uint8_t positive) {
    uint16_t mask = (1 << shift) - 1;
    uint16_t imm = (imm_high << shift) | (imm_low & mask);
    snprintf(instr_s->imm_str, sizeof(instr_s->imm_str), "#%s%d", (positive) ? "" : "-", imm);
}

// sys special register string
void get_sys_sr_str(Instr *instr_s, uint8_t mask) {
    if (mask == 0 || instr_s->Rn == PC) {
        instr_s->itype = TYPE_UNPRED;
        return;
    }
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
    if (mask == 0 || instr_s->Rn == PC) {
        instr_s->itype = TYPE_UNPRED;
        return;
    }
    uint8_t mask_0 = (mask >> 0) & 0x1;
    uint8_t mask_1 = (mask >> 1) & 0x1;

    char suffix[BUF_10] = "";

    if (mask_0 == 1) {
        strcat(suffix, "nzcvq");
    }
    if (mask_1 == 1) {
        strcat(suffix, "g");
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


// this function needs to be split up based on type of instruction (data-proc, etc)

// print_instr_table[instr_s.group]

void print_asm_instr(Instr *instr_s) {
    print_instr_table[instr_s->igroup](instr_s);
}
