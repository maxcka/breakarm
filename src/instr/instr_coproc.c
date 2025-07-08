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


        default: 
        {
            printf("UNKNOWN\n");
            break;
        }
    }
}

int process_coproc_instr(uint32_t instr, Instr *instr_s) {


    print_asm_instr(instr_s);
    return 0;
}



int _instr(uint32_t instr) {

}

// syntax: SVC<c> #<imm24>
// syntax: STC{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
// syntax: immediate: LDC{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
// syntax: literal: LDC{L}{<c>}{<q>} <coproc>, <CRd>, [PC, #+/-<imm>]
// syntax: MCRR<c> <coproc>, <opc1>, <Rt>, <Rt2>, <CRm>
// syntax: MRRC<c> <coproc>, <opc>, <Rt>, <Rt2>, <CRm>
// syntax: CDP<c> <coproc>, <opc1>, <CRd>, <CRn>, <CRm>, <opc2>
// syntax: MCR<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}
// syntax: MRC<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}

//> UNCONDITIONAL instructions

// syntax: SRS{<amode>} SP{!}, #<mode>
// syntax: RFE{<amode>} <Rn>{!}
// syntax: BLX <label> (put in instr_branch_block.c)
// syntax: STC2{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
// syntax: LDC2{L}<c> <coproc>, <CRd>, [<Rn>, #+/-<imm>]{!}
// syntax: MCRR2<c> <coproc>, <opc1>, <Rt>, <Rt2>, <CRm>
// syntax: MRRC2<c> <coproc>, <opc>, <Rt>, <Rt2>, <CRm>
// syntax: CDP2<c> <coproc>, <opc1>, <CRd>, <CRn>, <CRm>, <opc2>
// syntax: MCR2<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}
// syntax: MRC2<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>}

