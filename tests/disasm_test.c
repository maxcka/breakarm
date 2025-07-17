__attribute__((naked))
void test_data_proc() {
    asm volatile (
        "and r2, r0, r1\n"
        "eor r2, r0, r1\n"
        "sub r2, r0, r1\n"
        "rsb r2, r0, r1\n"
        "add r2, r0, r1\n"
        "adc r2, r0, r1\n"
    );
}