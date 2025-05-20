Disassembler for ARM Cortex-M4 CPU

TODO:
- create loader with libelf and load .text section of binary  DONE
- disassemble with Capstone   DONE
- start making own disassembler:
- - decode opcodes
  - convert to arm32 instructions
  - add control flow
  - add symbol resolution
- write example test files:
  - for x86_64    DONE
  - for arm32     DONE
  - for thumb     DONE
  - for arm64     DONE


======================
About ARM ISAs

- Thumb - used in older architectures, 16-bit instructions
- Thumb-2 - mix of 16-bit and 32-bit instructions
- ARM32 - only 32-bit instructions
- ARM64 - only 32-bit instructions

when compiling using arm32-linux-gnueabi:
- -marm option generates ARM32 instructions
- -mthumb option generates Thumb (Thumb or Thumb-2 depending on -arch option) instructions

======================
ARM32 ISA Encoding

|31 30 29 28|27 26 25|24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5| 4 |3 2 1 0|
|  cond     |  op1   |                                                      |op |       |
