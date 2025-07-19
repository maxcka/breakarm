__attribute__((naked))
void test_data_proc() {
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
}