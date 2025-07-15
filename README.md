Disassembler for A32 ISA used in AArch32 mode


- need to add better documentation
- need to add tests with inline assembly
=====================================================

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

=======================
Adding new instructions steps:
1. create macros for checking bits in higher layers
2. for final layer, use static inline function to check bits that match the specific instruction
3. create a function in decode.c that processes the instruction
4. create a helper function that is able to process similar instructions (e.g. data-processing fn, misc fn, etc)
5. if necessary, create a case for printing the assembly instruction based on the instruction syntax
6. put the inline bit-matching function and the instruction processing function in the "proc_instr_table" lookup table
7. add a decode path for this instruction in the decode_instr function

bit_matching.h 
-> instr_*.c : *_instr() 
-> instr_*.c : process_*()
-> instr_*.c : print_*()



TODO:
- DONE data proc reg
- DONE data proc rsr 
- DONE misc
- DONE half mult
- DONE mult
- DONE sync
- DONE extra load/store
- DONE extra load/store unpriv
- DONE data proc imm
- DONE MOV, MOVT 
- DONE MSR and hints

- DONE load/store word
- DONE media part 1
- 7/6 media part 2
- 7/13 branch, branch with link, and block data transfer
- 7/13 coproc and supervisor

- 7/13 unconditional

- 7/20 testing and cleanup

