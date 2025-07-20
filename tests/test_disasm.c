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