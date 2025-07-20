__attribute__((naked)) // remove function prologue/epilogue
void test_data_proc_reg() {
    asm volatile (
        "AND r2, r0, r1\n"
        "EOR r2, r0, r1\n"
        "SUB r2, r0, r1\n"
        "RSB r2, r0, r1\n"
        "ADD r2, r0, r1\n"
        "ADC r2, r0, r1\n"
        "SBC r0, r1, r2\n"
        "RSC r0, r1, r2\n"
        "TST r1, r2\n"
        "TEQ r1, r2\n"
        "CMP r1, r2\n"
        "CMN r1, r2\n"
        "ORR r0, r1, r2\n"
        "MOV r0, r1\n"
        "LSL r0, r1, #2\n"
        "LSR r0, r1, #2\n"
        "ASR r0, r1, #2\n"
        "RRX r0, r1\n"
        "ROR r0, r1, #4\n"
        "BIC r0, r1, r2\n"
        "MVN r0, r1\n"
    );
    __builtin_unreachable(); // remove nop at end
}

__attribute__((naked))
void test_data_proc_rsr() {
    asm volatile (
        // Logical operations
        "AND r4, r0, r1, LSL r2\n"
        "EOR r4, r0, r1, LSR r2\n"
        "TST r0, r1, ASR r2\n"
        "TEQ r0, r1, ROR r2\n"
        "BIC r4, r0, r1, LSL r2\n"
        "MVN r4, r1, ROR r2\n"

        // Arithmetic
        "ADD r4, r0, r1, LSL r2\n"
        "SUB r4, r0, r1, LSR r2\n"
        "RSB r4, r0, r1, ASR r2\n"
        "ADC r4, r0, r1, ROR r2\n"
        "SBC r4, r0, r1, LSL r2\n"
        "RSC r4, r0, r1, LSR r2\n"
        "CMP r0, r1, ASR r2\n"
        "CMN r0, r1, ROR r2\n"
        "ORR r4, r0, r1, LSL r2\n"
    );
    __builtin_unreachable(); // remove nop at end
}


__attribute__((naked))
void test_misc() {
    asm volatile (
        // --- is_MRS_BANKED ---
        "MRS r0, SPSR_fiq\n"          // Read SPSR of FIQ mode into r0
        // --- is_MSR_BANKED ---
        "MSR SPSR_svc, r0\n"          // Write r0 into SPSR of Supervisor mode
        // --- is_MRS (regular CPSR read) ---
        "MRS r1, CPSR\n"              // Read CPSR into r1
        // --- is_MSR_reg_app ---
        "MSR APSR_nzcvq, r1\n"            // Write flags from r1 (application-level)
        // --- is_MSR_reg_sys ---
        "MSR CPSR_cxsf, r1\n"         // Write control, extension, status, and flags fields
        // Branch and Exchange
        "BX r0\n"
        "BXJ r0\n"         // BXJ is deprecated and rare; behavior is similar to BX with additional jazelle state handling
        // Count Leading Zeros
        "CLZ r0, r1\n"     // r0 = number of leading zeros in r1
        // Branch with Link and Exchange (register)
        "BLX r0\n"
        // Saturating Arithmetic (Signed)
        "QADD r0, r1, r2\n"    // r0 = saturating r1 + r2
        "QSUB r0, r1, r2\n"    // r0 = saturating r1 - r2
        "QDADD r0, r1, r2\n"   // r0 = saturating r1 + (r2 << 1)
        "QDSUB r0, r1, r2\n"   // r0 = saturating r1 - (r2 << 1)
        // Exception Return
        "ERET\n"               // Return from exception — must be in privileged mode
        // Software Breakpoint
        "BKPT #16\n"         // Immediate value is arbitrary (8-bit)
        // Hypervisor Call (ARMv7)
        "HVC #10334\n"             // Immediate value 0–65535 (encoded as 16-bit in instruction)
        // Secure Monitor Call (TrustZone)
        "SMC #14\n"             // Immediate 0–15
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_half_mult() {
    asm volatile (
// --- is_SMLA ---
        "SMLABB r0, r1, r2, r3\n"     // r0 = r1 * r2 + r3 (Bottom x Bottom)
        "SMLABT r4, r5, r6, r7\n"     // r4 = r5 * (r6 >> 16) + r7 (Bottom x Top)
        // --- is_SMLAW ---
        "SMLAWB r8, r9, sl, fp\n"   // r8 = (r9 * r10[15:0] << 16) + r11
        "SMLAWT ip, r1, r2, r3\n"    // r12 = (r1 * r2[31:16] << 16) + r3
        // --- is_SMULW ---
        "SMULWB r4, r5, r6\n"         // r4 = (r5 * r6[15:0]) >> 16
        "SMULWT r7, r8, r9\n"         // r7 = (r8 * r9[31:16]) >> 16
        // --- is_SMLALXY ---
        "SMLALBB sl, fp, ip, sp\n" // {r10,r11} += r12[15:0] * r13[15:0]
        "SMLALBT r0, r1, r2, r3\n"     // {r0,r1} += r2[15:0] * r3[31:16]
        // --- is_SMUL ---
        "SMULBB r4, r5, r6\n"         // r4 = r5[15:0] * r6[15:0]
        "SMULTT r7, r8, r9\n"         // r7 = r8[31:16] * r9[31:16]
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_mult_mult() {
    asm volatile (
// --- is_MUL(u) ---
        "MUL r0, r1, r2\n"       // r0 = r1 * r2 (signed multiply)
        // --- is_MLA(u) ---
        "MLA r3, r4, r5, r6\n"   // r3 = r4 * r5 + r6 (signed multiply-accumulate)
        // --- is_UMAAL ---
        "UMAAL r7, r8, r9, sl\n"  // {r7, r8} = r9 * r10 + r7 + r8
        // --- is_MLS(u) ---
        "MLS fp, ip, r1, r2\n"  // r11 = r2 - (r12 * r1)
        // --- is_UMULL ---
        "UMULL r0, r1, r2, r3\n"  // {r0, r1} = r2 * r3 (unsigned 64-bit)
        // --- is_UMLAL ---
        "UMLAL r4, r5, r6, r7\n"  // {r4, r5} += r6 * r7 (unsigned accumulate)
        // --- is_SMULL ---
        "SMULL r8, r9, sl, fp\n" // {r8, r9} = r10 * r11 (signed 64-bit)
        // --- is_SMLAL ---
        "SMLAL ip, r1, r2, r3\n" // {r12, r1} += r2 * r3 (signed accumulate)
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_sync() {
    asm volatile (
        // SWP and SWPB (deprecated in ARMv7, not in ARMv8)
        "SWP r0, r1, [r2]\n"       // Swap word
        "SWPB r0, r1, [r2]\n"      // Swap byte
        // LDREX / STREX
        "LDREX r0, [r1]\n"         // Load exclusive word
        "STREX r2, r0, [r1]\n"     // Store exclusive word
        // LDREXD / STREXD
        "LDREXD r2, r3, [r1]\n"    // Load exclusive doubleword
        "STREXD r4, r2, r3, [r1]\n"// Store exclusive doubleword
        // LDREXB / STREXB
        "LDREXB r0, [r1]\n"        // Load exclusive byte
        "STREXB r2, r0, [r1]\n"    // Store exclusive byte
        // LDREXH / STREXH
        "LDREXH r0, [r1]\n"        // Load exclusive halfword
        "STREXH r2, r0, [r1]\n"    // Store exclusive halfword
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_ex_ld_str() {
    asm volatile (
        // Halfword store/load with register offset
        "STRH r0, [r1, r2]\n"      // Store halfword (reg offset)
        "LDRH r3, [r1, r2]\n"      // Load halfword (reg offset)

        // Halfword store/load with immediate offset
        "STRH r0, [r1, #4]\n"      // Store halfword (imm offset)
        "LDRH r3, [r1, #4]\n"      // Load halfword (imm offset)

        // Dual word load/store with register offset
        "LDRD r2, r3, [r4, r5]\n"  // Load doubleword (reg offset)
        "STRD r6, r7, [r4, r5]\n"  // Store doubleword (reg offset)

        // Dual word load/store with immediate offset
        "LDRD r2, r3, [r4, #8]\n"  // Load doubleword (imm offset)
        "STRD r6, r7, [r4, #8]\n"  // Store doubleword (imm offset)

        // Load signed byte with register offset
        "LDRSB r0, [r1, r2]\n"     // Load signed byte (reg offset)

        // Load signed byte with immediate offset
        "LDRSB r0, [r1, #1]\n"     // Load signed byte (imm offset)

        // Load signed halfword with register offset
        "LDRSH r0, [r1, r2]\n"     // Load signed halfword (reg offset)

        // Load signed halfword with immediate offset
        "LDRSH r0, [r1, #2]\n"     // Load signed halfword (imm offset)

        // Load/store halfword with translation (Thumb/privileged)
        "LDRHT r0, [r1], #4\n"     // Load halfword with translation
        "STRHT r2, [r3], #4\n"     // Store halfword with translation

        // Load signed byte with translation
        "LDRSBT r4, [r5], #1\n"    // Load signed byte with translation

        // Load signed halfword with translation
        "LDRSHT r6, [r7], #2\n"    // Load signed halfword with translation
    );
    __builtin_unreachable();
}

__attribute__((naked))
void test_data_proc_imm_and_imm16() {
    asm volatile (
        // Logical Instructions
        "AND r0, r1, #255\n"      // is_AND
        "EOR r0, r1, #15\n"       // is_EOR
        "ORR r0, r1, #240\n"      // is_ORR
        "BIC r0, r1, #15\n"       // is_BIC
        "MVN r0, #255\n"          // is_MVN
        "MOV r2, #1\n"          // is_MOV_imm

        // Arithmetic Instructions
        "ADD r0, r1, #100\n"      // is_ADD
        "SUB r0, r1, #100\n"      // is_SUB
        "RSB r0, r1, #100\n"      // is_RSB
        "ADC r0, r1, #100\n"      // is_ADC
        "SBC r0, r1, #100\n"      // is_SBC
        "RSC r0, r1, #100\n"      // is_RSC

        // Test / Compare
        "TST r1, #1\n"            // is_TST
        "TEQ r1, #1\n"            // is_TEQ
        "CMP r1, #100\n"          // is_CMP
        "CMN r1, #100\n"          // is_CMN

        // Move Wide (not classic data proc, but immediate)
        "MOVW r0, #4660\n"        // is_MOVW (0x1234 = 4660)
        "MOVT r0, #22136\n"       // is_MOVT (0x5678 = 22136)
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_misc_hints() {
    asm volatile (
        "NOP\n"
        "YIELD\n"
        "WFE\n"
        "WFI\n"
        "SEV\n"
        "DBG #0\n"
        "MSR APSR_nzcvq, #4026531840\n"  // Example: set N,Z,C,V flags
        "MSR SPSR_cxsf, #31\n"         // Example: change mode bits (privileged)
    );
    __builtin_unreachable();
}

__attribute__((naked))
void test_ld_str() {
    asm volatile (
        // STR (store register) immediate and register
        "STR r0, [r1, #4]\n"       // is_STR_imm
        "STR r0, [r1, r2]\n"       // is_STR_reg

        // STRT (store register unprivileged) variants
        "STRT r0, [r1], #4\n"      // STRT with post-indexed immediate
        "STRT r0, [r1], r2\n"      // STRT_2 with post-indexed register offset

        // LDR (load register) immediate and register
        "LDR r0, [r1, #4]\n"       // is_LDR_imm
        "LDR r0, [r1, r2]\n"       // is_LDR_reg

        // LDRT (load register unprivileged) variants
        "LDRT r0, [r1], #4\n"      // LDRT with post-indexed immediate
        "LDRT r0, [r1], r2\n"      // LDRT_2 with post-indexed register offset

        // STRB (store byte) immediate and register
        "STRB r0, [r1, #4]\n"      // is_STRB_imm
        "STRB r0, [r1, r2]\n"      // is_STRB_reg

        // STRBT (store byte unprivileged) variants
        "STRBT r0, [r1], #4\n"     // STRBT post-indexed immediate
        "STRBT r0, [r1], r2\n"     // STRBT_2 post-indexed register

        // LDRB (load byte) immediate and register
        "LDRB r0, [r1, #4]\n"      // is_LDRB_imm
        "LDRB r0, [r1, r2]\n"      // is_LDRB_reg

        // LDRBT (load byte unprivileged) variants
        "LDRBT r0, [r1], #4\n"     // LDRBT post-indexed immediate
        "LDRBT r0, [r1], r2\n"     // LDRBT_2 post-indexed register
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_pas() {
    asm volatile (
        // ADD16 variants
        // ADD16 not allowed on arm-cortex-a15
        "QADD16 r0, r1, r2\n"      // Saturating add halfwords
        "SADD16 r0, r1, r2\n"      // Signed saturating add halfwords

        // SUB16 variants
        // SUB16 not allowed on arm-cortex-a15
        "QSUB16 r0, r1, r2\n"      // Saturating subtract halfwords
        "SSUB16 r0, r1, r2\n"      // Signed saturating subtract halfwords

        // ADD8 variants
        // ADD8 not allowed on arm-cortex-a15
        "QADD8 r0, r1, r2\n"       // Saturating add bytes
        "SADD8 r0, r1, r2\n"       // Signed saturating add bytes

        // SUB8 variants
        // SUB8 not allowed on arm-cortex-a15
        "QSUB8 r0, r1, r2\n"       // Saturating subtract bytes
        "SSUB8 r0, r1, r2\n"       // Signed saturating subtract bytes

        // ASX and SAX (no saturation variants)
        // ASX not allowed on arm-cortex-a15
        // SAX not allowed on arm-cortex-a15
    );
    __builtin_unreachable();
}

__attribute__((naked))
void test_pusr() {
    asm volatile (
        // Pack Halfword (PKH)
        "PKHBT r0, r1, r2, LSL #16\n"      // PKHBT
        // Signed Extend and Add Byte/Halfword variants
        "SXTAB16 r0, r1, r2, ROR #8\n"     // Signed Extend and Add Byte 16
        "UXTAB16 r0, r1, r2, ROR #8\n"     // Unsigned Extend and Add Byte 16
        "SXTAB r0, r1, r2, ROR #8\n"       // Signed Extend and Add Byte
        "UXTAB r0, r1, r2, ROR #8\n"       // Unsigned Extend and Add Byte
        // Signed Extend Byte 16 / Unsigned Extend Byte 16
        "SXTB16 r0, r1, ROR #8\n"           // Signed Extend Byte 16
        "UXTB16 r0, r1, ROR #8\n"           // Unsigned Extend Byte 16
        // Signed Extend Byte / Unsigned Extend Byte
        "SXTB r0, r1\n"                     // Signed Extend Byte
        "UXTB r0, r1\n"                     // Unsigned Extend Byte
        // Saturate Signed
        "SSAT r0, #8, r1\n"                 // Saturate signed to 8 bits
        "SSAT16 r0, #8, r1\n"               // Saturate signed 16 bits halves
        // Saturate Unsigned
        "USAT r0, #8, r1\n"                 // Saturate unsigned to 8 bits
        "USAT16 r0, #8, r1\n"               // Saturate unsigned 16 bits halves
        // Select Bytes
        "SEL r0, r1, r2\n"                  // Select Bytes
        // Byte Reversals
        "REV r0, r1\n"                     // Reverse byte order in word
        "REV16 r0, r1\n"                   // Reverse bytes in each halfword
        // Signed Extend and Add Halfword
        "SXTAH r0, r1, r2\n"               // Signed Extend and Add Halfword
        "UXTAH r0, r1, r2\n"               // Unsigned Extend and Add Halfword
        // Signed Extend Halfword / Unsigned Extend Halfword
        "SXTH r0, r1\n"                    // Signed Extend Halfword
        "UXTH r0, r1\n"                    // Unsigned Extend Halfword
        // Reverse Bits
        "RBIT r0, r1\n"                    // Reverse bits in word
        // Reverse Signed Halfword Bytes
        "REVSH r0, r1\n"                   // Reverse bytes in signed halfword and sign-extend
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_signed_mult() {
    asm volatile (
        // Multiply-Accumulate Dual
        "SMLAD r0, r1, r2, r3\n"     // r0 = r3 + (r1.Lo16 * r2.Lo16) + (r1.Hi16 * r2.Hi16)
        // Multiply-Subtract Dual
        "SMLSD r0, r1, r2, r3\n"     // r0 = r3 - (r1.Lo16 * r2.Lo16) + (r1.Hi16 * r2.Hi16)
        // Unsigned Multiply-Accumulate Dual
        "SMUAD r0, r1, r2\n"         // r0 = (r1.Lo16 * r2.Lo16) + (r1.Hi16 * r2.Hi16)
        // Unsigned Multiply-Subtract Dual
        "SMUSD r0, r1, r2\n"         // r0 = (r1.Lo16 * r2.Lo16) - (r1.Hi16 * r2.Hi16)
        // Signed Integer Divide
        "SDIV r0, r1, r2\n"         // r0 = r1 / r2 (signed)
        // Unsigned Integer Divide
        "UDIV r0, r1, r2\n"         // r0 = r1 / r2 (unsigned)
        // Signed Multiply Accumulate Long Dual
        "SMLALD r0, r1, r2, r3\n"     // {r1:r0} = {r1:r0} + (r2.Lo16 * r3.Lo16) + (r2.Hi16 * r3.Hi16)
        // Signed Multiply Subtract Long Dual
        "SMLSLD r0, r1, r2, r3\n"     // {r1:r0} = {r1:r0} - (r2.Lo16 * r3.Lo16) + (r2.Hi16 * r3.Hi16)
        // Signed Most Significant Word Multiply-Accumulate
        "SMMLA r0, r1, r2, r3\n"     // r0 = ((r1 * r2) >> 32) + r3
        // Signed Most Significant Word Multiply-Subtract
        "SMMLS r0, r1, r2, r3\n"     // r0 = r3 - ((r1 * r2) >> 32)
        // Signed Most Significant Word Multiply (no accumulate)
        "SMMUL r0, r1, r2\n"         // r0 = (r1 * r2) >> 32
    );
    __builtin_unreachable();
}

__attribute__((naked))
void test_other_media() {
    asm volatile (
        // USAD8: Unsigned Sum of Absolute Differences, 8-bit
        "USAD8 r0, r1, r2\n"       // r0 = Σ|r1[i] - r2[i]| for each byte i
        // USADA8: USAD8 with accumulate
        "USADA8 r0, r1, r2, r3\n"  // r0 = r3 + USAD8(r1, r2)
        // SBFX: Signed Bit-Field Extract
        "SBFX r0, r1, #8, #4\n"    // r0 = sign_extend(r1[11:8])  // extract 4 bits from bit 8
        // BFC: Bit Field Clear
        "BFC r0, #4, #8\n"         // Clear 8 bits starting at bit 4 in r0 (i.e., r0[11:4] = 0)
        // BFI: Bit Field Insert
        "BFI r0, r1, #4, #8\n"     // Insert 8 bits of r1 into r0 at bit position 4
        // UBFX: Unsigned Bit-Field Extract
        "UBFX r0, r1, #8, #4\n"    // r0 = zero_extend(r1[11:8]) // extract 4 bits starting at bit 8
        // UDF: Permanently undefined instruction (used for traps/debugging)
        "UDF #0\n"                // Trigger undefined instruction exception
    );
    __builtin_unreachable();
}


__attribute__((naked))
void test_branch_block() {
    asm volatile (
        // STMDA: Store Multiple Decrement After (Full Descending stack)
        "STMDA r0!, {r1-r3}\n"     // Store r1-r3 to memory, starting below r0, r0 updated after
        // LDMDA: Load Multiple Decrement After
        "LDMDA r0!, {r1-r3}\n"     // Load r1-r3 from memory, starting below r0, r0 updated after
        // STMIA: Store Multiple Increment After (Full Ascending stack)
        "STM r0!, {r1-r3}\n"     // Store r1-r3 to memory, starting at r0, r0 updated after
        // LDMIA: Load Multiple Increment After
        "LDM r0!, {r1-r3}\n"     // Load r1-r3 from memory, starting at r0, r0 updated after
        // POP: Pop registers from stack (LDMIA with SP)
        "POP {r4-r6}\n"            // Load r4-r6 from stack and update SP
        // STMDB: Store Multiple Decrement Before (Empty Descending stack)
        "STMDB r0!, {r1-r3}\n"     // Store r1-r3 to memory, starting below r0-4, r0 updated after
        // PUSH: Push registers to stack (STMDB with SP)
        "PUSH {r4-r6}\n"           // Store r4-r6 to stack and update SP
        // LDMDB: Load Multiple Decrement Before
        "LDMDB r0!, {r1-r3}\n"     // Load r1-r3 from memory, starting below r0-4, r0 updated after
        // STMIB: Store Multiple Increment Before
        "STMIB r0!, {r1-r3}\n"     // Store r1-r3 to memory, starting after r0+4, r0 updated after
        // LDMIB: Load Multiple Increment Before
        "LDMIB r0!, {r1-r3}\n"     // Load r1-r3 from memory, starting after r0+4, r0 updated after
        // STM_usr: Store Multiple in User mode (from privileged mode)
        "STMIA r0!, {r1-r3}^\n"   // Store r1-r3 using user mode registers (privileged mode only)
        // LDM_usr: Load Multiple in User mode (from privileged mode)
        "LDMIA r0!, {r1-r3}^\n"   // Load r1-r3 using user mode registers (privileged mode only)
        // LDM_exc: Load Multiple with PC (return from exception)
        "LDMIA r0!, {r1-r3, pc}^\n"// Load r1-r3 and PC, and restore CPSR (used for exception return)
        // B: Branch
        // can't test here. need label                // Branch unconditionally to label
        // BL: Branch with Link (call subroutine)
        // can't test here. need label      // Branch to label and store return address in LR
    );
    __builtin_unreachable();
}

__attribute__((naked))
void test_coproc() {
    asm volatile (
        // SVC (Supervisor Call)
        "SVC #123\n"
        // STC (Store to Coprocessor)
        "STC p9, c4, [r3], #4\n"
        // LDC (Load from Coprocessor)
        "LDC p9, c4, [r3], #4\n"
        // MCRR (Move two registers to coprocessor)
        "MCRR p15, #1, r2, r3, c5\n"
        // MRRC (Move two registers from coprocessor)
        "MRRC p15, #1, r2, r3, c5\n"
        // CDP (Coprocessor Data Processing)
        "CDP p15, #1, c2, c3, c4, #5\n"
        // MCR (Move to Coprocessor from ARM register)
        "MCR p15, #0, r2, c7, c10, #5\n"
        // MRC (Move from Coprocessor to ARM register)
        "MRC p15, #0, r2, c7, c10, #5\n"
    );
    __builtin_unreachable();
}



__attribute__((naked))
void test_uncond_misc() {
    asm volatile (
        // CPS - Change Processor State
        "CPSID i\n"
        "CPSIE i\n"
        // SETEND - Set Endianness
        "SETEND LE\n"
        "SETEND BE\n"
        // PLI_imm - Preload Instruction (immediate)
        "PLI [r0, #16]\n"
        // PLDW_imm - Preload Data Write (immediate)
        "PLDW [r0, #16]\n"
        // PLD_imm - Preload Data (immediate)
        "PLD [r0, #16]\n"
        // CLREX - Clear Exclusive
        "CLREX\n"
        // DSB - Data Synchronization Barrier
        "DSB\n"
        // DMB - Data Memory Barrier
        "DMB\n"
        // ISB - Instruction Synchronization Barrier
        "ISB\n"
        // PLI_reg - Preload Instruction (register)
        "PLI [r0, r1]\n"
        // PLD_reg - Preload Data (register)
        "PLD [r0, r1]\n"
    );
    __builtin_unreachable();
}

__attribute__((naked))
void test_uncond() {
    asm volatile (
        // SRS (Store Return State)
        "SRSDB SP!, #17\n"       // store return state using DB mode, with writeback
        "SRSIA SP!, #17\n"       // store return state using IA mode
        // RFE (Return From Exception)
        "RFEDB r0!\n"
        "RFEIA r0!\n"
        // BLX immediate
        // can't be tested here. need label
        // STC2 (Store to coprocessor - extension space)
        "STC2 p1, c4, [r2], #4\n"
        // LDC2_imm (Load from coprocessor - extension space)
        "LDC2 p1, c4, [r2], #4\n"
        // MCRR2 (Move to coprocessor registers - extension)
        "MCRR2 p1, #0, r0, r1, c2\n"
        // MRRC2 (Move from coprocessor registers - extension)
        "MRRC2 p1, #0, r0, r1, c2\n"
        // CDP2 (Coprocessor data processing - extension)
        "CDP2 p1, #0, c0, c1, c2, #0\n"
        // MCR2 (Move to coprocessor - extension)
        "MCR2 p1, #0, r0, c1, c2, #0\n"
        // MRC2 (Move from coprocessor - extension)
        "MRC2 p1, #0, r0, c1, c2, #0\n"
    );
    __builtin_unreachable();
}