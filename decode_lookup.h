#ifndef DECODE_LOOKUP_H
#define DECODE_LOOKUP_H


#include <stdint.h>

static const char *core_reg[16] = {
    "R0", "R1", "R2", "R3",
    "R4", "R5", "R6", "R7",
    "R8", "R9", "SL", "R11",
    "IP", "SP", "LR", "PC"  // R12 can be called IP or R12
};



static const char *shift_codes[5] = {
    "LSL",
    "LSR",
    "ASR",
    "ROR",
    "RRX"
};





static const char *cond_codes[16] = {
    "EQ",
    "NE",
    "CS",
    "CC",
    "MI",
    "PL",
    "VS",
    "VC",
    "HI",
    "LS",
    "GE",
    "LT",
    "GT",
    "LE",
    ""  // AL is not printed bc it is the default
};


#endif