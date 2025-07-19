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

