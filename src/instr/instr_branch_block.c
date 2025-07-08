#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode.h"
#include "bit_matching.h"


// ---------------------------------------------------------
// --- Branch, branch with link, and block data transfer ---
// ---------------------------------------------------------

void print_branch_block_instr(Instr *instr_s) {
    switch (instr_s->itype) {


        default: 
        {
            printf("UNKNOWN\n");
            break;
        }
    }
}

int process_branch_block_instr(uint32_t instr, Instr *instr_s) {


    print_asm_instr(instr_s);
    return 0;
}


// syntax: STMDA<c> <Rn>{!}, <registers>
int STMDA_instr(uint32_t instr) {

}


// syntax: LDMDA<c> <Rn>{!}, <registers>

// syntax: STM<c> <Rn>{!}, <registers>

// syntax: LDM<c> <Rn>{!}, <registers>

// syntax: POP<c> <registers>

// syntax: STMDB<c> <Rn>{!}, <registers>

// syntax: PUSH<c> <registers>

// syntax: LDMDB<c> <Rn>{!}, <registers>

// syntax: STMIB<c> <Rn>{!}, <registers>

// syntax: LDMIB<c> <Rn>{!}, <registers>

// syntax: STM{<amode>}<c> <Rn>, <registers>^

// syntax: LDM{<amode>}<c> <Rn>, <registers_without_pc>^

// syntax: LDM{<amode>}<c> <Rn>{!}, <registers_with_pc>^

// syntax: B<c> <label>

// syntax: BL<c> <label>