Disassembler for ARM Cortex-M4 CPU

TODO:
- create loader with libbfd and load .text section of binary
- disassemble with Capstone
- start making own disassembler:
- - decode opcodes
  - convert to Thumb/Thumb-2 instructions
  - add control flow
  - add symbol resolution
