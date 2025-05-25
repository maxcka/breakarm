#include <stdint.h>
#include "a32_info.h"

Shift decodeImmShift(ShiftType type, uint8_t imm5) {
    Shift shift;
    switch(type) {
        case LSL:
            shift.shift_t = LSL;
            shift.shift_n = imm5;
        case LSR:
            shift.shift_t = LSR;
            shift.shift_n = (imm5 == 0) ? 32 : imm5; 
        case ASR:
            shift.shift_t = ASR;
            shift.shift_n = (imm5 == 0) ? 32 : imm5;
        case ROR: // or RRX
            if (imm5 == 0) {
                shift.shift_t = RRX;
                shift.shift_n = 1;
            }
            else {
                shift.shift_t = ROR;
                shift.shift_n = imm5;
            }
    }

    return shift;
}

//
void AND_reg_instr(uint32_t instr) {
    uint8_t type = (instr >> 5) & 0x3;
    uint8_t imm5 = (instr >> 7) & 0x1F;
    Shift shift = decodeImmShift(type, imm5);
    // read A8.4.3 for pseudocode of DecodeImmShift()

    Cond c =  (instr >> 28) & 0xF; // c is condition
    uint8_t Rd = (instr >> 12) & 0xF; // 0b1111
    uint8_t Rn = (instr >> 16) & 0xF;
    uint8_t Rm = (instr >> 0) & 0xF;
    uint8_t S =  (instr >> 20) & 0x1;
}