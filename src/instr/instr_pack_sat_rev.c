#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


// ----------------------------------------------------
// --- packing, unpacking, saturation, and reversal ---
// ----------------------------------------------------

void print_pack_sat_rev_instr(Instr *instr_s) {
    switch (instr_s->itype) {


        default: 
        {
            printf("UNKNOWN\n");
            break;
        }
    }
}

int process_pack_sat_rev_instr(uint32_t instr, Instr *instr_s) {


    print_asm_instr(instr_s);
    return 0;
}


// syntax: PKHBT{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, LSL #<imm>}  (tbform == FALSE)
// syntax: PKHTB{<c>}{<q>} {<Rd>,} <Rn>, <Rm> {, ASR #<imm>}  (tbform == TRUE)
int PKH_instr(uint32_t instr) {

}


// SXTAB
// UXTAB
// syntax: SXTAB16<c> <Rd>, <Rn>, <Rm>{, <rotation>}
// UXTAB16
int XTAB_instr(uint32_t instr) {

}


// SXTB
// UXTB
// syntax: SXTB16<c> <Rd>, <Rm>{, <rotation>}
// UXTB16
int XTB_instr(uint32_t instr) {

}


// syntax: SSAT<c> <Rd>, #<imm>, <Rn>{, <shift>}
// SSAT16
// USAT
// USAT16
int SAT_instr(uint32_t instr) {

}



// syntax: SEL<c> <Rd>, <Rn>, <Rm>
int SEL_instr(uint32_t instr) {
    
}

// syntax: REV<c> <Rd>, <Rm>
// REV16
int REV_instr(uint32_t instr) {
    
}

// syntax: SXTAH<c> <Rd>, <Rn>, <Rm>{, <rotation>}
// UXTAH
int XTAH_instr(uint32_t instr) {
    
}

// syntax: SXTH<c> <Rd>, <Rm>{, <rotation>}
// UXTH
int XTH_instr(uint32_t instr) {
    
}

// syntax: RBIT<c> <Rd>, <Rm>
int RBIT_instr(uint32_t instr) {

}

// syntax: REVSH<c> <Rd>, <Rm>
int REVSH_instr(uint32_t instr) {

}


