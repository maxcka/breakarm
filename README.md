Disassembler for A32 ISA used in AArch32 mode

TODO:
- create loader with libelf and load .text section of binary  DONE
- disassemble with Capstone   DONE
- start making own disassembler:
- - get machine code instructions
- - convert instructions to A32 ISA

*note: when decoding, specific instructions must be checked before more general ones

======================
About ARM ISAs

- Thumb - used in older architectures, 16-bit instructions
- Thumb-2 - mix of 16-bit and 32-bit instructions
- A32 - only 32-bit instructions
- A64 - only 32-bit instructions

when compiling using arm32-linux-gnueabi:
- -marm option generates ARM32 instructions
- -mthumb option generates Thumb (Thumb or Thumb-2 depending on -arch option) instructions

======================
ARM Core Registers

R0-R12 - 13 general-purpose 32-bit registers
SP - stack pointer (R13)
LR - link register (R14)
PC - program ocunter (R15)
