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
        "SMC #0\n"             // Immediate 0–65535
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

